#pragma once

#include "network-osal/Socket.h"

#include "utility/Serialization.h"
#include "network/IPV4EndPoint.h"

namespace network {

class IPV4Socket
    : private Socket
{
public:
    IPV4Socket(ISocketAPI & socketAPI);
    IPV4Socket(ISocketAPI & socketAPI, SocketType socketType, SocketProtocol socketProtocol = SocketProtocol::IP);
    IPV4Socket(const IPV4Socket & other) = delete;
    IPV4Socket(IPV4Socket && other);

    IPV4Socket & operator = (const IPV4Socket & other) = delete;
    IPV4Socket & operator = (IPV4Socket && other);

    using Socket::GetHandle;
    using Socket::SetHandle;

    using Socket::Family;
    using Socket::Type;
    using Socket::Protocol;

	using Socket::Open;
    using Socket::Close;
    using Socket::IsOpen;

    using Socket::SetSocketOptionWithLevel;
    using Socket::GetSocketOptionWithLevel;
    using Socket::SetSocketOption;
    using Socket::GetSocketOption;

    using Socket::GetSocketOptionBool;
    using Socket::SetSocketOptionBool;
    using Socket::GetSocketOptionInt;
    using Socket::SetSocketOptionInt;

    using Socket::GetBroadcastOption;
    using Socket::SetBroadcastOption;
    using Socket::GetBlockingMode;
    using Socket::SetBlockingMode;
    using Socket::GetReuseAddress;
    using Socket::SetReuseAddress;
    using Socket::GetReceiveTimeout;
    using Socket::SetReceiveTimeout;
    using Socket::GetSendTimeout;
    using Socket::SetSendTimeout;

    void Bind(const IPV4Address & ipAddress = IPV4Address::Any);
    void Bind(const IPV4Address & ipAddress, std::uint16_t port);
    void Bind(uint16_t port);
    void Bind(const IPV4EndPoint & ipEndPoint);

    bool Connect(const IPV4EndPoint & serverAddress, SocketTimeout timeout);
    using Socket::Listen;
    bool Accept(IPV4Socket & connectionSocket, IPV4EndPoint & clientAddress, SocketTimeout timeout);

    void GetLocalAddress(IPV4EndPoint & ipEndPoint);
    void GetRemoteAddress(IPV4EndPoint & ipEndPoint);

    using Socket::Receive;
    using Socket::Send;
    using Socket::ReceiveBuffer;
    using Socket::ReceiveBlock;
    using Socket::SendBuffer;

    std::size_t ReceiveFrom(IPV4EndPoint & ipEndPoint, std::uint8_t * data, std::size_t bufferSize);
    std::size_t SendTo(const IPV4EndPoint & ipEndPoint, const std::uint8_t * data, std::size_t bytesToSend);
    std::size_t ReceiveBufferFrom(IPV4EndPoint & ipEndPoint, ByteBuffer & data, std::size_t bufferSize);
    bool ReceiveBlockFrom(IPV4EndPoint & ipEndPoint, ByteBuffer & data, std::size_t bufferSize);
    bool SendBufferTo(const IPV4EndPoint & ipEndPoint, const ByteBuffer & data);
};

std::ostream & operator <<(std::ostream & stream, const IPV4Socket & value);

} // namespace network
