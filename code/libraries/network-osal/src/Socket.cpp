#include "network-osal/Socket.h"

#include <algorithm>
#include <thread>
#include "tracing/ScopedTracing.h"
#include "tracing/Logging.h"
#include "utility/Error.h"
#include "utility/GenericError.h"
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

const size_t BufferSize = 4096;

static const SocketTimeout TimeWait = 10;

Socket::Socket(ISocketAPI & socketAPI)
    : m_socketAPI(socketAPI)
    , m_handle(InvalidHandleValue)
    , m_socketFamily(SocketFamily::Any)
    , m_socketType(SocketType::None)
    , m_socketProtocol(SocketProtocol::IP)
#if defined(PLATFORM_WINDOWS)
    , m_isBlocking(true)
#endif
{
    SCOPEDTRACE(nullptr, nullptr);
}

Socket::Socket(ISocketAPI & socketAPI, SocketFamily socketFamily, SocketType socketType, SocketProtocol socketProtocol)
    : m_socketAPI(socketAPI)
    , m_handle(InvalidHandleValue)
    , m_socketFamily(socketFamily)
    , m_socketType(socketType)
    , m_socketProtocol(socketProtocol)
#if defined(PLATFORM_WINDOWS)
    , m_isBlocking(true)
#endif
{
    SCOPEDTRACE([&]{
        return utility::FormatString("socketFamily={}, socketType={}, socketProtocol={}", socketFamily, socketType, socketProtocol);
    }, nullptr);
}

Socket::Socket(Socket && other)
    : m_socketAPI(other.m_socketAPI)
    , m_handle(other.m_handle)
    , m_socketFamily(other.m_socketFamily)
    , m_socketType(other.m_socketType)
    , m_socketProtocol(other.m_socketProtocol)
#if defined(PLATFORM_WINDOWS)
    , m_isBlocking(other.m_isBlocking)
#endif
{
    SCOPEDTRACE([&]{
        return utility::FormatString("socketFamily={}, socketType={}, socketProtocol={}", other.m_socketFamily, other.m_socketType, other.m_socketProtocol);
    }, [this]{
        return serialization::Serialize(static_cast<int>(m_handle), 0);
    });
    other.m_handle = InvalidHandleValue;
}

Socket::~Socket()
{
    SCOPEDTRACE(nullptr, nullptr);
    if (IsOpen())
        Close();
}

