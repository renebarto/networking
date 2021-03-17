#include "network-osal/SocketAPI.h"

#include "tracing/Tracing.h"
#include "utility/Error.h"
#include "network-osal/Network.h"

#if defined(PLATFORM_LINUX)

#include <fcntl.h>

#elif defined(PLATFORM_WINDOWS)

#else

#error Unsupported platform

#endif

namespace network {

SocketAPI::SocketAPI()
{
}

SocketAPI::~SocketAPI()
{
}

SocketHandle SocketAPI::Open(SocketFamily socketFamily, SocketType socketType, SocketProtocol protocol)
{
    return ::socket(static_cast<int>(socketFamily), static_cast<int>(socketType), static_cast<int>(protocol));
}

int SocketAPI::Close(SocketHandle handle)
{
#if defined(PLATFORM_LINUX)
    return ::close(handle);
#elif defined(PLATFORM_WINDOWS)
    return ::closesocket(handle);
#endif
}

int SocketAPI::SetSocketOption(SocketHandle handle, SocketOptionLevel level, SocketOption socketOption, const void * optionValue, socklen_t optionLength)
{
#if defined(PLATFORM_LINUX)
    return ::setsockopt(handle, static_cast<int>(level), static_cast<int>(socketOption), optionValue, optionLength);
#elif defined(PLATFORM_WINDOWS)
    return ::setsockopt(handle, static_cast<int>(level), static_cast<int>(socketOption), reinterpret_cast<const char *>(optionValue), optionLength);
#endif
}

int SocketAPI::GetSocketOption(SocketHandle handle, SocketOptionLevel level, SocketOption socketOption, void * optionValue, socklen_t * optionLength)
{
#if defined(PLATFORM_LINUX)
    return ::getsockopt(handle, static_cast<int>(level), static_cast<int>(socketOption), optionValue, optionLength);
#elif defined(PLATFORM_WINDOWS)
    return ::getsockopt(handle, static_cast<int>(level), static_cast<int>(socketOption), reinterpret_cast<char *>(optionValue), optionLength);
#endif
}

int SocketAPI::SetBlockingMode(SocketHandle handle, bool value)
{
#if defined(PLATFORM_LINUX)
    int flags = fcntl(handle, F_GETFL);
    if (flags == -1)
    {
        return flags;
    }
    return ::fcntl(handle, F_SETFL, value ? (flags & ~O_NONBLOCK) : (flags | O_NONBLOCK));
#elif defined(PLATFORM_WINDOWS)
    unsigned long mode = value ? 0ul : 1ul;
    return ::ioctlsocket(handle, FIONBIO, &mode);
#endif
}

int SocketAPI::GetBlockingMode(SocketHandle handle, bool & value)
{
#if defined(PLATFORM_LINUX)
    int flags = ::fcntl(handle, F_GETFL);
    if (flags == -1)
    {
        return flags;
    }
    value = ((flags & O_NONBLOCK) == 0);
    return value;
#elif defined(PLATFORM_WINDOWS)
    (void) handle;
    (void) value;
    return 0;
#endif
}

int SocketAPI::Bind(SocketHandle handle, const sockaddr * address, socklen_t addressLength)
{
    return ::bind(handle, address, addressLength);
}

int SocketAPI::Connect(SocketHandle handle, sockaddr const * serverAddress, socklen_t serverAddressLength)
{
    return ::connect(handle, serverAddress, serverAddressLength);
}

int SocketAPI::Listen(SocketHandle handle, int numListeners)
{
    return ::listen(handle, numListeners);
}

int SocketAPI::Accept(SocketHandle handle, sockaddr * clientAddress, socklen_t * clientAddressLength)
{
    SocketHandle result = ::accept(handle, clientAddress, clientAddressLength);
    return static_cast<int>(result);
}

int SocketAPI::GetLocalAddress(SocketHandle handle, sockaddr * address, socklen_t * addressLength)
{
    return ::getsockname(handle, address, addressLength);
}

int SocketAPI::GetRemoteAddress(SocketHandle handle, sockaddr * address, socklen_t * addressLength)
{
    return ::getpeername(handle, address, addressLength);
}

int SocketAPI::Receive(SocketHandle handle, std::uint8_t * data, std::size_t bufferSize, int flags)
{
    return ::recv(handle, reinterpret_cast<char *>(data), static_cast<int>(bufferSize), flags);
}

int SocketAPI::Send(SocketHandle handle, const std::uint8_t * data, std::size_t bytesToSend, int flags)
{
    return ::send(handle, reinterpret_cast<const char *>(data), static_cast<int>(bytesToSend), flags);
}

int SocketAPI::ReceiveFrom(SocketHandle handle, std::uint8_t * data, std::size_t bufferSize, int flags, sockaddr * address, socklen_t * addressLength)
{
    return ::recvfrom(handle, reinterpret_cast<char *>(data), static_cast<int>(bufferSize), flags, address, addressLength);
}

int SocketAPI::SendTo(SocketHandle handle, const std::uint8_t *data, std::size_t bytesToSend, int flags, const sockaddr * address, socklen_t addressLength)
{
    return ::sendto(handle, reinterpret_cast<const char *>(data), static_cast<int>(bytesToSend), flags, address, addressLength);
}

#if defined(PLATFORM_WINDOWS)

class SocketInitializer
{
public:
    SocketInitializer()
        : m_data()
        , m_initialized(false)
    {
        int errorCode {};
        // Do not trace here, as tracing may not be initialized yet
        errorCode = WSAStartup(MAKEWORD(2, 2), &m_data);
        if (errorCode != 0)
            tracing::Tracing::Error(__FILE__, __LINE__, __func__, utility::Error(errorCode, GetErrorString(errorCode), "socket() failed")); 
        else
            m_initialized = true;
    }
    ~SocketInitializer()
    {
        int errorCode {};
        if (m_initialized)
        {
            errorCode = WSACleanup();
            if (errorCode != 0)
                tracing::Tracing::Error(__FILE__, __LINE__, __func__, utility::Error(errorCode, GetErrorString(errorCode), "socket() failed")); 
        }
    }
private:
    WSADATA m_data;
    bool m_initialized;
};

static SocketInitializer winsockInitalizer;

#endif

} // namespace serialization
