#include "network-osal/Socket.h"

#include <algorithm>
#include <thread>
#include "tracing/ScopedTracing.h"
#include "tracing/Logging.h"
#include "utility/Error.h"
#include "network-osal/Network.h"
#include "network-osal/SocketAPI.h"

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
            tracing::Logging::Error(__FILE__, __LINE__, __func__, utility::Error(errorCode, GetErrorString(errorCode), "socket() failed")); 
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
                tracing::Logging::Error(__FILE__, __LINE__, __func__, utility::Error(errorCode, GetErrorString(errorCode), "socket() failed")); 
        }
    }
private:
    WSADATA m_data;
    bool m_initialized;
};

static SocketInitializer winsockInitalizer;

#endif

Socket::Socket(ISocketAPI & socketAPI)
    : m_socketAPI(socketAPI)
    , m_handle(InvalidHandleValue)
    , m_socketFamily(SocketFamily::Any)
    , m_socketType(SocketType::None)
#if defined(PLATFORM_WINDOWS)
    , m_isBlocking(true)
#endif
{
    SCOPEDTRACE(nullptr, [&]{
        return serialization::Serialize(static_cast<int>(GetHandle()), 0);
    });
}

Socket::Socket(ISocketAPI & socketAPI, SocketFamily socketFamily, SocketType socketType)
    : m_socketAPI(socketAPI)
    , m_handle(InvalidHandleValue)
    , m_socketFamily(socketFamily)
    , m_socketType(socketType)
#if defined(PLATFORM_WINDOWS)
    , m_isBlocking(true)
#endif
{
    SCOPEDTRACE([&]{
        return utility::FormatString(std::string("socketFamily={}, socketType={}"), socketFamily, socketType);
    }, [&]{
        return serialization::Serialize(static_cast<int>(GetHandle()), 0);
    });
    Open(socketFamily, socketType);
}

Socket::Socket(const Socket & other)
    : m_socketAPI(other.m_socketAPI)
    , m_handle(other.m_handle)
    , m_socketFamily(other.m_socketFamily)
    , m_socketType(other.m_socketType)
#if defined(PLATFORM_WINDOWS)
    , m_isBlocking(true)
#endif
{
    SCOPEDTRACE([&]{
        return utility::FormatString(std::string("socketFamily={}, socketType={}"), other.m_socketFamily, other.m_socketType);
    }, [&]{
        return serialization::Serialize(static_cast<int>(GetHandle()), 0);
    });
}

Socket::Socket(Socket && other)
    : m_socketAPI(other.m_socketAPI)
    , m_handle(other.m_handle)
    , m_socketFamily(other.m_socketFamily)
    , m_socketType(other.m_socketType)
#if defined(PLATFORM_WINDOWS)
    , m_isBlocking(other.m_isBlocking)
#endif
{
    SCOPEDTRACE([&]{
        return utility::FormatString(std::string("socketFamily={}, socketType={}"), other.m_socketFamily, other.m_socketType);
    }, [&]{
        return serialization::Serialize(static_cast<int>(GetHandle()), 0);
    });
    other.m_handle = InvalidHandleValue;
    other.m_socketFamily = {};
    other.m_socketType = {};
}

Socket::~Socket()
{
    SCOPEDTRACE(nullptr, nullptr);
    Close();
}

Socket &
Socket::operator = (Socket && other)
{
    SCOPEDTRACE([&]{
        return utility::FormatString(std::string("socketFamily={}, socketType={}"), other.m_socketFamily, other.m_socketType);
    }, [&]{
        return serialization::Serialize(static_cast<int>(GetHandle()), 0);
    });
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
        other.m_socketFamily = {};
        other.m_socketType = {};
    }
    return *this;
}

SocketHandle
Socket::GetHandle() const
{
    // SCOPEDTRACE(nullptr, [&]{
    //     return serialization::Serialize(static_cast<int>(m_handle), 0);
    // });
    return m_handle;
}

void
Socket::SetHandle(SocketHandle handle)
{
    SCOPEDTRACE([&]{
        return serialization::Serialize(static_cast<int>(handle), 0);
    }, nullptr);
    m_handle = handle;
}

