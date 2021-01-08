#pragma once

#include "network-osal/Socket.h"

#include "utility/Serialization.h"
#include "network/IPV4EndPoint.h"

namespace network {

class IPV4Socket
    : private Socket
{
public:
    IPV4Socket();
    IPV4Socket(SocketType socketType);
    IPV4Socket(const IPV4Socket & other);
    IPV4Socket(IPV4Socket && other);

    IPV4Socket & operator = (IPV4Socket && other);

    using Socket::GetHandle;
    using Socket::SetHandle;

    using Socket::Family;
    using Socket::Type;

	void Open(SocketType socketType, SocketProtocol protocol = SocketProtocol::IP);
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
    void Bind(const IPV4Address & ipAddress, uint16_t port);
    void Bind(uint16_t port);
    void Bind(const IPV4EndPoint & ipEndPoint);

    bool Connect(const IPV4EndPoint & serverAddress, SocketTimeout timeout);
    using Socket::Listen;
    bool Accept(IPV4Socket & connectionSocket, IPV4EndPoint & clientAddress, SocketTimeout timeout);

    void GetLocalAddress(IPV4EndPoint & ipEndPoint);
    void GetRemoteAddress(IPV4EndPoint & ipEndPoint);

    void SendTo(const IPV4EndPoint & ipEndPoint, const std::vector<uint8_t> & data, size_t bytesToSend);
    void SendTo(const IPV4Address & ipAddress, uint16_t port, const std::vector<uint8_t> & data, size_t bytesToSend);
    void SendTo(const IPV4EndPoint & ipEndPoint, const uint8_t * data, size_t bytesToSend);
    void SendTo(const IPV4Address & ipAddress, uint16_t port, const uint8_t * data, size_t bytesToSend);

    using Socket::Receive;
    using Socket::Send;

    std::vector<uint8_t> ReceiveFrom(IPV4EndPoint & ipEndPoint);
    std::vector<uint8_t> ReceiveFrom(IPV4Address & ipAddress, uint16_t & port);
    size_t ReceiveFrom(IPV4EndPoint & ipEndPoint, uint8_t * data, size_t bufferSize);
    size_t ReceiveFrom(IPV4Address & ipAddress, uint16_t & port, uint8_t * data, size_t bufferSize);
};

std::ostream & operator <<(std::ostream & stream, const IPV4Socket & value);

} // namespace network
