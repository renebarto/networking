#include "pcap-interface.h"

#include <iomanip>
#include <ostream>

namespace packetcapture
{

std::ostream & operator << (std::ostream & stream, const AddressInfo & value)
{
    stream << "Address           :" << value.address << std::endl;
    stream << "Net Mask          :" << value.netmask << std::endl;
    stream << "Broadcast Address :" << value.broadcastAddress << std::endl;
    stream << "Dest Address      :" << value.destinationAddress << std::endl;
    return stream;
}

Interface::Interface(const std::string & name, const std::string & description, bool isLoopback, const std::vector<AddressInfo>  & addresses)
    : m_name(name)
    , m_description(description)
    , m_isLoopback(isLoopback)
    , m_addresses(addresses)
{
}

Interface::~Interface()
{

}

} // namespace packetcapture