void
Socket::Open(SocketFamily socketFamily, SocketType socketType, SocketProtocol protocol)
{
    SCOPEDTRACE([&]{
        return utility::FormatString(std::string("socketFamily={}, socketType={}, protocol={}"), 
            serialization::Serialize(socketFamily, 0), 
            serialization::Serialize(socketType, 0),
            serialization::Serialize(protocol, 0));
    }, [&]{
        return serialization::Serialize(static_cast<int>(GetHandle()), 0);
    });
    Close();
    Lock lock(m_mutex);
    SocketHandle result = m_socketAPI.Open(socketFamily, socketType, protocol);
    if (result != InvalidHandleValue)
    {
        m_handle = result;
        m_socketFamily = socketFamily;
        m_socketType = socketType;
    }
    else
    {
        int errorCode = GetError();

        tracing::Logging::Fatal(__FILE__, __LINE__, __func__, utility::Error(errorCode, GetErrorString(errorCode), "socket() failed")); 
    }
}

void
Socket::Close()
{
    SCOPEDTRACE(nullptr, nullptr);
    Lock lock(m_mutex);
    int result = 0;
    if (m_handle != InvalidHandleValue)
    {
        m_socketAPI.Close(m_handle);
        m_handle = InvalidHandleValue;
        m_socketFamily = {};
        m_socketType = {};
    }
    if (result == -1)
    {
        int errorCode = GetError();

        tracing::Logging::Fatal(__FILE__, __LINE__, __func__, utility::Error(errorCode, GetErrorString(errorCode), "close() failed")); 
    }
}

bool
Socket::IsOpen()
{
    bool result {};
    SCOPEDTRACE(nullptr, [&]{
        return serialization::Serialize(result, 0);
    });
    result = (m_handle != InvalidHandleValue);
    return result;
}

void Socket::SetSocketOptionWithLevel(SocketOptionLevel level, SocketOption socketOption, const void * optionValue, socklen_t optionLength)
{
    int result {};
    SCOPEDTRACE([&]{
        return utility::FormatString(std::string("level={}, socketOption={}, optionValue={}, optionLength={}"), 
            serialization::Serialize(level, 0), 
            serialization::Serialize(socketOption, 0),
            serialization::Serialize(optionValue, 0),
            serialization::Serialize(optionLength, 0));
    }, [&]{
        return utility::FormatString(std::string("result={}"), 
            serialization::Serialize(result, 0));
    });
    result = m_socketAPI.SetSocketOption(m_handle, level, socketOption, optionValue, optionLength);
    if (result == -1)
    {
        int errorCode = GetError();

        tracing::Logging::Fatal(__FILE__, __LINE__, __func__, utility::Error(errorCode, GetErrorString(errorCode), "setsockopt() failed"));
    }
}

void Socket::GetSocketOptionWithLevel(SocketOptionLevel level, SocketOption socketOption, void * optionValue, socklen_t * optionLength)
{
    int result {};
    SCOPEDTRACE([&]{
        return utility::FormatString(std::string("level={}, socketOption={}, optionValue={}, optionLength={}"), 
            serialization::Serialize(level, 0), 
            serialization::Serialize(socketOption, 0),
            serialization::Serialize(optionValue, 0),
            serialization::Serialize(*optionLength, 0));
    }, [&]{
        return utility::FormatString(std::string("result={}, optionLength={}"), 
            serialization::Serialize(result, 0),
            serialization::Serialize(*optionLength, 0));
    });
    result = m_socketAPI.GetSocketOption(m_handle, level, socketOption, optionValue, optionLength);
    if (result == -1)
    {
        int errorCode = GetError();

        tracing::Logging::Fatal(__FILE__, __LINE__, __func__, utility::Error(errorCode, GetErrorString(errorCode), "getsockopt() failed"));
    }
}

