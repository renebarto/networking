#pragma once

#include <vector>
#include "core/ActiveObject.h"
#include "network/IPV4TCPSocket.h"

namespace network {

class IIPV4TCPClientConnectionHandler
{
public:
    virtual ~IIPV4TCPClientConnectionHandler() {}

    virtual bool Start(IPV4TCPSocket & clientSocket, const IPV4EndPoint & clientEndPoint) = 0;
    virtual void Stop() = 0;
    virtual bool IsStarted() = 0;
};

class IPV4TCPClientConnectionThread 
    : private core::ActiveObject
    , public IIPV4TCPClientConnectionHandler
{
protected:
    ISocketAPI & m_socketAPI;
    IPV4TCPSocket m_clientSocket;
    IPV4EndPoint m_clientEndPoint;
    std::atomic<bool> m_abortThread;

public:
    IPV4TCPClientConnectionThread(ISocketAPI & api);
    
    bool Start(IPV4TCPSocket & clientSocket, const IPV4EndPoint & clientEndPoint) override;
    void Stop() override;
    bool IsStarted() override;

    // To be overridden by inheriting class:
    using ActiveObject::GetName;
    using ActiveObject::IsDying;
    void Run() = 0;
    void FlushThread() = 0;
};

} // namespace network