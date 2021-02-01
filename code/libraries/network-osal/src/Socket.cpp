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
        return utility::FormatString("socketFamily={}, socketType={}, socketProtocol={}", 
            socketFamily, socketType, socketProtocol);
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
        return utility::FormatString("socketFamily={}, socketType={}, socketProtocol={}", 
            other.m_socketFamily, other.m_socketType, other.m_socketProtocol);
    }, [this]{
        return utility::FormatString("result={}", static_cast<int>(m_handle));
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
        return utility::FormatString("socketFamily={}, socketType={}, socketProtocol={}", 
            other.m_socketFamily, other.m_socketType, other.m_socketProtocol);
    }, [this]{
        return utility::FormatString("result={}", static_cast<int>(m_handle));
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
        return utility::FormatString("result={}", static_cast<int>(m_handle));
    });
    return m_handle;
}

void
Socket::SetHandle(SocketHandle handle)
{
    SCOPEDTRACE([&]{ 
        return utility::FormatString("result={}", static_cast<int>(m_handle));
        }, nullptr);
    m_handle = handle;
}

void
Socket::Open()
{
    SCOPEDTRACE([this]{
        return utility::FormatString("socketFamily={}, socketType={}, protocol={}", 
            m_socketFamily, m_socketType, m_socketProtocol);
    }, [this]{
        return utility::FormatString("result={}", static_cast<int>(m_handle));
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
        return utility::FormatString("result={}", static_cast<int>(m_handle));
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
        return utility::FormatString("handle={}", static_cast<int>(m_handle));
    }, [&]{
        return utility::FormatString("result={}", result);
    });
    result = (m_handle != InvalidHandleValue);
    return result;
}

void Socket::SetSocketOptionWithLevel(SocketOptionLevel level, SocketOption socketOption, const void * optionValue, socklen_t optionLength)
{
    int result {};
    SCOPEDTRACE([&]{
        return utility::FormatString("handle={}, level={}, socketOption={}, optionValue={}, optionLength={}", 
            static_cast<int>(m_handle), level, socketOption, optionValue, optionLength);
    }, [&]{
        return utility::FormatString("result={}", result);
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
            static_cast<int>(m_handle), level, socketOption, optionValue, *optionLength);
    }, [&]{
        return utility::FormatString("result={}, optionLength={}", result, *optionLength);
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
            static_cast<int>(m_handle), socketOption, optionValue, optionLength);
    }, nullptr);
    SetSocketOptionWithLevel(SocketOptionLevel::Socket, socketOption, optionValue, optionLength);
}

void Socket::GetSocketOption(SocketOption socketOption, void * optionValue, socklen_t * optionLength)
{
    SCOPEDTRACE([&]{
        return utility::FormatString("handle={}, socketOption={}, optionValue={}, optionLength={}", 
            static_cast<int>(m_handle),
            socketOption,
            optionValue,
            *optionLength);
    }, [&]{
        return utility::FormatString("optionLength= {}", 
            *optionLength);
    });
    GetSocketOptionWithLevel(SocketOptionLevel::Socket, socketOption, optionValue, optionLength);
}

void Socket::SetSocketOptionBool(SocketOption socketOption, bool value)
{
    SCOPEDTRACE([&]{
        return utility::FormatString("handle={}, socketOption={}, value={}", 
            static_cast<int>(m_handle), socketOption, value);
    }, nullptr);
    int optionValue = value ? 1 : 0;
    SetSocketOption(socketOption, &optionValue, sizeof(optionValue));
}

bool Socket::GetSocketOptionBool(SocketOption socketOption)
{
    bool result {};
    SCOPEDTRACE([&]{
        return utility::FormatString("handle={}, socketOption={}", 
            static_cast<int>(m_handle), socketOption);
    }, [&]{
        return utility::FormatString("result={}", result);
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
            static_cast<int>(m_handle), socketOption, optionValue);
    }, nullptr);
    SetSocketOption(socketOption, &optionValue, sizeof(optionValue));
}

int Socket::GetSocketOptionInt(SocketOption socketOption)
{
    int optionValue {};
    SCOPEDTRACE([&]{
        return utility::FormatString("handle={}, socketOption={}", 
            static_cast<int>(m_handle), socketOption);
    }, [&]{
        return utility::FormatString("result={}", optionValue);
    });
    socklen_t optionLength = sizeof(optionValue);
    GetSocketOption(socketOption, &optionValue, &optionLength);
    return optionValue;
}