void Socket::SetSocketOption(SocketOption socketOption, const void * optionValue, socklen_t optionLength)
{
    SCOPEDTRACE([&]{
        return utility::FormatString(std::string("socketOption={}, optionValue={}, optionLength={}"), 
            serialization::Serialize(socketOption, 0),
            serialization::Serialize(optionValue, 0),
            serialization::Serialize(optionLength, 0));
    }, nullptr);
    SetSocketOptionWithLevel(SocketOptionLevel::Socket, socketOption, optionValue, optionLength);
}

void Socket::GetSocketOption(SocketOption socketOption, void * optionValue, socklen_t * optionLength)
{
    SCOPEDTRACE([&]{
        return utility::FormatString(std::string("socketOption={}, optionValue={}, optionLength={}"), 
            serialization::Serialize(socketOption, 0),
            serialization::Serialize(optionValue, 0),
            serialization::Serialize(*optionLength, 0));
    }, [&]{
        return utility::FormatString(std::string("optionLength= {}"), 
            serialization::Serialize(*optionLength, 0));
    });
    GetSocketOptionWithLevel(SocketOptionLevel::Socket, socketOption, optionValue, optionLength);
}

void Socket::SetSocketOptionBool(SocketOption socketOption, bool value)
{
    SCOPEDTRACE([&]{
        return utility::FormatString(std::string("socketOption={}, value={}"), 
            serialization::Serialize(socketOption, 0),
            serialization::Serialize(value, 0));
    }, nullptr);
    int optionValue = value ? 1 : 0;
    SetSocketOption(socketOption, &optionValue, sizeof(optionValue));
}

bool Socket::GetSocketOptionBool(SocketOption socketOption)
{
    bool result {};
    SCOPEDTRACE([&]{
        return utility::FormatString(std::string("socketOption={}"), 
            serialization::Serialize(socketOption, 0));
    }, [&]{
        return utility::FormatString(std::string("result={}"), 
            serialization::Serialize(result, 0));
    });
    int optionValue;
    socklen_t optionLength = sizeof(optionValue);
    GetSocketOption(socketOption, &optionValue, &optionLength);
    result = optionValue != 0;
    return result;
}

void Socket::SetSocketOptionInt(SocketOption socketOption, int optionValue)
{
    SCOPEDTRACE([&]{
        return utility::FormatString(std::string("socketOption={}, optionValue={}"), 
            serialization::Serialize(socketOption, 0),
            serialization::Serialize(optionValue, 0));
    }, nullptr);
    SetSocketOption(socketOption, &optionValue, sizeof(optionValue));
}

int Socket::GetSocketOptionInt(SocketOption socketOption)
{
    int optionValue {};
    SCOPEDTRACE([&]{
        return utility::FormatString(std::string("socketOption={}"), 
            serialization::Serialize(socketOption, 0));
    }, [&]{
        return utility::FormatString(std::string("result={}"), 
            serialization::Serialize(optionValue, 0));
    });
    socklen_t optionLength = sizeof(optionValue);
    GetSocketOption(socketOption, &optionValue, &optionLength);
    return optionValue;
}

void Socket::SetBroadcastOption(bool value)
{
    SCOPEDTRACE([&]{
        return utility::FormatString(std::string("value={}"), 
            serialization::Serialize(value, 0));
    }, nullptr);
    SetSocketOptionBool(SocketOption::Broadcast, value);
}

bool Socket::GetBroadcastOption()
{
    bool result {};
    SCOPEDTRACE(nullptr, [&]{
        return utility::FormatString(std::string("result={}"), 
            serialization::Serialize(result, 0));
    });
    result = GetSocketOptionBool(SocketOption::Broadcast);
    return result;
}

void Socket::SetBlockingMode(bool value)
{
    SCOPEDTRACE([&]{
        return utility::FormatString(std::string("value={}"), 
            serialization::Serialize(value, 0));
    }, nullptr);
    int result = m_socketAPI.SetBlockingMode(m_handle, value);
    if (result == -1)
    {
        int errorCode = GetError();

        tracing::Logging::Fatal(__FILE__, __LINE__, __func__, utility::Error(errorCode, GetErrorString(errorCode), "fcntl() failed"));
    }
#if defined(PLATFORM_WINDOWS)
    m_isBlocking = value;
#endif
}

