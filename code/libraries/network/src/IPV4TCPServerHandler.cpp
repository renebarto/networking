#include "network/IPV4TCPServerHandler.h"

#include "tracing/ScopedTracing.h"

namespace network {

IPV4TCPServerHandler::IPV4TCPServerHandler(ISocketAPI & api, DataCallback dataCallback)
    : IPV4TCPServerThread(api)
    , m_mutex()
    , m_connectionHandlers()
    , m_connectionHandlersClosed()
    , m_dataCallback(dataCallback)
{        
}

IPV4TCPServerHandler::~IPV4TCPServerHandler()
{
    SCOPEDTRACE(nullptr, nullptr);
    if (IsStarted())
        Stop();
    ForceConnectionClose();
    DoConnectionCleanup();
}

bool IPV4TCPServerHandler::Start(PortType port, int numListeners, std::chrono::milliseconds acceptTimeout)
{
    SCOPEDTRACE([&] () { return utility::FormatString("port={}, numListeners={}, acceptTimeout={}", port, numListeners, acceptTimeout); }, 
                nullptr);
    return IPV4TCPServerThread::Start(port, numListeners, acceptTimeout);
}
void IPV4TCPServerHandler::DoConnectionCleanup()
{
    SCOPEDTRACE(nullptr, 
                nullptr);
    Lock lock(m_mutex);
    for (auto & connectionHandler : m_connectionHandlersClosed)
    {
        connectionHandler->FlushThread();
        TraceMessage(__FILE__, __LINE__, __func__, "Stop old connection {}", connectionHandler->GetName());
        connectionHandler->Stop();
        TraceMessage(__FILE__, __LINE__, __func__, "Remove old connection {}", connectionHandler->GetName());
        connectionHandler.reset();
    }
    m_connectionHandlersClosed.clear();
}
void IPV4TCPServerHandler::ForceConnectionClose()
{
    SCOPEDTRACE(nullptr, 
                nullptr);
    Lock lock(m_mutex);
    for (auto & connectionHandler : m_connectionHandlers)
    {
        connectionHandler->Unsubscribe(this);
        connectionHandler->FlushThread();
        TraceMessage(__FILE__, __LINE__, __func__, "Stop old connection {}", connectionHandler->GetName());
        connectionHandler->Stop();
        TraceMessage(__FILE__, __LINE__, __func__, "Remove old connection {}", connectionHandler->GetName());
        connectionHandler.reset();
    }
    m_connectionHandlers.clear();
}
bool IPV4TCPServerHandler::ReadyToAccept()
{
    bool result {};
    SCOPEDTRACE(nullptr, 
                [&] () { return utility::FormatString("result={}", result);});
    result = true;
    return result;
}
bool IPV4TCPServerHandler::OnAccepted(IPV4TCPSocket && clientSocket, const IPV4EndPoint & clientAddress)
{
    bool connectionAdded {};
    SCOPEDTRACE([&] () { return utility::FormatString("clientAddress=[{}]", clientAddress); }, 
                [&] () { return utility::FormatString("connectionAdded={}", connectionAdded);});
    TraceMessage(__FILE__, __LINE__, __func__, "Add connection for {}", clientAddress);
    auto connectionHandler = std::make_shared<IPV4TCPServerConnectionHandler>(m_socketAPI, m_dataCallback);
    connectionHandler->Subscribe(this);
    connectionHandler->Start(std::move(clientSocket), clientAddress);

    Lock lock(m_mutex);
    m_connectionHandlers.push_back(connectionHandler);
    connectionAdded = true;
    return connectionAdded;
}

// Runs on connection handler thread
void IPV4TCPServerHandler::OnConnectionClosed(IPV4TCPServerConnectionThread * connection)
{
    SCOPEDTRACE([&] () { return utility::FormatString("connection={}", connection); }, 
                nullptr);
    TraceMessage(__FILE__, __LINE__, __func__, "Connection closed");
    IPV4TCPServerConnectionHandler * connectionHandler = dynamic_cast<IPV4TCPServerConnectionHandler *>(connection);

    Lock lock(m_mutex);
    auto it = m_connectionHandlers.begin();
    bool found {};
    while (!found && (it != m_connectionHandlers.end()))
    {
        if (it->get() == connectionHandler)
        {
            found = true;
            break;
        }
        ++it;
    }
    if (found)
    {
        auto handler = *it;
        handler->Unsubscribe(this);
        m_connectionHandlersClosed.push_back(handler);
        m_connectionHandlers.erase(it);
    }
}

} // namespace network
