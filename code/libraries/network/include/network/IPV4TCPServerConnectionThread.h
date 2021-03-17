#pragma once

#include <vector>
#include "core/threading/ActiveObject.h"
#include "network/IPV4TCPSocket.h"

namespace network {

class IPV4TCPServerConnectionThread 
    : private core::threading::ActiveObject
{
protected:
    ISocketAPI & m_socketAPI;
    IPV4TCPSocket m_clientSocket;
    IPV4EndPoint m_clientEndPoint;
    std::atomic<bool> m_abortThread;

public:
    IPV4TCPServerConnectionThread(ISocketAPI & api);
    virtual ~IPV4TCPServerConnectionThread();
    
    bool Start(IPV4TCPSocket && clientSocket, const IPV4EndPoint & clientEndPoint);
    void Stop();
    bool IsStarted();

    // To be overridden by inheriting class:
    using ActiveObject::GetName;
    using ActiveObject::IsDying;
    void Run() = 0;
    void FlushThread() = 0;
};

} // namespace network