bool Socket::GetBlockingMode()
{
    bool result {};
    SCOPEDTRACE(nullptr, [&]{
        return utility::FormatString(std::string("result={}"), 
            serialization::Serialize(result, 0));
    });
#if defined(PLATFORM_LINUX)
    int returnValue = m_socketAPI.GetBlockingMode(m_handle, result);
    if (returnValue == -1)
    {
        int errorCode = GetError();

        tracing::Logging::Fatal(__FILE__, __LINE__, __func__, utility::Error(errorCode, GetErrorString(errorCode), "fcntl() failed"));
    }
#elif defined(PLATFORM_WINDOWS)
    result = m_isBlocking;
#endif
    return result;
}

void Socket::SetReuseAddress(bool value)
{
    SCOPEDTRACE([&]{
        return utility::FormatString(std::string("value={}"), 
            serialization::Serialize(value, 0));
    }, nullptr);
    SetSocketOptionBool(SocketOption::ReuseAddress, value);
}

bool Socket::GetReuseAddress()
{
    bool result {};
    SCOPEDTRACE(nullptr, [&]{
        return utility::FormatString(std::string("result={}"), 
            serialization::Serialize(result, 0));
    });
    result = GetSocketOptionBool(SocketOption::ReuseAddress);
    return result;
}

void Socket::SetReceiveTimeout(std::chrono::milliseconds timeout)
{
    SCOPEDTRACE([&]{
        return utility::FormatString(std::string("timeout={} ms"), 
            serialization::Serialize(timeout.count(), 0));
    }, nullptr);
    timeval tv;
    std::memset(&tv, 0, sizeof(tv));
    tv.tv_sec = static_cast<long>(timeout.count() / 1000);
    tv.tv_usec = (timeout.count() % 1000) * 1000;
    SetSocketOption(SocketOption::ReceiveTimeout, &tv, sizeof(tv));
}

std::chrono::milliseconds Socket::GetReceiveTimeout()
{
    std::chrono::milliseconds result;
    SCOPEDTRACE(nullptr, [&]{
        return utility::FormatString(std::string("result={} ms"), 
            serialization::Serialize(result.count(), 0));
    });
    timeval tv;
    std::memset(&tv, 0, sizeof(tv));
    socklen_t optionLength = sizeof(tv);
    GetSocketOption(SocketOption::ReceiveTimeout, &tv, &optionLength);
    result = std::chrono::milliseconds(tv.tv_sec * 1000 + tv.tv_usec / 1000);
    return result;
}

void Socket::SetSendTimeout(std::chrono::milliseconds timeout)
{
    SCOPEDTRACE([&]{
        return utility::FormatString(std::string("timeout={} ms"), 
            serialization::Serialize(timeout.count(), 0));
    }, nullptr);
    timeval tv;
    std::memset(&tv, 0, sizeof(tv));
    tv.tv_sec = static_cast<long>(timeout.count() / 1000);
    tv.tv_usec = (timeout.count() % 1000) * 1000;
    SetSocketOption(SocketOption::SendTimeout, &tv, sizeof(tv));
}

std::chrono::milliseconds Socket::GetSendTimeout()
{
    std::chrono::milliseconds result;
    SCOPEDTRACE(nullptr, [&]{
        return utility::FormatString(std::string("result={} ms"), 
            serialization::Serialize(result.count(), 0));
    });
    timeval tv;
    std::memset(&tv, 0, sizeof(tv));
    socklen_t optionLength = sizeof(tv);
    GetSocketOption(SocketOption::SendTimeout, &tv, &optionLength);
    result = std::chrono::milliseconds(tv.tv_sec * 1000 + tv.tv_usec / 1000);
    return result;
}

