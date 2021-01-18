#pragma once

#include "network-osal/SocketDefinitions.h"

#include <mutex>

#if defined(PLATFORM_LINUX)
#include <sys/socket.h>
#endif

#include "utility/Serialization.h"

namespace network {

class ISocketAPI;

class Socket
{
private:
    ISocketAPI & m_socketAPI;
    SocketHandle m_handle;
    SocketFamily m_socketFamily;
    SocketType m_socketType;
    using Mutex = std::recursive_mutex;
    using Lock = std::lock_guard<Mutex>;
    Mutex m_mutex;
#if defined(PLATFORM_WINDOWS)
    bool m_isBlocking;
#endif

public:
    Socket(ISocketAPI & socketAPI);
	Socket(ISocketAPI & socketAPI, SocketFamily socketFamily, SocketType socketType);
	Socket(const Socket & other);
	Socket(Socket && other);
    virtual ~Socket();

	Socket & operator = (const Socket & other) = delete;
	Socket & operator = (Socket && other);

	SocketHandle GetHandle() const;
	void   SetHandle(SocketHandle handle);

    SocketFamily Family() const { return m_socketFamily; }
    SocketType Type() const { return m_socketType; }

	void Open(SocketFamily socketFamily, SocketType socketType, SocketProtocol protocol = SocketProtocol::IP);
	void Close();
	bool IsOpen();

    void SetSocketOptionWithLevel(SocketOptionLevel level, SocketOption socketOption, const void * optionValue, socklen_t optionLength);
    void GetSocketOptionWithLevel(SocketOptionLevel level, SocketOption socketOption, void * optionValue, socklen_t * optionLength);
    void SetSocketOption(SocketOption socketOption, const void * optionValue, socklen_t optionLength);
    void GetSocketOption(SocketOption socketOption, void * optionValue, socklen_t * optionLength);

    bool GetSocketOptionBool(SocketOption socketOption);
    void SetSocketOptionBool(SocketOption socketOption, bool value);
    int  GetSocketOptionInt(SocketOption socketOption);
    void SetSocketOptionInt(SocketOption socketOption, int value);

    bool GetBroadcastOption();
    void SetBroadcastOption(bool value);
    bool GetBlockingMode();
    void SetBlockingMode(bool value);
    bool GetReuseAddress();
    void SetReuseAddress(bool value);
    std::chrono::milliseconds GetReceiveTimeout();
    void                      SetReceiveTimeout(std::chrono::milliseconds timeout);
    std::chrono::milliseconds GetSendTimeout();
    void                      SetSendTimeout(std::chrono::milliseconds timeout);

    void Bind(const sockaddr * address, socklen_t addressLength);

    bool Connect(const sockaddr * serverAddress, socklen_t serverAddressLength, SocketTimeout timeout);
    void Listen(int numListeners);
    bool Accept(Socket & connectionSocket, sockaddr * clientAddress, socklen_t * clientAddressLength, SocketTimeout timeout);

    void GetLocalAddress(sockaddr * address, socklen_t * addressLength);
    void GetRemoteAddress(sockaddr * address, socklen_t * addressLength);

    std::size_t Receive(std::uint8_t * data, std::size_t bufferSize, int flags);
    bool        Send(const std::uint8_t * data, std::size_t bytesToSend, int flags);
    std::size_t ReceiveFrom(sockaddr * address, socklen_t * addressLength, std::uint8_t * data, std::size_t bufferSize);
    void        SendTo(const sockaddr * address, socklen_t addressLength, const std::uint8_t * data, std::size_t bytesToSend);
};

inline std::ostream &
operator <<(std::ostream & stream, const Socket & value)
{
    return stream << serialization::Serialize(value, 0);
}

} // namespace network

struct sockaddr;

namespace serialization {

std::string Serialize(const network::Socket & value, int width);
std::string Serialize(const sockaddr * value, socklen_t size);
std::string Serialize(sockaddr * value, socklen_t size);
std::string Serialize(const sockaddr * value, socklen_t * size);

} // namespace serialization