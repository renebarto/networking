#include "network/AddressTuple.h"

namespace network {

std::ostream & operator << (std::ostream & stream, const AddressTuple & value)
{
    switch (value.m_type)
    {
        case network::AddressType::Packet:
            return stream << "MAC : " << value.m_macAddress;
        case network::AddressType::IPV4:
            return stream << "IPV4: " << value.m_ipV4Address;
        case network::AddressType::IPV6:
            return stream << "IPV6: " << value.m_ipV6Address;
        default:
            return stream << "Invalid address";
    }
}

} // namespace network