Socket &
Socket::operator = (Socket && other)
{
    SCOPEDTRACE([&]{
        return utility::FormatString("socketFamily={}, socketType={}, socketProtocol={}", other.m_socketFamily, other.m_socketType, other.m_socketProtocol);
    }, [this]{
        return serialization::Serialize(static_cast<int>(m_handle), 0);
    });
    if (this != &other)
    {
        if (IsOpen())
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
    SCOPEDTRACE(nullptr, [&]{
        return serialization::Serialize(static_cast<int>(m_handle), 0);
    });
    return m_handle;
}

void
Socket::SetHandle(SocketHandle handle)
{
    SCOPEDTRACE([&]{ return serialization::Serialize(static_cast<int>(handle), 0); }, 
                nullptr);
    m_handle = handle;
}

void
Socket::Open()
{
    SCOPEDTRACE([this]{
        return utility::FormatString("socketFamily={}, socketType={}, protocol={}", 
            m_socketFamily, m_socketType, m_socketProtocol);
    }, [this]{
        return serialization::Serialize(static_cast<int>(m_handle), 0);
    });
    Close();
    Lock lock(m_mutex);
    SocketHandle result = m_socketAPI.Open(m_socketFamily, m_socketType, m_socketProtocol);
    if (result != InvalidHandleValue)
    {
        m_handle = result;
    }
    else
    {
        int errorCode = GetError();

        tracing::Logging::Error(__FILE__, __LINE__, __func__, utility::Error(errorCode, GetErrorString(errorCode), "socket() failed")); 
    }
}

void
Socket::Close()
{
    SCOPEDTRACE([this]{
        return serialization::Serialize(static_cast<int>(m_handle), 0);
    }, nullptr);
    Lock lock(m_mutex);
    int result = 0;
    if (m_handle != InvalidHandleValue)
    {
        m_socketAPI.Close(m_handle);
        m_handle = InvalidHandleValue;
    }
    if (result == -1)
    {
        int errorCode = GetError();

        tracing::Logging::Error(__FILE__, __LINE__, __func__, utility::Error(errorCode, GetErrorString(errorCode), "close() failed")); 
    }
}

bool
Socket::IsOpen()
{
    bool result {};
    SCOPEDTRACE([this]{
        return utility::FormatString("handle={}",
            serialization::Serialize(static_cast<int>(m_handle), 0));
    }, [&]{
        return utility::FormatString("result={}", 
            serialization::Serialize(result, 0));
    });
    result = (m_handle != InvalidHandleValue);
    return result;
}

void Socket::SetSocketOptionWithLevel(SocketOptionLevel level, SocketOption socketOption, const void * optionValue, socklen_t optionLength)
{
    int result {};
    SCOPEDTRACE([&]{
        return utility::FormatString("handle={}, level={}, socketOption={}, optionValue={}, optionLength={}", 
            serialization::Serialize(static_cast<int>(m_handle), 0),
            serialization::Serialize(level, 0), 
            serialization::Serialize(socketOption, 0),
            serialization::Serialize(optionValue, 0),
            serialization::Serialize(optionLength, 0));
    }, [&]{
        return utility::FormatString("result={}", 
            serialization::Serialize(result, 0));
    });
    Lock lock(m_mutex);
    result = m_socketAPI.SetSocketOption(m_handle, level, socketOption, optionValue, optionLength);
    if (result == -1)
    {
        int errorCode = GetError();

        tracing::Logging::Error(__FILE__, __LINE__, __func__, utility::Error(errorCode, GetErrorString(errorCode), "setsockopt() failed"));
    }
}

void Socket::GetSocketOptionWithLevel(SocketOptionLevel level, SocketOption socketOption, void * optionValue, socklen_t * optionLength)
{
    int result {};
    SCOPEDTRACE([&]{
        return utility::FormatString("handle={}, level={}, socketOption={}, optionValue={}, optionLength={}", 
            serialization::Serialize(static_cast<int>(m_handle), 0),
            serialization::Serialize(level, 0), 
            serialization::Serialize(socketOption, 0),
            serialization::Serialize(optionValue, 0),
            serialization::Serialize(*optionLength, 0));
    }, [&]{
        return utility::FormatString("result={}, optionLength={}", 
            serialization::Serialize(result, 0),
            serialization::Serialize(*optionLength, 0));
    });
    Lock lock(m_mutex);
    result = m_socketAPI.GetSocketOption(m_handle, level, socketOption, optionValue, optionLength);
    if (result == -1)
    {
        int errorCode = GetError();

        tracing::Logging::Error(__FILE__, __LINE__, __func__, utility::Error(errorCode, GetErrorString(errorCode), "getsockopt() failed"));
    }
}

void Socket::SetSocketOption(SocketOption socketOption, const void * optionValue, socklen_t optionLength)
{
    SCOPEDTRACE([&]{
        return utility::FormatString("handle={}, socketOption={}, optionValue={}, optionLength={}", 
            serialization::Serialize(static_cast<int>(m_handle), 0),
            serialization::Serialize(socketOption, 0),
            serialization::Serialize(optionValue, 0),
            serialization::Serialize(optionLength, 0));
    }, nullptr);
    SetSocketOptionWithLevel(SocketOptionLevel::Socket, socketOption, optionValue, optionLength);
}

void Socket::GetSocketOption(SocketOption socketOption, void * optionValue, socklen_t * optionLength)
{
    SCOPEDTRACE([&]{
        return utility::FormatString("handle={}, socketOption={}, optionValue={}, optionLength={}", 
            serialization::Serialize(static_cast<int>(m_handle), 0),
            serialization::Serialize(socketOption, 0),
            serialization::Serialize(optionValue, 0),
            serialization::Serialize(*optionLength, 0));
    }, [&]{
        return utility::FormatString("optionLength= {}", 
            serialization::Serialize(*optionLength, 0));
    });
    GetSocketOptionWithLevel(SocketOptionLevel::Socket, socketOption, optionValue, optionLength);
}

void Socket::SetSocketOptionBool(SocketOption socketOption, bool value)
{
    SCOPEDTRACE([&]{
        return utility::FormatString("handle={}, socketOption={}, value={}", 
            serialization::Serialize(static_cast<int>(m_handle), 0),
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
        return utility::FormatString("handle={}, socketOption={}", 
            serialization::Serialize(static_cast<int>(m_handle), 0),
            serialization::Serialize(socketOption, 0));
    }, [&]{
        return utility::FormatString("result={}", 
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
        return utility::FormatString("handle={}, socketOption={}, optionValue={}", 
            serialization::Serialize(static_cast<int>(m_handle), 0),
            serialization::Serialize(socketOption, 0),
            serialization::Serialize(optionValue, 0));
    }, nullptr);
    SetSocketOption(socketOption, &optionValue, sizeof(optionValue));
}

int Socket::GetSocketOptionInt(SocketOption socketOption)
{
    int optionValue {};
    SCOPEDTRACE([&]{
        return utility::FormatString("handle={}, socketOption={}", 
            serialization::Serialize(static_cast<int>(m_handle), 0),
            serialization::Serialize(socketOption, 0));
    }, [&]{
        return utility::FormatString("result={}", 
            serialization::Serialize(optionValue, 0));
    });
    socklen_t optionLength = sizeof(optionValue);
    GetSocketOption(socketOption, &optionValue, &optionLength);
    return optionValue;
}

void Socket::SetBroadcastOption(bool value)
{
    SCOPEDTRACE([&]{
        return utility::FormatString("handle={}, value={}", 
            serialization::Serialize(static_cast<int>(m_handle), 0),
            serialization::Serialize(value, 0));
    }, nullptr);
    SetSocketOptionBool(SocketOption::Broadcast, value);
}

bool Socket::GetBroadcastOption()
{
    bool result {};
    SCOPEDTRACE(nullptr, [&]{
        return utility::FormatString("handle={}, result={}", 
            serialization::Serialize(static_cast<int>(m_handle), 0),
            serialization::Serialize(result, 0));
    });
    result = GetSocketOptionBool(SocketOption::Broadcast);
    return result;
}

void Socket::SetBlockingMode(bool value)
{
    SCOPEDTRACE([&]{
        return utility::FormatString("handle={}, value={}", 
            serialization::Serialize(static_cast<int>(m_handle), 0),
            serialization::Serialize(value, 0));
    }, nullptr);
    Lock lock(m_mutex);
    int result = m_socketAPI.SetBlockingMode(m_handle, value);
    if (result == -1)
    {
        int errorCode = GetError();

        tracing::Logging::Error(__FILE__, __LINE__, __func__, utility::Error(errorCode, GetErrorString(errorCode), "fcntl() failed"));
    }
#if defined(PLATFORM_WINDOWS)
    m_isBlocking = value;
#endif
}

bool Socket::GetBlockingMode()
{
    bool result {};
    SCOPEDTRACE(nullptr, [&]{
        return utility::FormatString("handle={}, result={}", 
            serialization::Serialize(static_cast<int>(m_handle), 0),
            serialization::Serialize(result, 0));
    });
    Lock lock(m_mutex);
#if defined(PLATFORM_LINUX)
    int returnValue = m_socketAPI.GetBlockingMode(m_handle, result);
    if (returnValue == -1)
    {
        int errorCode = GetError();

        tracing::Logging::Error(__FILE__, __LINE__, __func__, utility::Error(errorCode, GetErrorString(errorCode), "fcntl() failed"));
    }
#elif defined(PLATFORM_WINDOWS)
    result = m_isBlocking;
#endif
    return result;
}

void Socket::SetReuseAddress(bool value)
{
    SCOPEDTRACE([&]{
        return utility::FormatString("handle={}, value={}", 
            serialization::Serialize(static_cast<int>(m_handle), 0),
            serialization::Serialize(value, 0));
    }, nullptr);
    SetSocketOptionBool(SocketOption::ReuseAddress, value);
}

bool Socket::GetReuseAddress()
{
    bool result {};
    SCOPEDTRACE(nullptr, [&]{
        return utility::FormatString("handle={}, result={}", 
            serialization::Serialize(static_cast<int>(m_handle), 0),
            serialization::Serialize(result, 0));
    });
    result = GetSocketOptionBool(SocketOption::ReuseAddress);
    return result;
}

void Socket::SetReceiveTimeout(std::chrono::milliseconds timeout)
{
    SCOPEDTRACE([&]{
        return utility::FormatString("handle={}, timeout={} ms", 
            serialization::Serialize(static_cast<int>(m_handle), 0),
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
        return utility::FormatString("handle={}, result={} ms", 
            serialization::Serialize(static_cast<int>(m_handle), 0),
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
        return utility::FormatString("handle={}, timeout={} ms", 
            serialization::Serialize(static_cast<int>(m_handle), 0),
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
        return utility::FormatString("handle={}, result={} ms", 
            serialization::Serialize(static_cast<int>(m_handle), 0),
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
        return utility::FormatString("handle={}, address=[{}], addressLength={}", 
            serialization::Serialize(static_cast<int>(m_handle), 0),
            serialization::Serialize(address, addressLength),
            serialization::Serialize(addressLength, 0));
    }, [&]{
        return utility::FormatString("result={}", 
            serialization::Serialize(result, 0));
    });
    Lock lock(m_mutex);
    result = m_socketAPI.Bind(m_handle, address, addressLength);
    if (result == -1)
    {
        int errorCode = GetError();

        tracing::Logging::Error(__FILE__, __LINE__, __func__, utility::Error(errorCode, GetErrorString(errorCode), "bind() failed"));
    }
}

bool Socket::Connect(sockaddr const * serverAddress, socklen_t serverAddressLength, SocketTimeout timeout)
{
    int result {};
    SCOPEDTRACE([&]{
        return utility::FormatString("handle={}, serverAddress=[{}], serverAddressLength={}, timeout={} ms", 
            serialization::Serialize(static_cast<int>(m_handle), 0),
            serialization::Serialize(serverAddress, serverAddressLength),
            serialization::Serialize(serverAddressLength, 0),
            serialization::Serialize(timeout, 0));
    }, [&]{
        return utility::FormatString("result={}", 
            serialization::Serialize(result != -1, 0));
    });

    Lock lock(m_mutex);
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
            descriptor.fd = m_handle;
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
            tracing::Logging::Error(__FILE__, __LINE__, __func__, utility::Error(errorCode, GetErrorString(errorCode), "connect() failed"));
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

        while ((waitTime > 0) && ((errorCode == EWOULDBLOCK) || (errorCode == EINPROGRESS)))
        {
            timeval waitInterval { 0, 1000 * TimeWait };
            fd_set readfds;
            fd_set writefds {1, {m_handle}};
            fd_set exceptfds {1, {m_handle}};
            FD_ZERO(&readfds);
            FD_ZERO(&writefds);
            FD_ZERO(&exceptfds);
            FD_SET(m_handle, &writefds);
            FD_SET(m_handle, &exceptfds);
            // tracing::Tracing::Trace(tracing::TraceCategory::Data, __FILE__, __LINE__, __func__, "connect() wait, {} ms left", waitTime);
            int selectResult = select(1, &readfds, &writefds, &exceptfds, &waitInterval);
            TraceMessage(__FILE__, __LINE__, __func__, "select() result={}", selectResult);
            waitTime -= std::min(waitTime, TimeWait);
            if (selectResult == -1)
            {
                errorCode = GetError();

                tracing::Logging::Error(__FILE__, __LINE__, __func__, utility::Error(errorCode, GetErrorString(errorCode), "select() failed"));
                break;
            }
            else if (selectResult == 0)
            {
                // errorCode = 0;
                // tracing::Logging::Error(__FILE__, __LINE__, __func__, utility::Error(errorCode, GetErrorString(errorCode), "connect() timeout"));
                // break;
            }
            if (FD_ISSET(m_handle, &exceptfds))
            {
                TraceMessage(__FILE__, __LINE__, __func__, "exceptfds");
            }
            if (FD_ISSET(m_handle, &writefds))
            {
                TraceMessage(__FILE__, __LINE__, __func__, "writefds");
                TraceMessage(__FILE__, __LINE__, __func__, "connect() success");
                result = 0;
                errorCode = 0;
                break;
            }
        }
        if (errorCode != 0)
            tracing::Logging::Error(__FILE__, __LINE__, __func__, utility::Error(errorCode, GetErrorString(errorCode), "connect() failed"));
    }
#endif

    SetBlockingMode(true);
    return (result != -1);
}

void Socket::Listen(int numListeners)
{
    int result {};
    SCOPEDTRACE([&]{
        return utility::FormatString("numListeners={}", 
            serialization::Serialize(numListeners, 0));
    }, [&]{
        return utility::FormatString("result={}", 
            serialization::Serialize(result, 0));
    });
    Lock lock(m_mutex);
    result = m_socketAPI.Listen(m_handle, numListeners);
    if (result == -1)
    {
        int errorCode = GetError();

        tracing::Logging::Error(__FILE__, __LINE__, __func__, utility::Error(errorCode, GetErrorString(errorCode), "listen() failed"));
    }
}

bool Socket::Accept(Socket & connectionSocket, sockaddr * clientAddress, socklen_t * clientAddressLength, SocketTimeout timeout)
{
    bool result {};
    SocketHandle handle {};
    SCOPEDTRACE([&]{
        return utility::FormatString("handle={}, clientAddressLength={}, timeout={} ms", 
            serialization::Serialize(static_cast<int>(m_handle), 0),
            serialization::Serialize(*clientAddressLength, 0),
            serialization::Serialize(timeout, 0));
    }, [&]{
        return utility::FormatString("result={}, clientAddress=[{}], socket={}", 
            serialization::Serialize(result, 0),
            serialization::Serialize(clientAddress, clientAddressLength),
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
            {
                tracing::Logging::Error(__FILE__, __LINE__, __func__, utility::Error(errorCode, GetErrorString(errorCode), "accept() failed"));
                break;
            }
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
        return utility::FormatString("handle={}, addressLength={}", 
            serialization::Serialize(static_cast<int>(m_handle), 0),
            serialization::Serialize(*addressLength, 0));
    }, [&]{
        return utility::FormatString("result={}, address=[{}], addressLength={}", 
            serialization::Serialize(result != -1, 0),
            serialization::Serialize(address, addressLength),
            serialization::Serialize(*addressLength, 0));
    });
    Lock lock(m_mutex);
    result = m_socketAPI.GetLocalAddress(m_handle, address, addressLength);
    if (result == -1)
    {
        int errorCode = GetError();

        tracing::Logging::Error(__FILE__, __LINE__, __func__, utility::Error(errorCode, GetErrorString(errorCode), "getsockname() failed"));
    }
}

void Socket::GetRemoteAddress(sockaddr * address, socklen_t * addressLength)
{
    int result {};
    SCOPEDTRACE([&]{
        return utility::FormatString("handle={}, addressLength={}", 
            serialization::Serialize(static_cast<int>(m_handle), 0),
            serialization::Serialize(*addressLength, 0));
    }, [&]{
        return utility::FormatString("result={}, address=[{}], addressLength={}", 
            serialization::Serialize(result != -1, 0),
            serialization::Serialize(address, addressLength),
            serialization::Serialize(*addressLength, 0));
    });
    Lock lock(m_mutex);
    result = m_socketAPI.GetRemoteAddress(m_handle, address, addressLength);
    if (result == -1)
    {
        int errorCode = GetError();

        tracing::Logging::Error(__FILE__, __LINE__, __func__, utility::Error(errorCode, GetErrorString(errorCode), "getpeername() failed"));
    }
}

std::size_t Socket::Receive(std::uint8_t * data, std::size_t bufferSize, int flags)
{
    std::size_t numBytes {};
    SCOPEDTRACE([&]{
        return utility::FormatString("handle={}, bufferSize={}, flags={}", 
            serialization::Serialize(static_cast<int>(m_handle), 0),
            serialization::Serialize(bufferSize, 0),
            serialization::Serialize(flags, 0));
    }, [&]{
        return utility::FormatString("result={}, data=[{}]", 
            serialization::Serialize(numBytes, 0),
            serialization::SerializeData(data, numBytes));
    });
    Lock lock(m_mutex);
    TraceMessage(__FILE__, __LINE__, __func__, "Receive");
    try
    {
        int result = m_socketAPI.Receive(m_handle, data, bufferSize, flags);
        if (result == -1)
        {
            int errorCode = GetError();

            if ((errorCode != EINTR) && (errorCode != EWOULDBLOCK) && (errorCode != EAGAIN))
                tracing::Logging::Error(__FILE__, __LINE__, __func__, utility::Error(errorCode, GetErrorString(errorCode), "recv() failed"));
        } else if (result == 0)
        {
            // Read is blocking, if 0 bytes are returned the other side is closed
            tracing::Logging::Error(__FILE__, __LINE__, __func__, utility::GenericError("recv() returned nothing, closing socket"));
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
    TraceMessage(__FILE__, __LINE__, __func__, "Received {} bytes", numBytes);
    return numBytes;
}

bool Socket::Send(const std::uint8_t * data, std::size_t numBytesToSend, int flags)
{
    bool result {};
    SCOPEDTRACE([&]{
        return utility::FormatString("handle={}, data=[{}], flags={}", 
            serialization::Serialize(static_cast<int>(m_handle), 0),
            serialization::SerializeData(data, numBytesToSend),
            serialization::Serialize(flags, 0));
    }, [&]{
        return utility::FormatString("result={}", 
            serialization::Serialize(result, 0));
    });
    Lock lock(m_mutex);
    TraceMessage(__FILE__, __LINE__, __func__, "Send");
    int numBytes = m_socketAPI.Send(m_handle, data, numBytesToSend, flags);
    if (numBytes == -1)
    {
        int errorCode = GetError();

        tracing::Logging::Error(__FILE__, __LINE__, __func__, utility::Error(errorCode, GetErrorString(errorCode), "send() failed"));
    }
    TraceMessage(__FILE__, __LINE__, __func__, "Sent {} bytes", numBytes);
    result = (static_cast<std::size_t>(numBytes) == numBytesToSend);
    return result;
}

bool Socket::ReceiveBlock(ByteBuffer & data, std::size_t bufferSize, int flags)
{
    bool result {};
    std::size_t numBytesReceivedTotal {};
    std::size_t numBytesToReceiveTotal = bufferSize;
    std::uint8_t block[BufferSize];
    SCOPEDTRACE([&]{
        return utility::FormatString("handle={}, bufferSize={} flags={}", 
            serialization::Serialize(static_cast<int>(m_handle), 0),
            serialization::Serialize(bufferSize, 0),
            serialization::Serialize(flags, 0));
    }, [&]{
        return utility::FormatString("result={}, data=[{}]", 
            serialization::Serialize(result, 0),
            serialization::SerializeData(data));
    });

    TraceMessage(__FILE__, __LINE__, __func__, "Receive");
    try
    {
        std::size_t numBytesToReceive = std::min(numBytesToReceiveTotal, BufferSize);
        std::size_t numBytesReceived = Receive(block, numBytesToReceive, flags);
        while (numBytesToReceiveTotal > 0)
        {
            numBytesReceivedTotal += numBytesReceived;
            numBytesToReceiveTotal -= numBytesReceived;
            data.insert(data.end(), std::begin(block), std::begin(block) + numBytesReceived);
            numBytesToReceive = std::min(numBytesToReceiveTotal, BufferSize);
            numBytesReceived = Receive(block, numBytesToReceive, flags);
        }
    }
    catch (std::exception & e)
    {
        LogError(__FILE__, __LINE__, __func__, "Receive failed: {}", e.what());
    }
    TraceMessage(__FILE__, __LINE__, __func__, "Received {} bytes", numBytesReceivedTotal);
    result = (numBytesToReceiveTotal == 0);
    return result;
}

std::size_t Socket::ReceiveBuffer(ByteBuffer & data, std::size_t bufferSize, int flags)
{
    std::size_t numBytesReceivedTotal {};
    std::size_t numBytesToReceiveTotal = bufferSize;
    std::uint8_t block[BufferSize];
    SCOPEDTRACE([&]{
        return utility::FormatString("handle={}, bufferSize={} flags={}", 
            serialization::Serialize(static_cast<int>(m_handle), 0),
            serialization::Serialize(bufferSize, 0),
            serialization::Serialize(flags, 0));
    }, [&]{
        return utility::FormatString("result={}, data=[{}]", 
            serialization::Serialize(numBytesReceivedTotal, 0),
            serialization::SerializeData(data));
    });

    TraceMessage(__FILE__, __LINE__, __func__, "Receive");
    try
    {
        std::size_t numBytesToReceive = std::min(numBytesToReceiveTotal, BufferSize);
        std::size_t numBytesReceived {};
        TraceDebug(__FILE__, __LINE__, __func__, "numBytesToReceiveTotal={}, numBytesReceivedTotal={} numBytesToReceive={} numBytesReceived={}", 
            serialization::Serialize(numBytesToReceiveTotal, 0),
            serialization::Serialize(numBytesReceivedTotal, 0),
            serialization::Serialize(numBytesToReceive, 0),
            serialization::Serialize(numBytesReceived, 0));
        do
        {
            if (numBytesToReceive > 0)
                numBytesReceived = Receive(block, numBytesToReceive, flags);
            data.insert(data.end(), std::begin(block), std::begin(block) + numBytesReceived);
            numBytesReceivedTotal += numBytesReceived;
            numBytesToReceiveTotal -= numBytesReceived;
            numBytesToReceive = std::min(numBytesToReceiveTotal, BufferSize);
            TraceDebug(__FILE__, __LINE__, __func__, "numBytesToReceiveTotal={}, numBytesReceivedTotal={} numBytesToReceive={} numBytesReceived={}", 
                serialization::Serialize(numBytesToReceiveTotal, 0),
                serialization::Serialize(numBytesReceivedTotal, 0),
                serialization::Serialize(numBytesToReceive, 0),
                serialization::Serialize(numBytesReceived, 0));
        }
        while ((numBytesToReceiveTotal > 0) && (numBytesReceived == numBytesToReceive));
    }
    catch (std::exception & e)
    {
        LogError(__FILE__, __LINE__, __func__, "Receive failed: {}", e.what());
    }
    TraceMessage(__FILE__, __LINE__, __func__, "Received {} bytes", numBytesReceivedTotal);
    return numBytesReceivedTotal;
}

bool Socket::SendBuffer(const ByteBuffer & data, int flags)
{
    bool result {};
    std::size_t numBytesSentTotal {};
    SCOPEDTRACE([&]{
        return utility::FormatString("handle={}, data=[{}], flags={}", 
            serialization::Serialize(static_cast<int>(m_handle), 0),
            serialization::SerializeData(data),
            serialization::Serialize(flags, 0));
    }, [&]{
        return utility::FormatString("result={} bytesSent={}", 
            serialization::Serialize(result, 0),
            serialization::Serialize(numBytesSentTotal, 0));
    });
    TraceMessage(__FILE__, __LINE__, __func__, "Send");
    std::size_t numBytesLeftToSend = data.size();
    std::uint8_t block[BufferSize];
    try
    {
        while (numBytesLeftToSend > 0)
        {
            std::size_t numBytesToSend = std::min(numBytesLeftToSend, BufferSize);
            std::copy_n(&data[numBytesSentTotal], numBytesToSend, std::begin(block));
            if (!Send(block, numBytesToSend, flags))
                break;
            numBytesSentTotal += numBytesToSend;
            numBytesLeftToSend -= numBytesToSend;
        }
    }
    catch (std::exception & e)
    {
        LogError(__FILE__, __LINE__, __func__, "Send failed {}", e.what());
    }
    TraceMessage(__FILE__, __LINE__, __func__, "Sent {} bytes", numBytesSentTotal);
    result = (numBytesLeftToSend == 0);
    return result;
}

std::size_t Socket::ReceiveFrom(sockaddr * address, socklen_t * addressLength, std::uint8_t * data, std::size_t bufferSize)
{
    std::size_t numBytes {};
    SCOPEDTRACE([&]{
        return utility::FormatString("handle={}, address=[{}], bufferSize={}", 
            serialization::Serialize(static_cast<int>(m_handle), 0),
            serialization::Serialize(address, addressLength),
            serialization::Serialize(bufferSize, 0));
    }, [&]{
        return utility::FormatString("result={}, address=[{}], data=[{}]", 
            serialization::Serialize(numBytes, 0),
            serialization::Serialize(address, addressLength),
            serialization::SerializeData(data, numBytes));
    });
    Lock lock(m_mutex);
    TraceMessage(__FILE__, __LINE__, __func__, "Receive");
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
    TraceMessage(__FILE__, __LINE__, __func__, "Received {} bytes", numBytes);
    return numBytes;
}

bool Socket::SendTo(const sockaddr * address, socklen_t addressLength, const std::uint8_t *data, std::size_t numBytesToSend)
{
    bool result {};
    SCOPEDTRACE([&]{
        return utility::FormatString("handle={}, address=[{}], data=[{}]", 
            serialization::Serialize(static_cast<int>(m_handle), 0),
            serialization::Serialize(address, addressLength),
            serialization::SerializeData(data, numBytesToSend));
    }, [&]{
        return utility::FormatString("result={}", 
            serialization::Serialize(result, 0));
    });
    Lock lock(m_mutex);
    TraceMessage(__FILE__, __LINE__, __func__, "Send");
    int numBytes = m_socketAPI.SendTo(m_handle, data, numBytesToSend, 0, address, addressLength);
    if (numBytes == -1)
    {
        int errorCode = GetError();

        tracing::Logging::Error(__FILE__, __LINE__, __func__, utility::Error(errorCode, GetErrorString(errorCode), "sendto() failed"));
    }
    TraceMessage(__FILE__, __LINE__, __func__, "Sent {} bytes", numBytes);
    result = (static_cast<std::size_t>(numBytes) == numBytesToSend);
    return result;
}

bool Socket::ReceiveBlockFrom(sockaddr * address, socklen_t * addressLength, ByteBuffer & data, std::size_t bufferSize)
{
    bool result {};
    std::size_t numBytesReceivedTotal {};
    std::size_t numBytesToReceiveTotal = bufferSize;
    std::uint8_t block[BufferSize];
    SCOPEDTRACE([&]{
        return utility::FormatString("handle={}, address=[{}], bufferSize={}", 
            serialization::Serialize(static_cast<int>(m_handle), 0),
            serialization::Serialize(address, addressLength),
            serialization::Serialize(bufferSize, 0));
    }, [&]{
        return utility::FormatString("result={}, data=[{}]", 
            serialization::Serialize(result, 0),
            serialization::SerializeData(data));
    });

    TraceMessage(__FILE__, __LINE__, __func__, "Receive");
    try
    {
        std::size_t numBytesToReceive = std::min(numBytesToReceiveTotal, BufferSize);
        std::size_t numBytesReceived {};
        TraceDebug(__FILE__, __LINE__, __func__, "numBytesToReceiveTotal={}, numBytesReceivedTotal={} numBytesToReceive={} numBytesReceived={}", 
            serialization::Serialize(numBytesToReceiveTotal, 0),
            serialization::Serialize(numBytesReceivedTotal, 0),
            serialization::Serialize(numBytesToReceive, 0),
            serialization::Serialize(numBytesReceived, 0));
        do
        {
            if (numBytesToReceive > 0)
                numBytesReceived = ReceiveFrom(address, addressLength, block, numBytesToReceive);
            data.insert(data.end(), std::begin(block), std::begin(block) + numBytesReceived);
            numBytesReceivedTotal += numBytesReceived;
            numBytesToReceiveTotal -= numBytesReceived;
            numBytesToReceive = std::min(numBytesToReceiveTotal, BufferSize);
            TraceDebug(__FILE__, __LINE__, __func__, "numBytesToReceiveTotal={}, numBytesReceivedTotal={} numBytesToReceive={} numBytesReceived={}", 
                serialization::Serialize(numBytesToReceiveTotal, 0),
                serialization::Serialize(numBytesReceivedTotal, 0),
                serialization::Serialize(numBytesToReceive, 0),
                serialization::Serialize(numBytesReceived, 0));
        }
        while (numBytesToReceiveTotal > 0);
    }
    catch (std::exception & e)
    {
        LogError(__FILE__, __LINE__, __func__, "Receive failed: {}", e.what());
    }
    TraceMessage(__FILE__, __LINE__, __func__, "Received {} bytes", numBytesReceivedTotal);
    result = (numBytesToReceiveTotal == 0);
    return result;
}

std::size_t Socket::ReceiveBufferFrom(sockaddr * address, socklen_t * addressLength, ByteBuffer & data, std::size_t bufferSize)
{
    std::size_t numBytesReceivedTotal {};
    std::size_t numBytesToReceiveTotal = bufferSize;
    std::uint8_t block[BufferSize];
    SCOPEDTRACE([&]{
        return utility::FormatString("handle={}, address=[{}], bufferSize={}", 
            serialization::Serialize(static_cast<int>(m_handle), 0),
            serialization::Serialize(address, addressLength),
            serialization::Serialize(bufferSize, 0));
    }, [&]{
        return utility::FormatString("result={}, data=[{}]", 
            serialization::Serialize(numBytesReceivedTotal, 0),
            serialization::SerializeData(data));
    });

    TraceMessage(__FILE__, __LINE__, __func__, "Receive");
    try
    {
        std::size_t numBytesToReceive = std::min(numBytesToReceiveTotal, BufferSize);
        std::size_t numBytesReceived {};
        TraceDebug(__FILE__, __LINE__, __func__, "numBytesToReceiveTotal={}, numBytesReceivedTotal={} numBytesToReceive={} numBytesReceived={}", 
            serialization::Serialize(numBytesToReceiveTotal, 0),
            serialization::Serialize(numBytesReceivedTotal, 0),
            serialization::Serialize(numBytesToReceive, 0),
            serialization::Serialize(numBytesReceived, 0));
        do
        {
            if (numBytesToReceive > 0)
                numBytesReceived = ReceiveFrom(address, addressLength, block, numBytesToReceive);
            data.insert(data.end(), std::begin(block), std::begin(block) + numBytesReceived);
            numBytesReceivedTotal += numBytesReceived;
            numBytesToReceiveTotal -= numBytesReceived;
            numBytesToReceive = std::min(numBytesToReceiveTotal, BufferSize);
            TraceDebug(__FILE__, __LINE__, __func__, "numBytesToReceiveTotal={}, numBytesReceivedTotal={} numBytesToReceive={} numBytesReceived={}", 
                serialization::Serialize(numBytesToReceiveTotal, 0),
                serialization::Serialize(numBytesReceivedTotal, 0),
                serialization::Serialize(numBytesToReceive, 0),
                serialization::Serialize(numBytesReceived, 0));
        }
        while ((numBytesToReceiveTotal > 0) && (numBytesReceived == numBytesToReceive));
    }
    catch (std::exception & e)
    {
        LogError(__FILE__, __LINE__, __func__, "Receive failed: {}", e.what());
    }
    TraceMessage(__FILE__, __LINE__, __func__, "Received {} bytes", numBytesReceivedTotal);
    return numBytesReceivedTotal;
}

bool Socket::SendBufferTo(const sockaddr * address, socklen_t addressLength, const ByteBuffer & data)
{
    bool result {};
    SCOPEDTRACE([&]{
        return utility::FormatString("handle={}, address=[{}], data=[{}]", 
            serialization::Serialize(static_cast<int>(m_handle), 0),
            serialization::Serialize(address, addressLength),
            serialization::SerializeData(data));
    }, [&]{
        return utility::FormatString("result={}", 
            serialization::Serialize(result, 0));
    });
    TraceMessage(__FILE__, __LINE__, __func__, "Send");
    std::size_t numBytesLeftToSend = data.size();
    std::size_t numBytesSentTotal {};
    std::uint8_t block[BufferSize];
    try
    {
        while (numBytesLeftToSend > 0)
        {
            std::size_t numBytesToSend = std::min(numBytesLeftToSend, BufferSize);
            std::copy_n(&data[numBytesSentTotal], numBytesToSend, std::begin(block));
            if (!SendTo(address, addressLength, block, numBytesToSend))
                break;
            numBytesSentTotal += numBytesToSend;
            numBytesLeftToSend -= numBytesToSend;
        }
    }
    catch (std::exception & e)
    {
        LogError(__FILE__, __LINE__, __func__, "Send failed {}", e.what());
    }
    TraceMessage(__FILE__, __LINE__, __func__, "Sent {} bytes", numBytesSentTotal);
    result = (numBytesLeftToSend == 0);
    return result;
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
    return utility::FormatString("addressFamily={}, size={}", value->sa_family, size);
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
    return utility::FormatString("addressFamily={}, size={}", value->sa_family, *size);
}

} // namespace serialization