void Socket::SetBroadcastOption(bool value)
{
    SCOPEDTRACE([&]{
        return utility::FormatString("handle={}, value={}", 
            static_cast<int>(m_handle), value);
    }, nullptr);
    SetSocketOptionBool(SocketOption::Broadcast, value);
}

bool Socket::GetBroadcastOption()
{
    bool result {};
    SCOPEDTRACE(nullptr, [&]{
        return utility::FormatString("handle={}, result={}", 
            static_cast<int>(m_handle), result);
    });
    result = GetSocketOptionBool(SocketOption::Broadcast);
    return result;
}

void Socket::SetBlockingMode(bool value)
{
    SCOPEDTRACE([&]{
        return utility::FormatString("handle={}, value={}", 
            static_cast<int>(m_handle), value);
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
            static_cast<int>(m_handle), result);
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
            static_cast<int>(m_handle), value);
    }, nullptr);
    SetSocketOptionBool(SocketOption::ReuseAddress, value);
}

bool Socket::GetReuseAddress()
{
    bool result {};
    SCOPEDTRACE(nullptr, [&]{
        return utility::FormatString("handle={}, result={}", 
            static_cast<int>(m_handle), result);
    });
    result = GetSocketOptionBool(SocketOption::ReuseAddress);
    return result;
}

