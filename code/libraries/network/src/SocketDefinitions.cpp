#include "network/SocketDefinitions.h"

#include "utility/EnumSerialization.h"

namespace serialization {

template<>
const utility::BidirectionalMap<network::SocketFamily, std::string> EnumSerializationMap<network::SocketFamily>::ConversionMap = 
{
    {network::SocketFamily::Any, "Any"},
    {network::SocketFamily::Unix, "Unix"},
    {network::SocketFamily::InternetV4, "InternetV4"},
    {network::SocketFamily::InternetV6, "InternetV6"},
    {network::SocketFamily::IPX, "IPX"},
#if defined(PLATFORM_LINUX)
    {network::SocketFamily::Packet, "Packet"},
#endif
};

template<>
const utility::BidirectionalMap<network::SocketType, std::string> EnumSerializationMap<network::SocketType>::ConversionMap = 
{
    {network::SocketType::Stream, "Stream"},
    {network::SocketType::Datagram, "Datagram"},
    {network::SocketType::Raw, "Raw"},
    {network::SocketType::SeqPacket, "SeqPacket"},
};

template<>
const utility::BidirectionalMap<network::SocketOptionLevel, std::string> EnumSerializationMap<network::SocketOptionLevel>::ConversionMap = 
{
    {network::SocketOptionLevel::Socket, "Socket"},
};

template<>
const utility::BidirectionalMap<network::SocketOption, std::string> EnumSerializationMap<network::SocketOption>::ConversionMap = 
{
    {network::SocketOption::Debug, "Debug"},
    {network::SocketOption::ReuseAddress, "ReuseAddress"},
    {network::SocketOption::Type, "Type"},
    {network::SocketOption::Error, "Error"},
    {network::SocketOption::DontRoute, "DontRoute"},
    {network::SocketOption::Broadcast, "Broadcast"},
    {network::SocketOption::SendBuffer, "SendBuffer"},
    {network::SocketOption::ReceiveBuffer, "ReceiveBuffer"},
#if defined(PLATFORM_LINUX)
    {network::SocketOption::SendBufferForce, "SendBufferForce"},
    {network::SocketOption::ReceiveBufferForce, "ReceiveBufferForce"},
#endif
    {network::SocketOption::KeepAlive, "KeepAlive"},
    {network::SocketOption::OutOfBandInline, "OutOfBandInline"},
#if defined(PLATFORM_LINUX)
    {network::SocketOption::NoCheck, "NoCheck"},
    {network::SocketOption::Priority, "Priority"},
#endif
    {network::SocketOption::Linger, "Linger"},
#if defined(PLATFORM_LINUX)
    {network::SocketOption::BSDCompatibility, "BSDCompatibility"},
#endif
    {network::SocketOption::ReceiveTimeout, "ReceiveTimeout"},
    {network::SocketOption::SendTimeout, "SendTimeout"},
#if defined(PLATFORM_LINUX)
    {network::SocketOption::BindToDevice, "BindToDevice"},
    {network::SocketOption::Timestamping, "Timestamping"},
#endif
};

} // namespace serialization

namespace network
{

#if defined(PLATFORM_LINUX)
const SocketHandle InvalidHandleValue = -1;
#elif defined(PLATFORM_WINDOWS)
const SocketHandle InvalidHandleValue = INVALID_SOCKET;
#endif
const SocketTimeout InfiniteTimeout = -1;

void SetIPAddress(sockaddr_in & address, const IPV4AddressType & ipAddress)
{
    address.sin_addr.s_addr = ipAddress;
}
void SetIPAddress(sockaddr_in6 & address, const IPV6AddressType & ipAddress)
{
    std::copy(ipAddress.begin(), ipAddress.end(), &address.sin6_addr.s6_addr[0]);
}
IPV4AddressType GetIPAddress(sockaddr_in const & address)
{
    return address.sin_addr.s_addr;
}
IPV6AddressType GetIPAddress(sockaddr_in6 const & address)
{
    IPV6AddressType result;
    std::copy(&address.sin6_addr.s6_addr[0], &address.sin6_addr.s6_addr[16], result.begin());
    return result;
}
bool operator == (sockaddr_in const & lhs, sockaddr_in const & rhs)
{
    return (lhs.sin_family == rhs.sin_family) &&
        (lhs.sin_port == rhs.sin_port) &&
        (lhs.sin_addr.s_addr == rhs.sin_addr.s_addr);
}
bool operator == (sockaddr_in6 const & lhs, sockaddr_in6 const & rhs)
{
    return (lhs.sin6_family == rhs.sin6_family) &&
        (lhs.sin6_port == rhs.sin6_port) &&
        (lhs.sin6_flowinfo == rhs.sin6_flowinfo) &&
        (lhs.sin6_scope_id == rhs.sin6_scope_id) &&
        std::equal(&lhs.sin6_addr.s6_addr[0], &lhs.sin6_addr.s6_addr[16], &rhs.sin6_addr.s6_addr[0]);
}

std::ostream & operator << (std::ostream & stream, SocketFamily value)
{
    return stream << serialization::Serialize(value);
}

std::ostream & operator << (std::ostream & stream, SocketType value)
{
    return stream << serialization::Serialize(value);
}

std::ostream & operator << (std::ostream & stream, SocketOptionLevel value)
{
    return stream << serialization::Serialize(value);
}

std::ostream & operator << (std::ostream & stream, SocketOption value)
{
    return stream << serialization::Serialize(value);
}

} // namespace network
