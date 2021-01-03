#pragma once

#include "network-osal/Socket.h"

#include "utility/Serialization.h"
#include "network/IPV4EndPoint.h"

namespace network {

class IPV4Socket
    : public Socket
{
public:
    IPV4Socket();
    IPV4Socket(const IPV4Socket & other);
    IPV4Socket(IPV4Socket && other);

    IPV4Socket & operator = (IPV4Socket && other);

    void Bind(const IPV4Address & ipAddress = IPV4Address::Any);
    void Bind(const IPV4Address & ipAddress, uint16_t port);
    void Bind(uint16_t port);
    void Bind(const IPV4EndPoint & ipEndPoint);

    void GetLocalAddress(IPV4EndPoint & ipEndPoint);
    void GetRemoteAddress(IPV4EndPoint & ipEndPoint);

    void SendTo(const IPV4EndPoint & ipEndPoint, const std::vector<uint8_t> & data, size_t bytesToSend);
    void SendTo(const IPV4Address & ipAddress, uint16_t port, const std::vector<uint8_t> & data, size_t bytesToSend);
    void SendTo(const IPV4EndPoint & ipEndPoint, const uint8_t * data, size_t bytesToSend);
    void SendTo(const IPV4Address & ipAddress, uint16_t port, const uint8_t * data, size_t bytesToSend);

    std::vector<uint8_t> ReceiveFrom(IPV4EndPoint & ipEndPoint);
    std::vector<uint8_t> ReceiveFrom(IPV4Address & ipAddress, uint16_t & port);
    size_t ReceiveFrom(IPV4EndPoint & ipEndPoint, uint8_t * data, size_t bufferSize);
    size_t ReceiveFrom(IPV4Address & ipAddress, uint16_t & port, uint8_t * data, size_t bufferSize);
};

inline std::ostream &
operator <<(std::ostream & stream, const IPV4Socket & value)
{
    return stream << serialization::Serialize(value, 0);
}

} // namespace network
