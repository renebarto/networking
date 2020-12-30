#include "network/Socket.h"

#include "tracing/Tracing.h"
#include "utility/Error.h"

#if defined(PLATFORM_LINUX)

#include <fcntl.h>

#elif defined(PLATFORM_WINDOWS)

#else

#error Unsupported platform

#endif

namespace network {

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

#endif

Socket::Socket()
    : m_handle(InvalidHandleValue)
#if defined(PLATFORM_WINDOWS)
    , m_isBlocking(true)
#endif
{

}

Socket::Socket(SocketFamily socketFamily, SocketType socketType)
    : m_handle(InvalidHandleValue)
#if defined(PLATFORM_WINDOWS)
    , m_isBlocking(true)
#endif
{
    Open(socketFamily, socketType);
}

Socket::Socket(Socket && other)
    : m_handle(other.m_handle)
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
Socket::Open(SocketFamily socketFamily, SocketType socketType)
{
    Close();
    Lock lock(m_mutex);
    SocketHandle result = socket(static_cast<int>(socketFamily), static_cast<int>(socketType), 0);
    if (result != InvalidHandleValue)
        m_handle = result;
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

} // namespace network