void Socket::Bind(const sockaddr * address, socklen_t addressLength)
{
    int result {};
    SCOPEDTRACE([&]{
        return utility::FormatString(std::string("address={}, addressLength={}"), 
            serialization::Serialize(address, 0),
            serialization::Serialize(addressLength, 0));
    }, [&]{
        return utility::FormatString(std::string("result={}"), 
            serialization::Serialize(result, 0));
    });
    result = m_socketAPI.Bind(m_handle, address, addressLength);
    if (result == -1)
    {
        int errorCode = GetError();

        tracing::Logging::Fatal(__FILE__, __LINE__, __func__, utility::Error(errorCode, GetErrorString(errorCode), "bind() failed"));
    }
}

bool Socket::Connect(sockaddr const * serverAddress, socklen_t serverAddressLength, SocketTimeout timeout)
{
    int result {};
    SCOPEDTRACE([&]{
        return utility::FormatString(std::string("serverAddress={}, serverAddressLength={}, timeout={} ms"), 
            serialization::Serialize(serverAddress, 0),
            serialization::Serialize(serverAddressLength, 0),
            serialization::Serialize(timeout, 0));
    }, [&]{
        return utility::FormatString(std::string("result={}"), 
            serialization::Serialize(result, 0));
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
    result = m_socketAPI.Connect(m_handle, serverAddress, serverAddressLength);
    if (result == -1)
    {
        int errorCode = GetError();

        // tracing::Logging::Error(__FILE__, __LINE__, __func__, utility::Error(errorCode, GetErrorString(errorCode), "connect() failed"));

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

                // tracing::Logging::Error(__FILE__, __LINE__, __func__, utility::Error(errorCode, GetErrorString(errorCode), "poll() failed"));
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
            tracing::Logging::Fatal(__FILE__, __LINE__, __func__, utility::Error(errorCode, GetErrorString(errorCode), "connect() failed"));
    }
#elif defined(PLATFORM_WINDOWS)
    SocketTimeout waitTime = 0;
    if (timeout != InfiniteTimeout)
    {
        waitTime = timeout;
    }

    result = m_socketAPI.Connect(m_handle, serverAddress, serverAddressLength);
    if (result == -1)
    {
        int errorCode = GetError();

        // tracing::Logging::Error(__FILE__, __LINE__, __func__, utility::Error(errorCode, GetErrorString(errorCode), "connect() failed"));

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

                tracing::Logging::Fatal(__FILE__, __LINE__, __func__, utility::Error(errorCode, GetErrorString(errorCode), "select() failed"));
            }
            else if (result == 0)
            {
                // tracing::Tracing::Trace(tracing::TraceCategory::Information, __FILE__, __LINE__, __func__, utility::Error(errorCode, GetErrorString(errorCode), "connect() timeout"));
            }
            if (FD_ISSET(GetHandle(), &exceptfds))
            {
                errorCode = GetError();
                if (errorCode != 0)
                    tracing::Logging::Fatal(__FILE__, __LINE__, __func__, utility::Error(errorCode, GetErrorString(errorCode), "connect() failed"));
            }
            if (FD_ISSET(GetHandle(), &writefds))
            {
                TraceMessage(__FILE__, __LINE__, __func__, "connect() success");
                errorCode = 0;
                break;
            }
        }
        if (errorCode != 0)
            tracing::Logging::Fatal(__FILE__, __LINE__, __func__, utility::Error(errorCode, GetErrorString(errorCode), "connect() failed"));
    }
#endif

    SetBlockingMode(true);
    return (result != -1);
}

void Socket::Listen(int numListeners)
{
    int result {};
    SCOPEDTRACE([&]{
        return utility::FormatString(std::string("numListeners={}"), 
            serialization::Serialize(numListeners, 0));
    }, [&]{
        return utility::FormatString(std::string("result={}"), 
            serialization::Serialize(result, 0));
    });
    result = m_socketAPI.Listen(m_handle, numListeners);
    if (result == -1)
    {
        int errorCode = GetError();

        tracing::Logging::Fatal(__FILE__, __LINE__, __func__, utility::Error(errorCode, GetErrorString(errorCode), "listen() failed"));
    }
}

