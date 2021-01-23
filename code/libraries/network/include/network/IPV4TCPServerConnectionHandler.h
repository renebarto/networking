#pragma once

#include "core/Observable.h"
#include "network/IPV4TCPServerConnectionThread.h"

namespace network {

class IClosedConnectionCallback
{
public:
    virtual ~IClosedConnectionCallback() {};
    virtual void OnConnectionClosed(IPV4TCPServerConnectionThread * connection) = 0;
};

using DataCallback = std::function<bool (const ByteBuffer &, ByteBuffer &)>;

class IPV4TCPServerConnectionHandler
    : public IPV4TCPServerConnectionThread
    , public core::Observable<IClosedConnectionCallback>
{
public:
    DataCallback m_dataCallback;
    bool m_abortThread;

    IPV4TCPServerConnectionHandler(ISocketAPI & api, DataCallback dataCallback);
    virtual ~IPV4TCPServerConnectionHandler();
    void Run() override;
    void FlushThread() override;

    bool Receive(ByteBuffer & buffer);
    bool Send(const ByteBuffer & buffer);
};

} // namespace network