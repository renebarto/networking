#include "network/Interface.h"

#include <iomanip>
#include <ostream>

namespace network
{

std::ostream & operator << (std::ostream & stream, const AddressInfo & value)
{
    stream << "Address           :" << value.address << std::endl;
    stream << "Net Mask          :" << value.netmask << std::endl;
    stream << "Broadcast Address :" << value.broadcastAddress << std::endl;
    stream << "Dest Address      :" << value.destinationAddress << std::endl;
    return stream;
}

Interface::Interface(const std::string & name, bool isUp)
    : m_name(name)
    , m_isUp(isUp)
    , m_addresses()
{
}

Interface::~Interface()
{

}

} // namespace network