#pragma once

#include "network/IPV4TCPServerConnectionHandler.h"
#include "network/IPV4TCPServerThread.h"

namespace network {

using IPV4TCPConnectionHandlerPtr = std::shared_ptr<IPV4TCPServerConnectionHandler>;

class IPV4TCPServerHandler
    : public IPV4TCPServerThread
    , public IClosedConnectionCallback
{
public:
    typedef std::recursive_mutex Mutex;
    typedef std::lock_guard<Mutex> Lock;

    Mutex m_mutex;
    std::vector<IPV4TCPConnectionHandlerPtr> m_connectionHandlers;
    std::vector<IPV4TCPConnectionHandlerPtr> m_connectionHandlersClosed;
    DataCallback m_dataCallback;

public:
    IPV4TCPServerHandler(ISocketAPI & api, DataCallback dataCallback);
    ~IPV4TCPServerHandler();
    
    bool Start(PortType port, int numListeners, std::chrono::milliseconds acceptTimeout);
    void DoConnectionCleanup() override;
    void ForceConnectionClose() override;
    bool ReadyToAccept() override;
    bool OnAccepted(IPV4TCPSocket && clientSocket, const IPV4EndPoint & clientAddress) override;

    // Runs on connection handler thread
    void OnConnectionClosed(IPV4TCPServerConnectionThread * connection) override;
};

} // namespace network