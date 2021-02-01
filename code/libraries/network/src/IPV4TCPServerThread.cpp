#include "network/IPV4TCPServerThread.h"

#include "tracing/ScopedTracing.h"
#include "network/IPV4TCPServerSocket.h"

namespace network {

const std::chrono::milliseconds IPV4TCPServerThread::WaitTime(1000);

IPV4TCPServerThread::IPV4TCPServerThread(ISocketAPI & api)
    : core::ActiveObject("IPV4TCPServerThread")
    , m_socketAPI(api)
    , m_port()
    , m_numListeners()
    , m_acceptTimeout()
    , m_abortThread()
{
    SCOPEDTRACE([this] () { return utility::FormatString("name={}", GetName()); }, nullptr);
}

IPV4TCPServerThread::~IPV4TCPServerThread()
{
    SCOPEDTRACE(nullptr, nullptr);
    if (IsStarted())
        Stop();
}

bool IPV4TCPServerThread::Start(PortType port, int numListeners, std::chrono::milliseconds acceptTimeout)
{
    m_port = port;
    m_numListeners = numListeners;
    m_acceptTimeout = acceptTimeout;
    SCOPEDTRACE([this] () { return utility::FormatString("Thread starting on port {}", m_port); }, nullptr);
    Create();

    return true;
}

void IPV4TCPServerThread::Stop()
{
    SCOPEDTRACE([this] () { return utility::FormatString("Thread stopping on port {}", m_port); }, nullptr);
    Kill();
}

bool IPV4TCPServerThread::IsStarted()
{
    SCOPEDTRACE(nullptr, nullptr);
    return IsRunning();
}

void IPV4TCPServerThread::Run()
{
    SCOPEDTRACE([this] () { return utility::FormatString("Run starting"); }, 
                [this] () { return utility::FormatString("Run stopped"); });
    IPV4TCPServerSocket serverSocket(m_socketAPI, m_port, m_numListeners, m_acceptTimeout);
    serverSocket.Initialize();
    m_abortThread = false;
    TraceMessage(__FILE__, __LINE__, __func__, "Starting {}", GetName());
    while (!IsDying() && !m_abortThread)
    {
        DoConnectionCleanup();
        if (ReadyToAccept())
        {
            IPV4TCPSocket clientSocket(m_socketAPI);
            IPV4EndPoint clientAddress;
            bool accepted = serverSocket.Accept(clientSocket, clientAddress);
            if (accepted)
            {
                if (!OnAccepted(std::move(clientSocket), clientAddress))
                {
                    TraceMessage(__FILE__, __LINE__, __func__, "Connection refused {}", GetName());
                }
                else
                {
                    TraceMessage(__FILE__, __LINE__, __func__, "Connection accepted {}", GetName());
                }
            }
            else
            {
                TraceMessage(__FILE__, __LINE__, __func__, "Accept failed {}", GetName());
            }
        }
        else
        {
            std::this_thread::sleep_for(m_acceptTimeout);
        }
    }
    ForceConnectionClose();
    TraceMessage(__FILE__, __LINE__, __func__, "Shutting down {}", GetName());
    serverSocket.Uninitialize();
}

void IPV4TCPServerThread::FlushThread()
{
    TraceMessage(__FILE__, __LINE__, __func__, "Flush thread {}", GetName());
    m_abortThread = true;
}

} // namespace network
