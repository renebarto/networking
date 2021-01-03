#include "network-osal/SocketDefinitions.h"

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
const utility::BidirectionalMap<network::SocketProtocol, std::string> EnumSerializationMap<network::SocketProtocol>::ConversionMap = 
{
    {network::SocketProtocol::IP, "IP"},
    {network::SocketProtocol::ICMP, "ICMP"},
    {network::SocketProtocol::IGMP, "IGMP"},
    {network::SocketProtocol::TCP, "TCP"},
    {network::SocketProtocol::PUP, "PUP"},
    {network::SocketProtocol::UDP, "UDP"},
    {network::SocketProtocol::IDP, "IDP"},
#if defined(PLATFORM_WINDOWS)
    {network::SocketProtocol::ND, "ND"},
#endif
    {network::SocketProtocol::Raw, "Raw"},
};

template<>
const utility::BidirectionalMap<network::SocketOptionLevel, std::string> EnumSerializationMap<network::SocketOptionLevel>::ConversionMap = 
{
#if defined(PLATFORM_LINUX)
    {network::SocketOptionLevel::IP, "IP"},
#endif
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

template<>
const utility::BidirectionalMap<network::IPSocketOption, std::string> EnumSerializationMap<network::IPSocketOption>::ConversionMap = 
{
    {network::IPSocketOption::TTL, "TTL"},
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
const PortType AnyPort = 0;

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

std::ostream & operator << (std::ostream & stream, SocketProtocol value)
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

std::ostream & operator << (std::ostream & stream, IPSocketOption value)
{
    return stream << serialization::Serialize(value);
}

} // namespace network
