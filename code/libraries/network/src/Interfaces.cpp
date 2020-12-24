#include "network/Interfaces.h"

#include <net/if.h>
// #include <net/ethernet.h>
#include <netinet/in.h>
#include <linux/if_packet.h>
#include <linux/if_arp.h>
#include "tracing/Tracing.h"
#include "utility/Assert.h"
#include <ifaddrs.h>
#include "utility/Endian.h"
#include "utility/Error.h"
#include "utility/GenericError.h"
#include "network/Interface.h"

namespace network
{

Interfaces::Interfaces()
    : m_interfacesMap()
    , m_interfaces()
{
    ifaddrs * interfaces {};

    // Get all NICs
    if (getifaddrs(&interfaces) == -1)
    {
        tracing::Tracer::Trace(__FILE__, __LINE__, __func__, utility::Error(errno, strerror(errno), "Cannot determine network interfaces"));
    }
    else
    {
        m_interfaces = reinterpret_cast<void *>(interfaces);
    }
    ExtractInterfaceInfo();
}

bool Interfaces::HaveInterface(const std::string & interfaceName) const
{
    return (m_interfacesMap.find(interfaceName) != m_interfacesMap.end());
}

const Interface & Interfaces::GetInterface(const std::string & interfaceName) const
{
    if (m_interfacesMap.find(interfaceName) == m_interfacesMap.end())
    {
        tracing::Tracer::Throw(__FILE__, __LINE__, __func__, utility::GenericError("Cannot find interface {}", interfaceName));
    }
    return m_interfacesMap.at(interfaceName);
}

Interfaces::~Interfaces()
{
    if (IsValid())
    {
        freeifaddrs(reinterpret_cast<ifaddrs *>(m_interfaces));
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
                        network::MACAddress addr(llAddress->sll_addr, llAddress->sll_ifindex);
                        addressTuple = network::AddressTuple(addr);
                    }
                    break;
                case ARPHRD_LOOPBACK:
                    {
                        ASSERT(llAddress->sll_halen == 6);
                        network::MACAddress addr(llAddress->sll_addr, llAddress->sll_ifindex);
                        addressTuple = network::AddressTuple(addr);
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
            network::IPV4Address addr(utility::FromNetworkByteOrder(ipV4Address->sin_addr.s_addr));
            addressTuple = network::AddressTuple(addr);
        }
        break;
    case AF_INET6:
        {
            auto ipV6Address = reinterpret_cast<const struct sockaddr_in6 *>(address);
            network::IPV6Address addr(ipV6Address->sin6_addr.s6_addr);
            addressTuple = network::AddressTuple(addr);
        }
        break;
    default:
        addressTuple = network::AddressTuple(network::AddressType::Invalid);
        break;
    }
}

void Interfaces::ExtractInterfaceInfo()
{
    ifaddrs * interfaces = reinterpret_cast<ifaddrs *>(m_interfaces);

    auto interface = interfaces;
    while (interface)
    {
        std::string name = interface->ifa_name;
        bool isUp = ((interface->ifa_flags & IFF_UP) != 0);
        bool hasBroadcastAddress = ((interface->ifa_flags & IFF_BROADCAST) != 0);
        bool hasDestinationAddress = ((interface->ifa_flags & IFF_POINTOPOINT) != 0);
        // void * address = interface->ifa_data;
        if (!HaveInterface(name))
        {
            m_interfacesMap.emplace(name, Interface(name, isUp));
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
        interface = interface->ifa_next;
    }
}

} // namespace network