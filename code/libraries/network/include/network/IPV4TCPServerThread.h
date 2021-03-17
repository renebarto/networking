#pragma once

#include <vector>
#include "core/threading/ActiveObject.h"
#include "network/IPV4TCPSocket.h"
#include "network/IPV4TCPServerConnectionThread.h"

namespace network {

class IPV4TCPServerThread 
    : private core::threading::ActiveObject
{
protected:
    ISocketAPI & m_socketAPI;
    PortType m_port;
    int m_numListeners;
    std::chrono::milliseconds m_acceptTimeout;
    std::atomic<bool> m_abortThread;
    static const std::chrono::milliseconds WaitTime;

public:
    IPV4TCPServerThread(ISocketAPI & api);
    virtual ~IPV4TCPServerThread();
    
    virtual bool Start(PortType port, int numListeners, std::chrono::milliseconds acceptTimeout);
    virtual void Stop();
    bool IsStarted();
    
    virtual void DoConnectionCleanup() = 0;
    virtual void ForceConnectionClose() = 0;
    virtual bool ReadyToAccept() = 0;
    virtual bool OnAccepted(IPV4TCPSocket && clientSocket, const IPV4EndPoint & clientAddress) = 0;

    void Run() override;   
    void FlushThread() override;
};

} // namespace network