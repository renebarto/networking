#pragma once

#include "network-osal/Socket.h"

#include "utility/Serialization.h"
#include "network/IPV6EndPoint.h"

namespace network {

class IPV6Socket
    : private Socket
{
public:
    IPV6Socket(ISocketAPI & socketAPI);
    IPV6Socket(ISocketAPI & socketAPI, SocketType socketType, SocketProtocol socketProtocol = SocketProtocol::IP);
    IPV6Socket(const IPV6Socket & other) = delete;
    IPV6Socket(IPV6Socket && other);

    IPV6Socket & operator = (const IPV6Socket & other) = delete;
    IPV6Socket & operator = (IPV6Socket && other);

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

    void Bind(const IPV6Address & ipAddress = IPV6Address::Any);
    void Bind(const IPV6Address & ipAddress, std::uint16_t port, std::uint32_t flowInfo = 0, std::uint32_t scopeID = 0);
    void Bind(std::uint16_t port);
    void Bind(const IPV6EndPoint & ipEndPoint);

    bool Connect(const IPV6EndPoint & serverAddress, SocketTimeout timeout);
    using Socket::Listen;
    bool Accept(IPV6Socket & connectionSocket, IPV6EndPoint & clientAddress, SocketTimeout timeout);

    void GetLocalAddress(IPV6EndPoint & ipEndPoint);
    void GetRemoteAddress(IPV6EndPoint & ipEndPoint);

    using Socket::Receive;
    using Socket::Send;
    using Socket::ReceiveBuffer;
    using Socket::ReceiveBlock;
    using Socket::SendBuffer;

    size_t ReceiveFrom(IPV6EndPoint & ipEndPoint, std::uint8_t * data, size_t bufferSize);
    bool SendTo(const IPV6EndPoint & ipEndPoint, const std::uint8_t * data, size_t bytesToSend);
    size_t ReceiveBufferFrom(IPV6EndPoint & ipEndPoint, ByteBuffer & data, size_t bufferSize);
    bool ReceiveBlockFrom(IPV6EndPoint & ipEndPoint, ByteBuffer & data, size_t bufferSize);
    bool SendBufferTo(const IPV6EndPoint & ipEndPoint, const ByteBuffer & data);
};

std::ostream & operator <<(std::ostream & stream, const IPV6Socket & value);

} // namespace network
