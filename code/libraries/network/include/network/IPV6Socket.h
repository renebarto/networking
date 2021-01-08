#pragma once

#include "network-osal/Socket.h"

#include "utility/Serialization.h"
#include "network/IPV6EndPoint.h"

namespace network {

class IPV6Socket
    : private Socket
{
public:
    IPV6Socket();
    IPV6Socket(SocketType socketType);
    IPV6Socket(const IPV6Socket & other);
    IPV6Socket(IPV6Socket && other);

    IPV6Socket & operator = (IPV6Socket && other);

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

    void Bind(const IPV6Address & ipAddress = IPV6Address::Any);
    void Bind(const IPV6Address & ipAddress, std::uint16_t port, std::uint32_t flowInfo = 0, std::uint32_t scopeID = 0);
    void Bind(std::uint16_t port);
    void Bind(const IPV6EndPoint & ipEndPoint);

    bool Connect(const IPV6EndPoint & serverAddress, SocketTimeout timeout);
    using Socket::Listen;
    bool Accept(IPV6Socket & connectionSocket, IPV6EndPoint & clientAddress, SocketTimeout timeout);

    void GetLocalAddress(IPV6EndPoint & ipEndPoint);
    void GetRemoteAddress(IPV6EndPoint & ipEndPoint);

    void SendTo(const IPV6EndPoint & ipEndPoint, const std::vector<uint8_t> & data, size_t bytesToSend);
    void SendTo(const IPV6Address & ipAddress, std::uint16_t port, std::uint32_t flowInfo, std::uint32_t scopeID, const std::vector<uint8_t> & data, size_t bytesToSend);
    void SendTo(const IPV6EndPoint & ipEndPoint, const uint8_t * data, size_t bytesToSend);
    void SendTo(const IPV6Address & ipAddress, std::uint16_t port, std::uint32_t flowInfo, std::uint32_t scopeID, const uint8_t * data, size_t bytesToSend);

    using Socket::Receive;
    using Socket::Send;

    std::vector<uint8_t> ReceiveFrom(IPV6EndPoint & ipEndPoint);
    std::vector<uint8_t> ReceiveFrom(IPV6Address & ipAddress, std::uint16_t & port, std::uint32_t & flowInfo, std::uint32_t & scopeID);
    size_t ReceiveFrom(IPV6EndPoint & ipEndPoint, uint8_t * data, size_t bufferSize);
    size_t ReceiveFrom(IPV6Address & ipAddress, std::uint16_t & port, std::uint32_t & flowInfo, std::uint32_t & scopeID, uint8_t * data, size_t bufferSize);
};

std::ostream & operator <<(std::ostream & stream, const IPV6Socket & value);

} // namespace network