void Socket::SetReceiveTimeout(std::chrono::milliseconds timeout)
{
    SCOPEDTRACE([&]{
        return utility::FormatString("handle={}, timeout={} ms", 
            static_cast<int>(m_handle), timeout.count());
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
            static_cast<int>(m_handle), result.count());
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
            static_cast<int>(m_handle), timeout.count());
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
            static_cast<int>(m_handle), result.count());
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
            static_cast<int>(m_handle), 
            serialization::SerializeAddress(address, addressLength), 
            addressLength);
    }, [&]{
        return utility::FormatString("result={}", 
            result);
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
            static_cast<int>(m_handle), 
            serialization::SerializeAddress(serverAddress, serverAddressLength), 
            serverAddressLength, 
            timeout);
    }, [&]{
        return utility::FormatString("result={}", 
            result != -1);
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
        return utility::FormatString("numListeners={}", numListeners);
    }, [&]{
        return utility::FormatString("result={}", result);
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
            static_cast<int>(m_handle),
            *clientAddressLength,
            timeout);
    }, [&]{
        return utility::FormatString("result={}, clientAddress=[{}], socket={}", 
            result,
            serialization::SerializeAddress(clientAddress, clientAddressLength),
            handle);
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
            static_cast<int>(m_handle), *addressLength);
    }, [&]{
        return utility::FormatString("result={}, address=[{}], addressLength={}", 
            result != -1,
            serialization::SerializeAddress(address, addressLength),
            *addressLength);
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
            static_cast<int>(m_handle), *addressLength);
    }, [&]{
        return utility::FormatString("result={}, address=[{}], addressLength={}", 
            result != -1,
            serialization::SerializeAddress(address, addressLength),
            *addressLength);
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
            static_cast<int>(m_handle),
            bufferSize,
            flags);
    }, [&]{
        return utility::FormatString("result={}, data=[{}]", 
            numBytes,
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
            if (IsOpen())
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

std::size_t Socket::Send(const std::uint8_t * data, std::size_t numBytesToSend, int flags)
{
    std::size_t result {};
    SCOPEDTRACE([&]{
        return utility::FormatString("handle={}, data=[{}], flags={}", 
            static_cast<int>(m_handle),
            serialization::SerializeData(data, numBytesToSend),
            flags);
    }, [&]{
        return utility::FormatString("result={}", 
            result);
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
    result = static_cast<std::size_t>(numBytes);
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
            static_cast<int>(m_handle),
            bufferSize,
            flags);
    }, [&]{
        return utility::FormatString("result={}, data=[{}]", 
            result,
            serialization::SerializeData(data));
    });

    TraceMessage(__FILE__, __LINE__, __func__, "Receive");
    try
    {
        std::size_t numBytesToReceive = std::min(numBytesToReceiveTotal, BufferSize);
        std::size_t numBytesReceived {};
        TraceDebug(__FILE__, __LINE__, __func__, "numBytesToReceiveTotal={}, numBytesReceivedTotal={} numBytesToReceive={} numBytesReceived={}", 
            numBytesToReceiveTotal, numBytesReceivedTotal, numBytesToReceive, numBytesReceived);
        do
        {
            if (numBytesToReceive > 0)
                numBytesReceived = Receive(block, numBytesToReceive, flags);
            if (!IsOpen())
                // Socket was closed as nothing was received
                break;
            data.insert(data.end(), std::begin(block), std::begin(block) + numBytesReceived);
            numBytesReceivedTotal += numBytesReceived;
            numBytesToReceiveTotal -= numBytesReceived;
            numBytesToReceive = std::min(numBytesToReceiveTotal, BufferSize);
            TraceDebug(__FILE__, __LINE__, __func__, "numBytesToReceiveTotal={}, numBytesReceivedTotal={} numBytesToReceive={} numBytesReceived={}", 
                numBytesToReceiveTotal, numBytesReceivedTotal, numBytesToReceive, numBytesReceived);
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

std::size_t Socket::ReceiveBuffer(ByteBuffer & data, std::size_t bufferSize, int flags)
{
    std::size_t numBytesReceivedTotal {};
    std::size_t numBytesToReceiveTotal = bufferSize;
    std::uint8_t block[BufferSize];
    SCOPEDTRACE([&]{
        return utility::FormatString("handle={}, bufferSize={} flags={}", 
            static_cast<int>(m_handle),
            bufferSize,
            flags);
    }, [&]{
        return utility::FormatString("result={}, data=[{}]", 
            numBytesReceivedTotal,
            serialization::SerializeData(data));
    });

    TraceMessage(__FILE__, __LINE__, __func__, "Receive");
    try
    {
        std::size_t numBytesToReceive = std::min(numBytesToReceiveTotal, BufferSize);
        std::size_t numBytesReceived {};
        TraceDebug(__FILE__, __LINE__, __func__, "numBytesToReceiveTotal={}, numBytesReceivedTotal={} numBytesToReceive={} numBytesReceived={}", 
            numBytesToReceiveTotal, numBytesReceivedTotal, numBytesToReceive, numBytesReceived);
        do
        {
            if (numBytesToReceive > 0)
                numBytesReceived = Receive(block, numBytesToReceive, flags);
            data.insert(data.end(), std::begin(block), std::begin(block) + numBytesReceived);
            numBytesReceivedTotal += numBytesReceived;
            numBytesToReceiveTotal -= numBytesReceived;
            numBytesToReceive = std::min(numBytesToReceiveTotal, BufferSize);
            TraceDebug(__FILE__, __LINE__, __func__, "numBytesToReceiveTotal={}, numBytesReceivedTotal={} numBytesToReceive={} numBytesReceived={}", 
                numBytesToReceiveTotal, numBytesReceivedTotal, numBytesToReceive, numBytesReceived);
        }
        while ((numBytesToReceiveTotal > 0) && (numBytesReceived > 0));
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
            static_cast<int>(m_handle),
            serialization::SerializeData(data),
            flags);
    }, [&]{
        return utility::FormatString("result={} bytesSent={}", 
            result, numBytesSentTotal);
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
            std::size_t numBytesSent = Send(block, numBytesToSend, flags);
            if (numBytesSent == 0)
                break;
            numBytesSentTotal += numBytesSent;
            numBytesLeftToSend -= numBytesSent;
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
            static_cast<int>(m_handle),
            serialization::SerializeAddress(address, addressLength),
            bufferSize);
    }, [&]{
        return utility::FormatString("result={}, address=[{}], data=[{}]", 
            numBytes,
            serialization::SerializeAddress(address, addressLength),
            serialization::SerializeData(data, numBytes));
    });
    Lock lock(m_mutex);
    TraceMessage(__FILE__, __LINE__, __func__, "Receive");
    try
    {
        int result = m_socketAPI.ReceiveFrom(m_handle, data, bufferSize, 0, address, addressLength);
        if (result == -1)
        {
            int errorCode = GetError();

            if ((errorCode != EINTR) && (errorCode != EWOULDBLOCK) && (errorCode != EAGAIN))
                tracing::Logging::Error(__FILE__, __LINE__, __func__, utility::Error(errorCode, GetErrorString(errorCode), "recvfrom() failed"));
        } else if (result == 0)
        {
            // Read is blocking, if 0 bytes are returned the other side is closed
            tracing::Logging::Error(__FILE__, __LINE__, __func__, utility::GenericError("recvfrom() returned nothing, closing socket"));
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

std::size_t Socket::SendTo(const sockaddr * address, socklen_t addressLength, const std::uint8_t *data, std::size_t numBytesToSend)
{
    std::size_t result {};
    SCOPEDTRACE([&]{
        return utility::FormatString("handle={}, address=[{}], data=[{}]", 
            static_cast<int>(m_handle),
            serialization::SerializeAddress(address, addressLength),
            serialization::SerializeData(data, numBytesToSend));
    }, [&]{
        return utility::FormatString("result={}", result);
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
    result = static_cast<std::size_t>(numBytes);
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
            static_cast<int>(m_handle),
            serialization::SerializeAddress(address, addressLength),
            bufferSize);
    }, [&]{
        return utility::FormatString("result={}, data=[{}]", 
            result,
            serialization::SerializeData(data));
    });

    TraceMessage(__FILE__, __LINE__, __func__, "Receive");
    try
    {
        std::size_t numBytesToReceive = std::min(numBytesToReceiveTotal, BufferSize);
        std::size_t numBytesReceived {};
        TraceDebug(__FILE__, __LINE__, __func__, "numBytesToReceiveTotal={}, numBytesReceivedTotal={} numBytesToReceive={} numBytesReceived={}", 
            numBytesToReceiveTotal, numBytesReceivedTotal, numBytesToReceive, numBytesReceived);
        do
        {
            if (numBytesToReceive > 0)
                numBytesReceived = ReceiveFrom(address, addressLength, block, numBytesToReceive);
            if (!IsOpen())
                // Socket was closed as nothing was received
                break;
            data.insert(data.end(), std::begin(block), std::begin(block) + numBytesReceived);
            numBytesReceivedTotal += numBytesReceived;
            numBytesToReceiveTotal -= numBytesReceived;
            numBytesToReceive = std::min(numBytesToReceiveTotal, BufferSize);
            TraceDebug(__FILE__, __LINE__, __func__, "numBytesToReceiveTotal={}, numBytesReceivedTotal={} numBytesToReceive={} numBytesReceived={}", 
                numBytesToReceiveTotal, numBytesReceivedTotal, numBytesToReceive, numBytesReceived);
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
            static_cast<int>(m_handle),
            serialization::SerializeAddress(address, addressLength),
            bufferSize);
    }, [&]{
        return utility::FormatString("result={}, data=[{}]", 
            numBytesReceivedTotal,
            serialization::SerializeData(data));
    });

    TraceMessage(__FILE__, __LINE__, __func__, "Receive");
    try
    {
        std::size_t numBytesToReceive = std::min(numBytesToReceiveTotal, BufferSize);
        std::size_t numBytesReceived {};
        TraceDebug(__FILE__, __LINE__, __func__, "numBytesToReceiveTotal={}, numBytesReceivedTotal={} numBytesToReceive={} numBytesReceived={}", 
            numBytesToReceiveTotal, numBytesReceivedTotal, numBytesToReceive, numBytesReceived);
        do
        {
            if (numBytesToReceive > 0)
                numBytesReceived = ReceiveFrom(address, addressLength, block, numBytesToReceive);
            data.insert(data.end(), std::begin(block), std::begin(block) + numBytesReceived);
            numBytesReceivedTotal += numBytesReceived;
            numBytesToReceiveTotal -= numBytesReceived;
            numBytesToReceive = std::min(numBytesToReceiveTotal, BufferSize);
            TraceDebug(__FILE__, __LINE__, __func__, "numBytesToReceiveTotal={}, numBytesReceivedTotal={} numBytesToReceive={} numBytesReceived={}", 
                numBytesToReceiveTotal, numBytesReceivedTotal, numBytesToReceive, numBytesReceived);
        }
        while ((numBytesToReceiveTotal > 0) && (numBytesReceived > 0));
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
            static_cast<int>(m_handle),
            serialization::SerializeAddress(address, addressLength),
            serialization::SerializeData(data));
    }, [&]{
        return utility::FormatString("result={}", 
            result);
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
            std::size_t numBytesSent = SendTo(address, addressLength, block, numBytesToSend);
            if (numBytesSent == 0)
                break;
            numBytesSentTotal += numBytesSent;
            numBytesLeftToSend -= numBytesSent;
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

std::string SerializeAddress(const sockaddr * value, socklen_t size)
{
    if (value == nullptr)
        return "null";
    return utility::FormatString("addressFamily={}, size={}", value->sa_family, size);
}

std::string SerializeAddress(sockaddr * value, socklen_t size)
{
    return SerializeAddress(const_cast<const sockaddr *>(value), size);
}

std::string SerializeAddress(const sockaddr * value, socklen_t * size)
{
    if (value == nullptr)
        return "null";
    if (size == nullptr)
        return "????";
    return utility::FormatString("addressFamily={}, size={}", value->sa_family, *size);
}

} // namespace serialization