bool Socket::Accept(Socket & connectionSocket, sockaddr * clientAddress, socklen_t * clientAddressLength, SocketTimeout timeout)
{
    bool result {};
    SocketHandle handle {};
    SCOPEDTRACE([&]{
        return utility::FormatString(std::string("clientAddress={}, clientAddressLength={}, timeout={} ms"), 
            serialization::Serialize(clientAddress, 0),
            serialization::Serialize(*clientAddressLength, 0),
            serialization::Serialize(timeout, 0));
    }, [&]{
        return utility::FormatString(std::string("result={}, socket={}"), 
            serialization::Serialize(result, 0),
            serialization::Serialize(handle, 0));
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
        handle = static_cast<SocketHandle>(m_socketAPI.Accept(m_handle, clientAddress, clientAddressLength));
        if (handle == -1)
        {
            int errorCode = GetError();

            // tracing::Logging::Error(__FILE__, __LINE__, __func__, utility::Error(errorCode, GetErrorString(errorCode), "accept() failed"));

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
                tracing::Logging::Fatal(__FILE__, __LINE__, __func__, utility::Error(errorCode, GetErrorString(errorCode), "accept() failed"));
        }
    }
    while ((handle == -1) && (waitTime > 0));

    SetBlockingMode(true);
    if (handle != -1)
    {
        TraceMessage(__FILE__, __LINE__, __func__, "accept() success");
        connectionSocket.SetHandle(static_cast<SocketHandle>(handle));
    }
    else
    {
        connectionSocket.Close();
    }
    result = (handle != -1);
    return result;
}

void Socket::GetLocalAddress(sockaddr * address, socklen_t * addressLength)
{
    int result {};
    SCOPEDTRACE([&]{
        return utility::FormatString(std::string("address={}, addressLength={}"), 
            serialization::Serialize(address, 0),
            serialization::Serialize(*addressLength, 0));
    }, [&]{
        return utility::FormatString(std::string("result={}, addressLength={}"), 
            serialization::Serialize(result != -1, 0),
            serialization::Serialize(*addressLength, 0));
    });
    result = m_socketAPI.GetLocalAddress(m_handle, address, addressLength);
    if (result == -1)
    {
        int errorCode = GetError();

        tracing::Logging::Fatal(__FILE__, __LINE__, __func__, utility::Error(errorCode, GetErrorString(errorCode), "getsockname() failed"));
    }
}

void Socket::GetRemoteAddress(sockaddr * address, socklen_t * addressLength)
{
    int result {};
    SCOPEDTRACE([&]{
        return utility::FormatString(std::string("address={}, addressLength={}"), 
            serialization::Serialize(address, 0),
            serialization::Serialize(*addressLength, 0));
    }, [&]{
        return utility::FormatString(std::string("result={}, addressLength={}"), 
            serialization::Serialize(result != -1, 0),
            serialization::Serialize(*addressLength, 0));
    });
    result = m_socketAPI.GetRemoteAddress(m_handle, address, addressLength);
    if (result == -1)
    {
        int errorCode = GetError();

        tracing::Logging::Fatal(__FILE__, __LINE__, __func__, utility::Error(errorCode, GetErrorString(errorCode), "getpeername() failed"));
    }
}

std::size_t Socket::Receive(std::uint8_t * data, std::size_t bufferSize, int flags)
{
    std::size_t numBytes {};
    SCOPEDTRACE([&]{
        return utility::FormatString(std::string("bufferSize={}, flags={}"), 
            serialization::Serialize(bufferSize, 0),
            serialization::Serialize(flags, 0));
    }, [&]{
        return utility::FormatString(std::string("result={}, data=[{}]"), 
            serialization::Serialize(numBytes, 0),
            serialization::SerializeData(data, bufferSize));
    });
    try
    {
        int result = m_socketAPI.Receive(m_handle, data, bufferSize, flags);
        if (result == -1)
        {
            int errorCode = GetError();

            tracing::Logging::Error(__FILE__, __LINE__, __func__, utility::Error(errorCode, GetErrorString(errorCode), "recv() failed"));
            if ((errorCode != EINTR) && (errorCode != EWOULDBLOCK) && (errorCode != EAGAIN))
                tracing::Logging::Throw(__FILE__, __LINE__, __func__, utility::Error(errorCode, GetErrorString(errorCode), "recv() failed"));
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
    SCOPEDTRACE([&]{
        return utility::FormatString(std::string("data=[{}], flags={}"), 
            serialization::SerializeData(data, bytesToSend),
            serialization::Serialize(flags, 0));
    }, [&]{
        return utility::FormatString(std::string("result={}"), 
            serialization::Serialize(result, 0));
    });
    int numBytesLeftToSend = static_cast<int>(bytesToSend);
    std::size_t offset = 0;

    while (numBytesLeftToSend > 0)
    {
        int numBytes = m_socketAPI.Send(m_handle, data + offset, static_cast<std::size_t>(numBytesLeftToSend), flags);
        if (numBytes == -1)
        {
            int errorCode = GetError();

            tracing::Logging::Error(__FILE__, __LINE__, __func__, utility::Error(errorCode, GetErrorString(errorCode), "send() failed"));

            if ((errorCode == EPIPE) || (errorCode == ECONNRESET))
            {
                result = false;
                break;
            }
            
            tracing::Logging::Throw(__FILE__, __LINE__, __func__, utility::Error(errorCode, GetErrorString(errorCode), "send() failed"));
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
    SCOPEDTRACE([&]{
        return utility::FormatString(std::string("address=[{}], bufferSize={}, flags={}"), 
            serialization::Serialize(address, addressLength),
            serialization::Serialize(bufferSize, 0));
    }, [&]{
        return utility::FormatString(std::string("result={}, address=[{}], data=[{}]"), 
            serialization::Serialize(numBytes, 0),
            serialization::Serialize(address, addressLength),
            serialization::SerializeData(data, bufferSize));
    });
    int result = m_socketAPI.ReceiveFrom(m_handle, data, bufferSize, 0, address, addressLength);
    if (result == -1)
    {
        int errorCode = GetError();

        tracing::Logging::Error(__FILE__, __LINE__, __func__, utility::Error(errorCode, GetErrorString(errorCode), "recvfrom() failed"));

        if ((errorCode != EINTR) && (errorCode != EWOULDBLOCK) && (errorCode != EAGAIN))
            tracing::Logging::Throw(__FILE__, __LINE__, __func__, utility::Error(errorCode, GetErrorString(errorCode), "recv() failed"));
        result = 0;
    }

    numBytes = static_cast<std::size_t>(result);
    return numBytes;
}

void Socket::SendTo(const sockaddr * address, socklen_t addressLength, const std::uint8_t *data, std::size_t bytesToSend)
{
    int result {};
    SCOPEDTRACE([&]{
        return utility::FormatString(std::string("address=[{}], data=[{}]"), 
            serialization::Serialize(address, addressLength),
            serialization::SerializeData(data, bytesToSend));
    }, [&]{
        return utility::FormatString(std::string("result={}"), 
            serialization::Serialize(result, 0));
    });
    result = m_socketAPI.SendTo(m_handle, data, bytesToSend, 0, address, addressLength);
    if (result == -1)
    {
        int errorCode = GetError();

        tracing::Logging::Throw(__FILE__, __LINE__, __func__, utility::Error(errorCode, GetErrorString(errorCode), "sendto() failed"));
    }
}

} // namespace network

namespace serialization {

std::string Serialize(const network::Socket & value, int width)
{
    return Serialize(static_cast<int>(value.GetHandle()), width);
}

std::string Serialize(const sockaddr * value, socklen_t size)
{
    if (value == nullptr)
        return "null";
    return utility::FormatString(std::string("addressFamily={}, size={}"), value->sa_family, size);
}

std::string Serialize(sockaddr * value, socklen_t size)
{
    return Serialize(const_cast<const sockaddr *>(value), size);
}

std::string Serialize(const sockaddr * value, socklen_t * size)
{
    if (value == nullptr)
        return "null";
    if (size == nullptr)
        return "????";
    return utility::FormatString(std::string("addressFamily={}, size={}"), value->sa_family, *size);
}

} // namespace serialization
