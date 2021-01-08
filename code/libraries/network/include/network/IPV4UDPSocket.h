#pragma once

#include "network/IPV4Socket.h"

namespace network {

class IPV4UDPSocket
    : private IPV4Socket
{
public:
    IPV4UDPSocket();
    IPV4UDPSocket(const IPV4UDPSocket & other);
    IPV4UDPSocket(IPV4UDPSocket && other);

    IPV4UDPSocket & operator = (IPV4UDPSocket && other);

    using IPV4Socket::GetHandle;
    using IPV4Socket::SetHandle;

    using IPV4Socket::Family;
    using IPV4Socket::Type;

	void Open(SocketProtocol protocol = SocketProtocol::IP);
    using IPV4Socket::Close;
    using IPV4Socket::IsOpen;

    using IPV4Socket::SetSocketOptionWithLevel;
    using IPV4Socket::GetSocketOptionWithLevel;
    using IPV4Socket::SetSocketOption;
    using IPV4Socket::GetSocketOption;

    using IPV4Socket::GetSocketOptionBool;
    using IPV4Socket::SetSocketOptionBool;
    using IPV4Socket::GetSocketOptionInt;
    using IPV4Socket::SetSocketOptionInt;

    using IPV4Socket::GetBroadcastOption;
    using IPV4Socket::SetBroadcastOption;
    using IPV4Socket::GetBlockingMode;
    using IPV4Socket::SetBlockingMode;
    using IPV4Socket::GetReuseAddress;
    using IPV4Socket::SetReuseAddress;
    using IPV4Socket::GetReceiveTimeout;
    using IPV4Socket::SetReceiveTimeout;
    using IPV4Socket::GetSendTimeout;
    using IPV4Socket::SetSendTimeout;

    void Bind(const IPV4Address & ipAddress = IPV4Address::Any);
    void Bind(const IPV4Address & ipAddress, std::uint16_t port);
    void Bind(uint16_t port);
    void Bind(const IPV4EndPoint & ipEndPoint);

    using IPV4Socket::Connect;

    using IPV4Socket::GetLocalAddress;
    using IPV4Socket::GetRemoteAddress;

    void SendTo(const IPV4EndPoint & ipEndPoint, const std::vector<uint8_t> & data, size_t bytesToSend);
    void SendTo(const IPV4Address & ipAddress, std::uint16_t port, const std::vector<uint8_t> & data, size_t bytesToSend);
    void SendTo(const IPV4EndPoint & ipEndPoint, const uint8_t * data, size_t bytesToSend);
    void SendTo(const IPV4Address & ipAddress, std::uint16_t port, const uint8_t * data, size_t bytesToSend);

    using IPV4Socket::Receive;
    using IPV4Socket::Send;

    std::vector<uint8_t> ReceiveFrom(IPV4EndPoint & ipEndPoint);
    std::vector<uint8_t> ReceiveFrom(IPV4Address & ipAddress, std::uint16_t & port);
    size_t ReceiveFrom(IPV4EndPoint & ipEndPoint, uint8_t * data, size_t bufferSize);
    size_t ReceiveFrom(IPV4Address & ipAddress, std::uint16_t & port, uint8_t * data, size_t bufferSize);
};

} // namespace network
