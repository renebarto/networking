#pragma once

#include "network-osal/Socket.h"

#include "utility/Serialization.h"

namespace network {

class IPV6Socket
    : public Socket
{
public:
    IPV6Socket();
    IPV6Socket(const IPV6Socket & other);
    IPV6Socket(IPV6Socket && other);
    virtual ~IPV6Socket();

    IPV6Socket & operator = (IPV6Socket const & other);
    IPV6Socket & operator = (IPV6Socket && other);

    void Bind(const IPV6Address & ipAddress = IPV6Address::Any);
    void Bind(const IPV6Address & ipAddress, uint16_t port);
    void Bind(uint16_t port);

    void Bind(const IPEndPoint<IPV6Address> & ipEndPoint)
    {
        Bind(ipEndPoint.IPV6Address(), ipEndPoint.GetPort());
    }

    void GetLocalAddress(IPEndPoint<IPV6Address> & ipEndPoint);
    void GetRemoteAddress(IPEndPoint<IPV6Address> & ipEndPoint);

    void SendTo(const IPEndPoint<IPV6Address> & ipEndPoint, const std::vector<uint8_t> & data, size_t bytesToSend);
    void SendTo(const IPV6Address & ipAddress, uint16_t port, const std::vector<uint8_t> & data, size_t bytesToSend);
    void SendTo(const IPEndPoint<IPV6Address> & ipEndPoint, const uint8_t * data, size_t bytesToSend);
    void SendTo(const IPV6Address & ipAddress, uint16_t port, const uint8_t * data, size_t bytesToSend);

    std::vector<uint8_t> ReceiveFrom(IPEndPoint<IPV6Address> & ipEndPoint);
    std::vector<uint8_t> ReceiveFrom(IPV6Address & ipAddress, uint16_t & port);
    size_t ReceiveFrom(IPEndPoint<IPV6Address> & ipEndPoint, uint8_t * data, size_t bufferSize);
    size_t ReceiveFrom(IPV6Address & ipAddress, uint16_t & port, uint8_t * data, size_t bufferSize);
};

void IPV6Socket<IPV4Address>::GetLocalAddress(IPEndPoint<IPV4Address> & ipEndPoint)
{
    sockaddr_in localAddress;
    socklen_t addressSize = sizeof(localAddress);
    Socket::GetLocalAddress((sockaddr *)&localAddress, &addressSize);
    ipEndPoint = IPEndPoint<IPV4Address>(IPV4Address(localAddress.sin_addr.s_addr), htons(localAddress.sin_port));
}

void IPV6Socket<IPV4Address>::GetRemoteAddress(IPEndPoint<IPV4Address> & ipEndPoint)
{
    sockaddr_in localAddress;
    socklen_t addressSize = sizeof(localAddress);
    Socket::GetRemoteAddress((sockaddr *)&localAddress, &addressSize);
    ipEndPoint = IPEndPoint<IPV4Address>(IPV4Address(localAddress.sin_addr.s_addr), htons(localAddress.sin_port));
}

inline std::ostream &
operator <<(std::ostream & stream, const IPV6Socket & value)
{
    return stream << serialization::Serialize(value, 0);
}

} // namespace network
