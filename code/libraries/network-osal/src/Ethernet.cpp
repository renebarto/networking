#include "network-osal/Ethernet.h"

namespace serialization {

template<>
const utility::BidirectionalMap<network::EthernetType, std::string> EnumSerializationMap<network::EthernetType>::ConversionMap = 
{
    {network::EthernetType::IP, "IP"},
    {network::EthernetType::IPV6, "IPV6"},
    {network::EthernetType::ARP, "ARP"},
    {network::EthernetType::RARP, "RARP"},
};

} // namespace serialization
