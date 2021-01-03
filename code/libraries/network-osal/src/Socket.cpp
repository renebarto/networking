#include "network-osal/Socket.h"

#include <algorithm>
#include <thread>
#include "tracing/ScopedTracing.h"
#include "tracing/Tracing.h"
#include "utility/Error.h"

#if defined(PLATFORM_LINUX)

#include <fcntl.h>
#include <poll.h>

#elif defined(PLATFORM_WINDOWS)

#ifdef min
#undef min
#endif

#else

#error Unsupported platform

#endif

namespace network {

static const SocketTimeout TimeWait = 10;

#if defined(PLATFORM_LINUX)

static int GetError()
{
    return errno;
}

static std::string GetErrorString(int errorCode)
{
    return std::strerror(errorCode);
}

#elif defined(PLATFORM_WINDOWS)

static int GetError()
{
    return WSAGetLastError();
}

static std::string GetErrorString(int errorCode)
{
    char * text {};
    FormatMessageA(
        FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_IGNORE_INSERTS, 
        nullptr, static_cast<DWORD>(errorCode),
        MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
        reinterpret_cast<LPSTR>(&text), DWORD {}, nullptr);
    std::string result(text);
    LocalFree(text);
    return result;
}

class SocketInitializer
{
public:
    SocketInitializer()
        : m_data()
        , m_initialized(false)
    {
        int errorCode = WSAStartup(MAKEWORD(2, 2), &m_data);
        if (errorCode != 0)
            tracing::Tracer::Trace(__FILE__, __LINE__, __func__, utility::Error(errorCode, GetErrorString(errorCode), "socket() failed")); 
        else
            m_initialized = true;
    }
    ~SocketInitializer()
    {
        if (m_initialized)
        {
            int errorCode = WSACleanup();
            if (errorCode != 0)
                tracing::Tracer::Trace(__FILE__, __LINE__, __func__, utility::Error(errorCode, GetErrorString(errorCode), "socket() failed")); 
        }
    }
private:
    WSADATA m_data;
    bool m_initialized;
};

static SocketInitializer winsockInitalizer;

#endif

Socket::Socket()
    : m_handle(InvalidHandleValue)
    , m_socketFamily(SocketFamily::Any)
    , m_socketType(SocketType::Raw)
#if defined(PLATFORM_WINDOWS)
    , m_isBlocking(true)
#endif
{
}

Socket::Socket(SocketFamily socketFamily, SocketType socketType)
    : m_handle(InvalidHandleValue)
    , m_socketFamily(socketFamily)
    , m_socketType(socketType)
#if defined(PLATFORM_WINDOWS)
    , m_isBlocking(true)
#endif
{
    Open(socketFamily, socketType);
}

Socket::Socket(const Socket & other)
    : m_handle(other.m_handle)
    , m_socketFamily(other.m_socketFamily)
    , m_socketType(other.m_socketType)
#if defined(PLATFORM_WINDOWS)
    , m_isBlocking(true)
#endif
{
}

Socket::Socket(Socket && other)
    : m_handle(other.m_handle)
    , m_socketFamily(other.m_socketFamily)
    , m_socketType(other.m_socketType)
#if defined(PLATFORM_WINDOWS)
    , m_isBlocking(other.m_isBlocking)
#endif
{
    other.m_handle = InvalidHandleValue;
}

Socket::~Socket()
{
    Close();
}

Socket &
Socket::operator = (Socket && other)
{
    if (this != & other)
    {
        Close();
        m_handle = other.m_handle;
        m_socketFamily = other.m_socketFamily;
        m_socketType = other.m_socketType;
#if defined(PLATFORM_WINDOWS)
        m_isBlocking = other.m_isBlocking;
#endif
        other.m_handle = InvalidHandleValue;
    }
    return *this;
}

SocketHandle
Socket::GetHandle() const
{
    return m_handle;
}

void
Socket::SetHandle(SocketHandle handle)
{
    m_handle = handle;
}

void
Socket::Open(SocketFamily socketFamily, SocketType socketType, SocketProtocol protocol)
{
    Close();
    Lock lock(m_mutex);
    SocketHandle result = socket(static_cast<int>(socketFamily), static_cast<int>(socketType), static_cast<int>(protocol));
    if (result != InvalidHandleValue)
    {
        m_handle = result;
        m_socketFamily = socketFamily;
        m_socketType = socketType;
    }
    else
    {
        int errorCode = GetError();

        tracing::Tracer::Fatal(__FILE__, __LINE__, __func__, utility::Error(errorCode, GetErrorString(errorCode), "socket() failed")); 
    }
}

void
Socket::Close()
{
    Lock lock(m_mutex);
    int result = 0;
    if (m_handle != InvalidHandleValue)
    {
#if defined(PLATFORM_LINUX)
        result = close(m_handle);
#elif defined(PLATFORM_WINDOWS)
        result = closesocket(m_handle);
#endif
        m_handle = InvalidHandleValue;
    }
    if (result == -1)
    {
        int errorCode = GetError();

        tracing::Tracer::Fatal(__FILE__, __LINE__, __func__, utility::Error(errorCode, GetErrorString(errorCode), "close() failed")); 
    }
}

bool
Socket::IsOpen()
{
    return (m_handle != InvalidHandleValue);
}

void Socket::SetSocketOption(SocketOptionLevel level, SocketOption socketOption, const void * optionValue, socklen_t optionLength)
{
#if defined(PLATFORM_LINUX)
    int result = setsockopt(this->GetHandle(), static_cast<int>(level), static_cast<int>(socketOption), optionValue, optionLength);
#elif defined(PLATFORM_WINDOWS)
    int result = setsockopt(this->GetHandle(), static_cast<int>(level), static_cast<int>(socketOption), reinterpret_cast<const char *>(optionValue), optionLength);
#endif
    if (result == -1)
    {
        int errorCode = GetError();

        tracing::Tracer::Fatal(__FILE__, __LINE__, __func__, utility::Error(errorCode, GetErrorString(errorCode), "setsockopt() failed"));
    }
}

void Socket::GetSocketOption(SocketOptionLevel level, SocketOption socketOption, void * optionValue, socklen_t * optionLength)
{
#if defined(PLATFORM_LINUX)
    int result = getsockopt(this->GetHandle(), static_cast<int>(level), static_cast<int>(socketOption), optionValue, optionLength);
#elif defined(PLATFORM_WINDOWS)
    int result = getsockopt(this->GetHandle(), static_cast<int>(level), static_cast<int>(socketOption), reinterpret_cast<char *>(optionValue), optionLength);
#endif
    if (result == -1)
    {
        int errorCode = GetError();

        tracing::Tracer::Fatal(__FILE__, __LINE__, __func__, utility::Error(errorCode, GetErrorString(errorCode), "getsockopt() failed"));
    }
}

void Socket::SetSocketOption(SocketOption socketOption, const void * optionValue, socklen_t optionLength)
{
    SetSocketOption(SocketOptionLevel::Socket, socketOption, optionValue, optionLength);
}

void Socket::GetSocketOption(SocketOption socketOption, void * optionValue, socklen_t * optionLength)
{
    GetSocketOption(SocketOptionLevel::Socket, socketOption, optionValue, optionLength);
}

void Socket::SetSocketOptionBool(SocketOption socketOption, bool value)
{
    int optionValue = value ? 1 : 0;
    SetSocketOption(socketOption, &optionValue, sizeof(optionValue));
}

bool Socket::GetSocketOptionBool(SocketOption socketOption)
{
    int optionValue;
    socklen_t optionLength = sizeof(optionValue);
    GetSocketOption(socketOption, &optionValue, &optionLength);
    return optionValue != 0;
}

void Socket::SetSocketOptionInt(SocketOption socketOption, int optionValue)
{
    SetSocketOption(SocketOptionLevel::Socket, socketOption, &optionValue, sizeof(optionValue));
}

int Socket::GetSocketOptionInt(SocketOption socketOption)
{
    int optionValue;
    socklen_t optionLength = sizeof(optionValue);
    GetSocketOption(SocketOptionLevel::Socket, socketOption, &optionValue, &optionLength);
    return optionValue;
}

bool Socket::GetBroadcastOption()
{
    return GetSocketOptionBool(SocketOption::Broadcast);
}

void Socket::SetBroadcastOption(bool value)
{
    SetSocketOptionBool(SocketOption::Broadcast, value);
}

bool Socket::GetBlockingMode()
{
#if defined(PLATFORM_LINUX)
    int flags = fcntl(this->GetHandle(), F_GETFL);
    if (flags == -1)
    {
        int errorCode = GetError();

        tracing::Tracer::Fatal(__FILE__, __LINE__, __func__, utility::Error(errorCode, GetErrorString(errorCode), "fcntl() failed"));
    }
    return (flags & O_NONBLOCK) == 0;
#elif defined(PLATFORM_WINDOWS)
    return m_isBlocking;
#endif
}

void Socket::SetBlockingMode(bool value)
{
#if defined(PLATFORM_LINUX)
    int flags = fcntl(this->GetHandle(), F_GETFL);
    if (flags == -1)
    {
        int errorCode = GetError();

        tracing::Tracer::Fatal(__FILE__, __LINE__, __func__, utility::Error(errorCode, GetErrorString(errorCode), "fcntl() failed"));
    }
    int errorCode = fcntl(this->GetHandle(), F_SETFL, value ? (flags & ~O_NONBLOCK) : (flags | O_NONBLOCK));
    if (errorCode == -1)
    {
        int errorCode = GetError();

        tracing::Tracer::Fatal(__FILE__, __LINE__, __func__, utility::Error(errorCode, GetErrorString(errorCode), "fcntl() failed"));
    }
#elif defined(PLATFORM_WINDOWS)
    unsigned long mode = value ? 0ul : 1ul;
    int result = ioctlsocket(this->GetHandle(), FIONBIO, &mode);
    if (result == SOCKET_ERROR)
    {
        int errorCode = GetError();

        tracing::Tracer::Fatal(__FILE__, __LINE__, __func__, utility::Error(errorCode, GetErrorString(errorCode), "fcntl() failed"));
    }
    m_isBlocking = value;
#endif
}

bool Socket::GetReuseAddress()
{
    return GetSocketOptionBool(SocketOption::ReuseAddress);
}

void Socket::SetReuseAddress(bool value)
{
    SetSocketOptionBool(SocketOption::ReuseAddress, value);
}

std::chrono::milliseconds Socket::GetReceiveTimeout()
{
    timeval tv;
    std::memset(&tv, 0, sizeof(tv));
    socklen_t optionLength = sizeof(tv);
    GetSocketOption(SocketOption::ReceiveTimeout, &tv, &optionLength);
    return std::chrono::milliseconds(tv.tv_sec * 1000 + tv.tv_usec / 1000);
}

void Socket::SetReceiveTimeout(std::chrono::milliseconds timeout)
{
    timeval tv;
    std::memset(&tv, 0, sizeof(tv));
    tv.tv_sec = static_cast<long>(timeout.count() / 1000);
    tv.tv_usec = (timeout.count() % 1000) * 1000;
    SetSocketOption(SocketOption::ReceiveTimeout, &tv, sizeof(tv));
}

std::chrono::milliseconds Socket::GetSendTimeout()
{
    timeval tv;
    std::memset(&tv, 0, sizeof(tv));
    socklen_t optionLength = sizeof(tv);
    GetSocketOption(SocketOption::SendTimeout, &tv, &optionLength);
    return std::chrono::milliseconds(tv.tv_sec * 1000 + tv.tv_usec / 1000);
}

void Socket::SetSendTimeout(std::chrono::milliseconds timeout)
{
    timeval tv;
    std::memset(&tv, 0, sizeof(tv));
    tv.tv_sec = static_cast<long>(timeout.count() / 1000);
    tv.tv_usec = (timeout.count() % 1000) * 1000;
    SetSocketOption(SocketOption::SendTimeout, &tv, sizeof(tv));
}

void Socket::Bind(const sockaddr * address, socklen_t addressLength)
{
    int result {};
    SCOPEDTRACE("Bind", [&]{
        return serialization::Serialize(result != -1);
    });
    result = ::bind(this->GetHandle(), address, addressLength);
    if (result == -1)
    {
        int errorCode = GetError();

        tracing::Tracer::Fatal(__FILE__, __LINE__, __func__, utility::Error(errorCode, GetErrorString(errorCode), "bind() failed"));
    }
}

bool Socket::Connect(sockaddr const * serverAddress, socklen_t serverAddressLength, SocketTimeout timeout)
{
    int result {};
    SCOPEDTRACE("Connect", [&]{
        return serialization::Serialize(result != -1);
    });

    if (timeout != InfiniteTimeout)
    {
        SetBlockingMode(false);
    }
    else
    {
        SetBlockingMode(true);
    }

#if defined(PLATFORM_LINUX)
    result = ::connect(GetHandle(), serverAddress, serverAddressLength);
    if (result == -1)
    {
        int errorCode = GetError();

        // tracing::Tracer::Trace(__FILE__, __LINE__, __func__, utility::Error(errorCode, GetErrorString(errorCode), "connect() failed"));

        if ((errorCode == EINPROGRESS) || (errorCode == EALREADY))
        {
            pollfd descriptor;
            descriptor.fd = GetHandle();
            descriptor.events = POLLIN | POLLOUT;
            descriptor.revents = 0;
            int pollResult = ::poll(&descriptor, 1, timeout);
            if (pollResult == -1)
            {
                errorCode = GetError();

                // tracing::Tracer::Trace(__FILE__, __LINE__, __func__, utility::Error(errorCode, GetErrorString(errorCode), "poll() failed"));
            }
            else if (pollResult == 0)
            {
                return false;
            }
            else
            {
                if ((descriptor.revents & POLLHUP))
                    return false;
                result = 0;
            }
        }
        else if ((errorCode != EWOULDBLOCK) && (errorCode != EAGAIN))
            tracing::Tracer::Fatal(__FILE__, __LINE__, __func__, utility::Error(errorCode, GetErrorString(errorCode), "connect() failed"));
    }
#elif defined(PLATFORM_WINDOWS)
    SocketTimeout waitTime = 0;
    if (timeout != InfiniteTimeout)
    {
        waitTime = timeout;
    }

    result = ::connect(GetHandle(), serverAddress, serverAddressLength);
    if (result == -1)
    {
        int errorCode = GetError();

        // tracing::Tracer::Trace(__FILE__, __LINE__, __func__, utility::Error(errorCode, GetErrorString(errorCode), "connect() failed"));

        while ((waitTime > 0) && ((errorCode == EWOULDBLOCK) || (errorCode == WSAEINPROGRESS)))
        {
            timeval waitInterval { 0, 1000 * TimeWait };
            fd_set readfds {0, {}};
            fd_set writefds {1, {GetHandle()}};
            fd_set exceptfds {1, {GetHandle()}};
            // tracing::Tracing::Trace(tracing::TraceCategory::Information, __FILE__, __LINE__, __func__, "connect() wait, {} ms left", waitTime);
            result = select(1, &readfds, &writefds, &exceptfds, &waitInterval);
            waitTime -= std::min(waitTime, TimeWait);
            if (result == -1)
            {
                errorCode = GetError();

                tracing::Tracer::Fatal(__FILE__, __LINE__, __func__, utility::Error(errorCode, GetErrorString(errorCode), "select() failed"));
            }
            else if (result == 0)
            {
                // tracing::Tracing::Trace(tracing::TraceCategory::Information, __FILE__, __LINE__, __func__, utility::Error(errorCode, GetErrorString(errorCode), "connect() timeout"));
            }
            if (FD_ISSET(GetHandle(), &exceptfds))
            {
                errorCode = GetError();
                if (errorCode != 0)
                    tracing::Tracer::Fatal(__FILE__, __LINE__, __func__, utility::Error(errorCode, GetErrorString(errorCode), "connect() failed"));
            }
            if (FD_ISSET(GetHandle(), &writefds))
            {
                tracing::Tracing::Trace(tracing::TraceCategory::Information, __FILE__, __LINE__, __func__, "connect() success");
                errorCode = 0;
                break;
            }
        }
        if (errorCode != 0)
            tracing::Tracer::Fatal(__FILE__, __LINE__, __func__, utility::Error(errorCode, GetErrorString(errorCode), "connect() failed"));
    }
#endif

    SetBlockingMode(true);
    return (result != -1);
}

void Socket::Listen(int numListeners)
{
    int result {};
    SCOPEDTRACE("Listen", [&]{
        return serialization::Serialize(result != -1);
    });
    result = ::listen(GetHandle(), numListeners);
    if (result == -1)
    {
        int errorCode = GetError();

        tracing::Tracer::Fatal(__FILE__, __LINE__, __func__, utility::Error(errorCode, GetErrorString(errorCode), "listen() failed"));
    }
}

bool Socket::Accept(Socket & connectionSocket, sockaddr * clientAddress, socklen_t * clientAddressLength, SocketTimeout timeout)
{
    SocketHandle result = 0;

    SCOPEDTRACE("Accept", [&]{
        return serialization::Serialize(result != -1);
    });
    Lock lock(m_mutex);
    if (timeout != InfiniteTimeout)
    {
        // tracing::Tracing::Trace(tracing::TraceCategory::Information, __FILE__, __LINE__, __func__, "Accept non-blocking");
        SetBlockingMode(false);
    }
    else
    {
        // tracing::Tracing::Trace(tracing::TraceCategory::Information, __FILE__, __LINE__, __func__, "Accept blocking");
        SetBlockingMode(true);
    }

    SocketTimeout waitTime = 0;
    if (timeout != InfiniteTimeout)
    {
        waitTime = timeout;
    }

    do
    {
        result = ::accept(GetHandle(), clientAddress, clientAddressLength);
        if (result == -1)
        {
            int errorCode = GetError();

            // tracing::Tracer::Trace(__FILE__, __LINE__, __func__, utility::Error(errorCode, GetErrorString(errorCode), "accept() failed"));

            if (((errorCode == EWOULDBLOCK) || (errorCode == EAGAIN)) && (waitTime > 0))
            {
                // tracing::Tracing::Trace(tracing::TraceCategory::Information, __FILE__, __LINE__, __func__, "accept() wait, {} ms left", waitTime);
                std::this_thread::sleep_for(std::chrono::milliseconds(TimeWait));
                waitTime -= std::min(waitTime, TimeWait);
            }
            else if (errorCode == EBADF)
            {
                break;
            }
            else
                tracing::Tracer::Fatal(__FILE__, __LINE__, __func__, utility::Error(errorCode, GetErrorString(errorCode), "accept() failed"));
        }
    }
    while ((result == -1) && (waitTime > 0));

    SetBlockingMode(true);
    if (result != -1)
    {
        tracing::Tracing::Trace(tracing::TraceCategory::Information, __FILE__, __LINE__, __func__, "accept() success");
        connectionSocket.SetHandle(static_cast<SocketHandle>(result));
    }
    else
    {
        connectionSocket.Close();
    }
    return (result != -1);
}

void Socket::GetLocalAddress(sockaddr * address, socklen_t * addressLength)
{
    int result = 0;
    SCOPEDTRACE("GetLocalAddress", [&]{
        return serialization::Serialize(result != -1);
    });
    result = ::getsockname(this->GetHandle(), address, addressLength);
    if (result == -1)
    {
        int errorCode = GetError();

        tracing::Tracer::Fatal(__FILE__, __LINE__, __func__, utility::Error(errorCode, GetErrorString(errorCode), "getsockname() failed"));
    }
}

void Socket::GetRemoteAddress(sockaddr * address, socklen_t * addressLength)
{
    int result = 0;
    SCOPEDTRACE("GetRemoteAddress", [&]{
        return serialization::Serialize(result != -1);
    });
    result = ::getpeername(this->GetHandle(), address, addressLength);
    if (result == -1)
    {
        int errorCode = GetError();

        tracing::Tracer::Fatal(__FILE__, __LINE__, __func__, utility::Error(errorCode, GetErrorString(errorCode), "getpeername() failed"));
    }
}

std::size_t Socket::Receive(std::uint8_t * data, std::size_t bufferSize, int flags)
{
    std::size_t numBytes = 0;
    SCOPEDTRACE("Receive", [&]{
        return serialization::Serialize(numBytes);
    });
    try
    {
        int result = ::recv(GetHandle(), reinterpret_cast<char *>(data), static_cast<int>(bufferSize), flags);
        if (result == -1)
        {
            int errorCode = GetError();

            tracing::Tracer::Trace(__FILE__, __LINE__, __func__, utility::Error(errorCode, GetErrorString(errorCode), "recv() failed"));
            if ((errorCode != EINTR) && (errorCode != EWOULDBLOCK) && (errorCode != EAGAIN))
                tracing::Tracer::Throw(__FILE__, __LINE__, __func__, utility::Error(errorCode, GetErrorString(errorCode), "recv() failed"));
        } else if (result == 0)
        {
            Close();
        } else
        {
            numBytes = static_cast<size_t>(result);
        }
    }
    catch (const std::system_error &e)
    {
        if (e.code().value() == EBADF)
        {
            numBytes = 0;
        }
    }
    return numBytes;
}

bool Socket::Send(const std::uint8_t * data, std::size_t bytesToSend, int flags)
{
    bool result {};
    SCOPEDTRACE("Send", [&]{
        return serialization::Serialize(result);
    });
    int numBytesLeftToSend = static_cast<int>(bytesToSend);
    std::size_t offset = 0;

    while (numBytesLeftToSend > 0)
    {
        int numBytes = ::send(GetHandle(), reinterpret_cast<const char *> (data + offset), numBytesLeftToSend, flags);
        if (numBytes == -1)
        {
            int errorCode = GetError();

            tracing::Tracer::Trace(__FILE__, __LINE__, __func__, utility::Error(errorCode, GetErrorString(errorCode), "send() failed"));

            if ((errorCode == EPIPE) || (errorCode == ECONNRESET))
            {
                result = false;
                break;
            }
            
            tracing::Tracer::Throw(__FILE__, __LINE__, __func__, utility::Error(errorCode, GetErrorString(errorCode), "send() failed"));
        } else
        {
            offset += numBytes;
            numBytesLeftToSend -= numBytes;
        }
    }
    result = true;
    return result;
}

std::size_t Socket::ReceiveFrom(sockaddr * address, socklen_t * addressLength, std::uint8_t * data, std::size_t bufferSize)
{
    std::size_t numBytes = 0;
    SCOPEDTRACE("Receive", [&]{
        return serialization::Serialize(numBytes);
    });
    int result = ::recvfrom(GetHandle(), reinterpret_cast<char *>(data), static_cast<int>(bufferSize), 0, address, addressLength);
    if (result == -1)
    {
        int errorCode = GetError();

        tracing::Tracer::Trace(__FILE__, __LINE__, __func__, utility::Error(errorCode, GetErrorString(errorCode), "recvfrom() failed"));

        if ((errorCode != EINTR) && (errorCode != EWOULDBLOCK) && (errorCode != EAGAIN))
            tracing::Tracer::Throw(__FILE__, __LINE__, __func__, utility::Error(errorCode, GetErrorString(errorCode), "recv() failed"));
        result = 0;
    }

    numBytes = static_cast<std::size_t>(result);
    return numBytes;
}

void Socket::SendTo(const sockaddr * address, socklen_t addressLength, const std::uint8_t *data, std::size_t bytesToSend)
{
    int result {};
    SCOPEDTRACE("Send", [&]{
        return serialization::Serialize(result != -1);
    });
    result = ::sendto(GetHandle(), reinterpret_cast<const char *>(data), static_cast<int>(bytesToSend), 0, address, addressLength);
    if (result == -1)
    {
        int errorCode = GetError();

        tracing::Tracer::Throw(__FILE__, __LINE__, __func__, utility::Error(errorCode, GetErrorString(errorCode), "sendto() failed"));
    }
}

} // namespace network

namespace serialization {

void Serialize(const network::Socket & value, int width)
{
    Serialize(static_cast<int>(value.GetHandle(), width));
}

} // namespace serialization