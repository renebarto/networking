#pragma once

#include "network/IPV6Socket.h"

namespace network {

class IPV6UDPSocket
    : private IPV6Socket
{
public:
    IPV6UDPSocket(ISocketAPI & socketAPI);
    IPV6UDPSocket(const IPV6UDPSocket & other);
    IPV6UDPSocket(IPV6UDPSocket && other);

    IPV6UDPSocket & operator = (IPV6UDPSocket && other);

    using IPV6Socket::GetHandle;
    using IPV6Socket::SetHandle;

    using IPV6Socket::Family;
    using IPV6Socket::Type;

	void Open(SocketProtocol protocol = SocketProtocol::IP);
    using IPV6Socket::Close;
    using IPV6Socket::IsOpen;

    using IPV6Socket::SetSocketOptionWithLevel;
    using IPV6Socket::GetSocketOptionWithLevel;
    using IPV6Socket::SetSocketOption;
    using IPV6Socket::GetSocketOption;

    using IPV6Socket::GetSocketOptionBool;
    using IPV6Socket::SetSocketOptionBool;
    using IPV6Socket::GetSocketOptionInt;
    using IPV6Socket::SetSocketOptionInt;

    using IPV6Socket::GetBroadcastOption;
    using IPV6Socket::SetBroadcastOption;
    using IPV6Socket::GetBlockingMode;
    using IPV6Socket::SetBlockingMode;
    using IPV6Socket::GetReuseAddress;
    using IPV6Socket::SetReuseAddress;
    using IPV6Socket::GetReceiveTimeout;
    using IPV6Socket::SetReceiveTimeout;
    using IPV6Socket::GetSendTimeout;
    using IPV6Socket::SetSendTimeout;

    void Bind(const IPV6Address & ipAddress = IPV6Address::Any);
    void Bind(const IPV6Address & ipAddress, std::uint16_t port, std::uint32_t flowInfo = 0, std::uint32_t scopeID = 0);
    void Bind(std::uint16_t port);
    void Bind(const IPV6EndPoint & ipEndPoint);

    using IPV6Socket::Connect;

    using IPV6Socket::GetLocalAddress;
    using IPV6Socket::GetRemoteAddress;

    void SendTo(const IPV6EndPoint & ipEndPoint, const std::vector<uint8_t> & data, size_t bytesToSend);
    void SendTo(const IPV6Address & ipAddress, std::uint16_t port, std::uint32_t flowInfo, std::uint32_t scopeID, const std::vector<uint8_t> & data, size_t bytesToSend);
    void SendTo(const IPV6EndPoint & ipEndPoint, const uint8_t * data, size_t bytesToSend);
    void SendTo(const IPV6Address & ipAddress, std::uint16_t port, std::uint32_t flowInfo, std::uint32_t scopeID, const uint8_t * data, size_t bytesToSend);

    using IPV6Socket::Receive;
    using IPV6Socket::Send;

    std::vector<uint8_t> ReceiveFrom(IPV6EndPoint & ipEndPoint);
    std::vector<uint8_t> ReceiveFrom(IPV6Address & ipAddress, std::uint16_t & port, std::uint32_t & flowInfo, std::uint32_t & scopeID);
    size_t ReceiveFrom(IPV6EndPoint & ipEndPoint, uint8_t * data, size_t bufferSize);
    size_t ReceiveFrom(IPV6Address & ipAddress, std::uint16_t & port, std::uint32_t & flowInfo, std::uint32_t & scopeID, uint8_t * data, size_t bufferSize);
};

} // namespace network
