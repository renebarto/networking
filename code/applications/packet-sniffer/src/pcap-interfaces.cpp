#include "pcap-interfaces.h"

#include <pcap.h>
#include <net/if.h>
#include <net/ethernet.h>
#include <netinet/in.h>
#include <linux/if_packet.h>
#include <linux/if_arp.h>
#include "tracing/Logging.h"
#include "tracing/Tracing.h"
#include "utility/Assert.h"
#include "utility/GenericError.h"
#include "pcap-interface.h"

namespace packetcapture
{

static char error_buffer[PCAP_ERRBUF_SIZE]; // Size defined in pcap.h

Interfaces::Interfaces()
    : m_interfaces()
    , m_devices()
{
    pcap_if_t * devices {};

    // Find a device
    if (pcap_findalldevs(&devices, error_buffer) == -1)
    {
        std::string buffer;
        utility::Format(buffer, "Error finding devices: {}}", error_buffer);
        tracing::Tracing::Error(__FILE__, __LINE__, __func__, utility::GenericError(buffer));
    }
    else
    {
        m_devices = reinterpret_cast<void *>(devices);
    }
    ExtractInterfaceInfo();
}

bool Interfaces::HaveInterface(const std::string & interfaceName) const
{
    return (m_interfaces.find(interfaceName) != m_interfaces.end());
}

const Interface & Interfaces::GetInterface(const std::string & interfaceName) const
{
    if (m_interfaces.find(interfaceName) == m_interfaces.end())
    {
        tracing::Tracing::Throw(__FILE__, __LINE__, __func__, utility::GenericError("Cannot find interface {}", interfaceName));
    }
    return m_interfaces.at(interfaceName);
}

Interfaces::~Interfaces()
{
    if (IsValid())
    {
        pcap_freealldevs(reinterpret_cast<pcap_if_t *>(m_devices));
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
                        addressTuple = network::AddressTuple(addr, llAddress->sll_ifindex);
                    }
                    break;
                case ARPHRD_LOOPBACK:
                    {
                        ASSERT(llAddress->sll_halen == 6);
                        network::MACAddress addr(llAddress->sll_addr, llAddress->sll_ifindex);
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

void Interfaces::ExtractInterfaceInfo()
{
    pcap_if_t * devices = reinterpret_cast<pcap_if_t *>(m_devices);

    auto interface = devices;
    while (interface)
    {
        std::string name = interface->name;
        std::string description = (interface->description) ? interface->description : "";
        bool isLoopback = ((interface->flags & PCAP_IF_LOOPBACK) != 0);
        pcap_addr_t * address = interface->addresses;
        std::vector<AddressInfo> addresses;
        while (address)
        {
            AddressInfo addressInfo;
            if (address->addr)
            {
                ConvertAddressInfo(address->addr, addressInfo.address);
            }
            if (address->netmask)
            {
                ConvertAddressInfo(address->netmask, addressInfo.netmask);
            }
            if (address->broadaddr)
            {
                ConvertAddressInfo(address->broadaddr, addressInfo.broadcastAddress);
            }
            if (address->dstaddr)
            {
                ConvertAddressInfo(address->dstaddr, addressInfo.destinationAddress);
            }
            TraceInfo(__FILE__, __LINE__, __func__, serialization::Serialize(addressInfo, 0));
            addresses.push_back(addressInfo);
            address = address->next;
        }
        if (!HaveInterface(name))
        {
            m_interfaces.emplace(name, Interface(name, description, isLoopback, addresses));
        }
        else
        {
            m_interfaces[name] = Interface(name, description, isLoopback, addresses);
        }
        
        TraceInfo(__FILE__, __LINE__, __func__, "Device: {} - {} ({})", interface->name, (interface->description ? interface->description : "no description"), ((interface->flags & PCAP_IF_LOOPBACK) ? "loopback" : ""));
        interface = interface->next;
    }

}

} // namespace packetcapture