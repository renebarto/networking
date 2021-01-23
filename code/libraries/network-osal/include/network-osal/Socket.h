#pragma once

#include "network-osal/SocketDefinitions.h"

#include <mutex>

#if defined(PLATFORM_LINUX)
#include <sys/socket.h>
#endif

#include "utility/Serialization.h"

namespace network {

using ByteBuffer = std::vector<std::uint8_t>;

class ISocketAPI;

class Socket
{
private:
    ISocketAPI & m_socketAPI;
    SocketHandle m_handle;
    SocketFamily m_socketFamily;
    SocketType m_socketType;
    SocketProtocol m_socketProtocol;
    using Mutex = std::recursive_mutex;
    using Lock = std::lock_guard<Mutex>;
    Mutex m_mutex;
#if defined(PLATFORM_WINDOWS)
    bool m_isBlocking;
#endif

public:
    Socket(ISocketAPI & socketAPI);
	Socket(ISocketAPI & socketAPI, SocketFamily socketFamily, SocketType socketType, SocketProtocol socketProtocol = SocketProtocol::IP);
	Socket(const Socket & other) = delete;
	Socket(Socket && other);
    virtual ~Socket();

	Socket & operator = (const Socket & other) = delete;
	Socket & operator = (Socket && other);

	SocketHandle GetHandle() const;
	void   SetHandle(SocketHandle handle);

    SocketFamily Family() const { return m_socketFamily; }
    SocketType Type() const { return m_socketType; }
    SocketProtocol Protocol() const { return m_socketProtocol; }

	void Open();
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
    bool        ReceiveBlock(ByteBuffer & data, std::size_t bufferSize, int flags);
    std::size_t ReceiveBuffer(ByteBuffer & data, std::size_t bufferSize, int flags);
    bool        SendBuffer(const ByteBuffer & data, int flags);
    std::size_t ReceiveFrom(sockaddr * address, socklen_t * addressLength, std::uint8_t * data, std::size_t bufferSize);
    bool        SendTo(const sockaddr * address, socklen_t addressLength, const std::uint8_t * data, std::size_t bytesToSend);
    bool        ReceiveBlockFrom(sockaddr * address, socklen_t * addressLength, ByteBuffer & data, std::size_t bufferSize);
    std::size_t ReceiveBufferFrom(sockaddr * address, socklen_t * addressLength, ByteBuffer & data, std::size_t bufferSize);
    bool        SendBufferTo(const sockaddr * address, socklen_t addressLength, const ByteBuffer & data);
};

inline std::ostream &
operator <<(std::ostream & stream, const Socket & value)
{
    return stream << serialization::Serialize(value.GetHandle(), 0);
}

extern const size_t BufferSize;

} // namespace network

struct sockaddr;

namespace serialization {

std::string Serialize(const network::Socket & value, int width);
std::string Serialize(const sockaddr * value, socklen_t size);
std::string Serialize(sockaddr * value, socklen_t size);
std::string Serialize(const sockaddr * value, socklen_t * size);

} // namespace serialization