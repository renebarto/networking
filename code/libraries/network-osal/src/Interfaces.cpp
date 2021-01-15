#include "network-osal/Interfaces.h"

#if defined(PLATFORM_LINUX)

#include <net/if.h>
#include <netinet/in.h>
#include <linux/if_packet.h>
#include <linux/if_arp.h>
#include <ifaddrs.h>

#elif defined(PLATFORM_WINDOWS)

#pragma warning(disable: 5039)
#include <winsock2.h>
#include <iphlpapi.h>
#pragma warning(disable: 4365)
#include <ws2ipdef.h>
#pragma warning(default: 4365)
#pragma warning(default: 5039)

#if defined(interface)
#undef interface
#endif

#else

#error Unsupported platform

#endif

#include <vector>
#include "osal/StringConversion.h"
#include "tracing/Logging.h"
#include "tracing/Tracing.h"
#include "utility/Assert.h"
#include "utility/Endian.h"
#include "utility/Error.h"
#include "utility/GenericError.h"
#include "network-osal/Interface.h"
#include "network-osal/Network.h"

namespace network
{

#if defined(PLATFORM_LINUX)

static const std::string LocalLoopbackInterface = "lo";

Interfaces::Interfaces()
    : m_interfacesMap()
{
    ifaddrs * interfaces {};

    // Get all NICs
    if (getifaddrs(&interfaces) == -1)
    {
        tracing::Logging::Error(__FILE__, __LINE__, __func__, utility::Error(errno, strerror(errno), "Cannot determine network interfaces"));
    }
    else
    {
        ExtractInterfaceInfo(reinterpret_cast<void *>(interfaces));
        freeifaddrs(reinterpret_cast<ifaddrs *>(interfaces));
    }
}

Interfaces::Interfaces(const std::string & interfaceName)
    : m_interfacesMap()
{
    ifaddrs * interfaces {};

    // Get all NICs
    if (getifaddrs(&interfaces) == -1)
    {
        tracing::Logging::Error(__FILE__, __LINE__, __func__, utility::Error(errno, strerror(errno), "Cannot determine network interfaces"));
    }
    else
    {
        ExtractInterfaceInfo(reinterpret_cast<void *>(interfaces), interfaceName);
        freeifaddrs(reinterpret_cast<ifaddrs *>(interfaces));
    }
}

static void ConvertAddressInfo(const struct sockaddr * address, network::AddressTuple & addressTuple)
{
    switch (address->sa_family)
    {
    case AF_PACKET:
        {
            auto llAddress = reinterpret_cast<const struct sockaddr_ll *>(address);
            switch (llAddress->sll_hatype)
            {
                case ARPHRD_ETHER:
                    {
                        ASSERT(llAddress->sll_halen == 6);
                        network::MACAddress addr(llAddress->sll_addr, llAddress->sll_halen);
                        addressTuple = network::AddressTuple(addr, llAddress->sll_ifindex);
                    }
                    break;
                case ARPHRD_LOOPBACK:
                    {
                        ASSERT(llAddress->sll_halen == 6);
                        network::MACAddress addr(llAddress->sll_addr, llAddress->sll_halen);
                        addressTuple = network::AddressTuple(addr, llAddress->sll_ifindex);
                    }
                    break;
                default:
                    addressTuple = network::AddressTuple(network::AddressType::Invalid);
            }
        }
        break;
    case AF_INET:
        {
            auto ipV4Address = reinterpret_cast<const struct sockaddr_in *>(address);
            network::IPV4Address addr(ipV4Address->sin_addr);
            addressTuple = network::AddressTuple(addr);
        }
        break;
    case AF_INET6:
        {
            auto ipV6Address = reinterpret_cast<const struct sockaddr_in6 *>(address);
            network::IPV6Address addr(ipV6Address->sin6_addr);
            addressTuple = network::AddressTuple(addr);
        }
        break;
    default:
        addressTuple = network::AddressTuple(network::AddressType::Invalid);
        break;
    }
}

void Interfaces::ExtractInterfaceInfo(void * info, const std::string & interfaceName)
{
    ifaddrs * interfaces = reinterpret_cast<ifaddrs *>(info);

    auto interface = interfaces;
    while (interface)
    {
        std::string name = interface->ifa_name;
        if (interfaceName.empty() || (name == interfaceName))
        {
            bool isUp = ((interface->ifa_flags & IFF_UP) != 0);
            bool isLoopback = ((interface->ifa_flags & IFF_LOOPBACK) != 0);
            bool hasBroadcastAddress = ((interface->ifa_flags & IFF_BROADCAST) != 0);
            bool hasDestinationAddress = ((interface->ifa_flags & IFF_POINTOPOINT) != 0);
            if (!HaveInterface(name))
            {
                m_interfacesMap.emplace(name, Interface(name, isUp, isLoopback));
            }
            Interface & nic = m_interfacesMap[name];
            std::vector<AddressInfo> & addresses = nic.Addresses();
            if (interface->ifa_addr)
            {
                AddressInfo addressInfo;
                ConvertAddressInfo(interface->ifa_addr, addressInfo.address);
                if (interface->ifa_netmask)
                {
                    ConvertAddressInfo(interface->ifa_netmask, addressInfo.netmask);
                }
                if (hasBroadcastAddress && interface->ifa_ifu.ifu_broadaddr)
                {
                    ConvertAddressInfo(interface->ifa_ifu.ifu_broadaddr, addressInfo.broadcastAddress);
                }
                if (hasDestinationAddress && interface->ifa_ifu.ifu_dstaddr)
                {
                    ConvertAddressInfo(interface->ifa_ifu.ifu_dstaddr, addressInfo.destinationAddress);
                }
                TraceInfo(__FILE__, __LINE__, __func__, serialization::Serialize(addressInfo, 0));
                addresses.push_back(addressInfo);
            }
            
            TraceInfo(__FILE__, __LINE__, __func__, "Device: {} ({})", name, (isUp ? "Up" : "Down"));
        }
        
        interface = interface->ifa_next;
    }
}

#elif defined(PLATFORM_WINDOWS)

static const std::size_t InitialBufferSize = 16384;

Interfaces::Interfaces()
    : m_interfacesMap()
{
    std::size_t numAdapters = (InitialBufferSize + sizeof(IP_ADAPTER_ADDRESSES) - 1) / sizeof(IP_ADAPTER_ADDRESSES);
    int numIterations = 0;
    static int MaxIterations = 3;
    IP_ADAPTER_ADDRESSES * adapterInfo = new IP_ADAPTER_ADDRESSES[numAdapters];
    DWORD result {};

    do
    {
        unsigned long adapterInfoSize = static_cast<unsigned long>(numAdapters * sizeof(IP_ADAPTER_ADDRESSES));
        // Get all NICs
        result = GetAdaptersAddresses(AF_UNSPEC, GAA_FLAG_INCLUDE_PREFIX, nullptr, adapterInfo, &adapterInfoSize);
        if (result == ERROR_BUFFER_OVERFLOW)
        {
            numAdapters = (adapterInfoSize + sizeof(IP_ADAPTER_ADDRESSES) - 1) / sizeof(IP_ADAPTER_ADDRESSES);
            delete [] adapterInfo;
            adapterInfo = new IP_ADAPTER_ADDRESSES[numAdapters];
        }
    }
    while ((result == ERROR_BUFFER_OVERFLOW) && (numIterations < MaxIterations));

    if (result != NO_ERROR)
    {
        int errorCode = static_cast<int>(result);
        tracing::Logging::Error(__FILE__, __LINE__, __func__, utility::Error(errorCode, GetErrorString(errorCode), "Cannot determine network interfaces"));
    }
    else
    {
        ExtractInterfaceInfo(adapterInfo);
    }
    delete [] adapterInfo;
}

Interfaces::Interfaces(const std::string & interfaceName)
    : m_interfacesMap()
{
    std::size_t numAdapters = (InitialBufferSize + sizeof(IP_ADAPTER_ADDRESSES) - 1) / sizeof(IP_ADAPTER_ADDRESSES);
    int numIterations = 0;
    static int MaxIterations = 3;
    IP_ADAPTER_ADDRESSES * adapterInfo = new IP_ADAPTER_ADDRESSES[numAdapters];
    DWORD result {};

    do
    {
        unsigned long adapterInfoSize = static_cast<unsigned long>(numAdapters * sizeof(IP_ADAPTER_ADDRESSES));
        // Get all NICs
        result = GetAdaptersAddresses(AF_UNSPEC, GAA_FLAG_INCLUDE_PREFIX, nullptr, adapterInfo, &adapterInfoSize);
        if (result == ERROR_BUFFER_OVERFLOW)
        {
            numAdapters = (adapterInfoSize + sizeof(IP_ADAPTER_ADDRESSES) - 1) / sizeof(IP_ADAPTER_ADDRESSES);
            delete [] adapterInfo;
            adapterInfo = new IP_ADAPTER_ADDRESSES[numAdapters];
        }
    }
    while ((result == ERROR_BUFFER_OVERFLOW) && (numIterations < MaxIterations));

    if (result != NO_ERROR)
    {
        int errorCode = static_cast<int>(result);
        tracing::Logging::Error(__FILE__, __LINE__, __func__, utility::Error(errorCode, GetErrorString(errorCode), "Cannot determine network interfaces"));
    }
    else
    {
        ExtractInterfaceInfo(adapterInfo, interfaceName);
    }
    delete [] adapterInfo;
}

static void ConvertMACAddressInfo(const std::uint8_t * data, std::size_t size, int interfaceIndex, network::AddressTuple & addressTuple)
{
    ASSERT(size == 6);
    network::MACAddress addr(data, static_cast<std::uint8_t>(size));
    addressTuple = network::AddressTuple(addr, interfaceIndex);
}

static void ConvertAddressInfo(const struct sockaddr * address, std::uint32_t maskBits, network::AddressInfo & addressInfo)
{
    switch (address->sa_family)
    {
    case AF_INET:
        {
            auto ipV4Address = reinterpret_cast<const struct sockaddr_in *>(address);
            network::IPV4Address addr(ipV4Address->sin_addr);
            addressInfo.address = network::AddressTuple(addr);
            ULONG netMask {};
            ConvertLengthToIpv4Mask(maskBits, &netMask);
            in_addr netMaskAddress;
            netMaskAddress.s_addr = static_cast<std::uint32_t>(netMask);
            addressInfo.netmask = IPV4Address(netMaskAddress);
            in_addr broadcastAddress;
            broadcastAddress.s_addr = ~netMask | ipV4Address->sin_addr.s_addr;
            addressInfo.broadcastAddress = IPV4Address(broadcastAddress);
        }
        break;
    case AF_INET6:
        {
            auto ipV6Address = reinterpret_cast<const struct sockaddr_in6 *>(address);
            network::IPV6Address addr(ipV6Address->sin6_addr);
            addressInfo.address = network::AddressTuple(addr);
        }
        break;
    default:
        addressInfo.address = network::AddressTuple(network::AddressType::Invalid);
        break;
    }
}

void Interfaces::ExtractInterfaceInfo(void * info, const std::string & interfaceName)
{
    IP_ADAPTER_ADDRESSES * interfaces = reinterpret_cast<IP_ADAPTER_ADDRESSES *>(info);

    auto interface = interfaces;
    while (interface)
    {
        std::string name = osal::WStringToString(interface->FriendlyName);
        if (interfaceName.empty() || (name == interfaceName))
        {
            auto interfaceIndex = interface->IfIndex;
            (void)interfaceIndex;
            bool isLoopback = (interface->IfType == IF_TYPE_SOFTWARE_LOOPBACK);
            bool isUp = (interface->OperStatus == IfOperStatusUp);
            if (!HaveInterface(name))
            {
                m_interfacesMap.emplace(name, Interface(name, isUp, isLoopback));
            }
            Interface & nic = m_interfacesMap[name];
            std::vector<AddressInfo> & addresses = nic.Addresses();
            if (interface->FirstUnicastAddress)
            {
                int i = 0;
                auto address = interface->FirstUnicastAddress;
                while (address)
                {
                    AddressInfo addressInfo;
                    ConvertAddressInfo(address->Address.lpSockaddr, address->OnLinkPrefixLength, addressInfo);
                    addresses.push_back(addressInfo);
                    // tracing::Tracing::Trace(tracing::TraceCategory::Information, __FILE__, __LINE__, __func__, "Unicast address {}: {}", i, addressInfo);
                    ++i;
                    address = address ->Next;
                }
            }
            if (interface->FirstAnycastAddress)
            {
                int i = 0;
                auto address = interface->FirstAnycastAddress;
                while (address)
                {
                    AddressInfo addressInfo;
                    ConvertAddressInfo(address->Address.lpSockaddr, 0, addressInfo);
                    addresses.push_back(addressInfo);
                    // tracing::Tracing::Trace(tracing::TraceCategory::Information, __FILE__, __LINE__, __func__, "Anycast address {}: {}", i, addressInfo);
                    ++i;
                    address = address ->Next;
                }
            }
            std::vector<AddressInfo> & multicastAddresses = nic.MulticastAddresses();
            if (interface->FirstMulticastAddress)
            {
                int i = 0;
                auto address = interface->FirstMulticastAddress;
                while (address)
                {
                    AddressInfo addressInfo;
                    ConvertAddressInfo(address->Address.lpSockaddr, 0, addressInfo);
                    multicastAddresses.push_back(addressInfo);
                    // tracing::Tracing::Trace(tracing::TraceCategory::Information, __FILE__, __LINE__, __func__, "Multicast address {}: {}", i, addressInfo);
                    ++i;
                    address = address ->Next;
                }
            }
            if (interface->PhysicalAddressLength != 0)
            {
                AddressInfo addressInfo;
                ConvertMACAddressInfo(interface->PhysicalAddress, interface->PhysicalAddressLength, static_cast<int>(interface->IfIndex), addressInfo.address);
                addresses.push_back(addressInfo);
                // tracing::Tracing::Trace(tracing::TraceCategory::Information, __FILE__, __LINE__, __func__, "Interface {} has MAC address {}", name, addressInfo);
            }
            
            TraceMessage(__FILE__, __LINE__, __func__, "Device: {}", nic);
        }
        
        interface = interface->Next;
    }
}

#endif

bool Interfaces::HaveInterface(const std::string & interfaceName) const
{
    return (m_interfacesMap.find(interfaceName) != m_interfacesMap.end());
}

bool Interfaces::HaveLocalLoopbackInterface(std::string & interfaceName) const
{
    for (auto const & entry : m_interfacesMap)
    {
        if (entry.second.IsLoopback())
        {
            interfaceName = entry.first;
            return true;   
        }
    }
    return false;
}

const Interface & Interfaces::GetInterface(const std::string & interfaceName) const
{
    if (m_interfacesMap.find(interfaceName) == m_interfacesMap.end())
    {
        tracing::Logging::Throw(__FILE__, __LINE__, __func__, utility::GenericError("Cannot find interface {}", interfaceName));
    }
    return m_interfacesMap.at(interfaceName);
}

std::map<std::string, Interface> Interfaces::GetLocalLoopbackInterfaces() const
{
    std::map<std::string, Interface> result;
    for (auto const & entry : m_interfacesMap)
    {
        if (entry.second.IsLoopback())
        {
            result.emplace(entry);
        }
    }
    return result;
}

std::map<std::string, Interface> Interfaces::GetIPV4Interfaces() const
{
    std::map<std::string, Interface> result;
    for (auto const & entry : m_interfacesMap)
    {
        bool foundIPV4Address = false;
        auto addresses = entry.second.Addresses();
        for (auto const & addressInfo : addresses)
        {
            if (addressInfo.address.IsIPV4Address())
            {
                foundIPV4Address = true;
                break;
            }
        }
        if (foundIPV4Address)
            result.emplace(entry);
    }
    return result;
}

std::map<std::string, Interface> Interfaces::GetIPV6Interfaces() const
{
    std::map<std::string, Interface> result;
    for (auto const & entry : m_interfacesMap)
    {
        bool foundIPV6Address = false;
        auto addresses = entry.second.Addresses();
        for (auto const & addressInfo : addresses)
        {
            if (addressInfo.address.IsIPV6Address())
            {
                foundIPV6Address = true;
                break;
            }
        }
        if (foundIPV6Address)
            result.emplace(entry);
    }
    return result;
}

} // namespace network
