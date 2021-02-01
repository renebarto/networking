#include "network/IPV4TCPServerConnectionHandler.h"

#include "tracing/Logging.h"
#include "tracing/ScopedTracing.h"

namespace network {

IPV4TCPServerConnectionHandler::IPV4TCPServerConnectionHandler(ISocketAPI & api, DataCallback dataCallback)
    : IPV4TCPServerConnectionThread(api)
    , m_dataCallback(dataCallback)
    , m_abortThread()
{
    SCOPEDTRACE(nullptr, nullptr);
}

IPV4TCPServerConnectionHandler::~IPV4TCPServerConnectionHandler()
{
    SCOPEDTRACE(nullptr, nullptr);
}

void IPV4TCPServerConnectionHandler::Run()
{
    SCOPEDTRACE(nullptr, nullptr);
    TraceMessage(__FILE__, __LINE__, __func__, "Starting connection handler");
    m_abortThread = false;
    while (!IsDying() && !m_abortThread)
    {
        ByteBuffer receivedData;
        ByteBuffer dataToSend;
        if (!Receive(receivedData))
        {
            TraceMessage(__FILE__, __LINE__, __func__, "Receive signalling to stop connection");
            m_abortThread = true;
            continue;
        }
        if (!m_dataCallback)
        {
            TraceMessage(__FILE__, __LINE__, __func__, "No data handling installed, stop connection");
            m_abortThread = true;
            continue;
        }
        if (!m_dataCallback(receivedData, dataToSend))
        {
            TraceMessage(__FILE__, __LINE__, __func__, "Data handling signalling to stop connection");
            m_abortThread = true;
            continue;
        }
        if (!Send(dataToSend))
        {
            TraceMessage(__FILE__, __LINE__, __func__, "Send signalling to stop connection");
            m_abortThread = true;
        }
    }
    TraceMessage(__FILE__, __LINE__, __func__, "Shutting down connection handler");
    ForAll([this] (IClosedConnectionCallback * listener) { listener->OnConnectionClosed(this); });
}

void IPV4TCPServerConnectionHandler::FlushThread()
{
    SCOPEDTRACE(nullptr, nullptr);
    m_abortThread = true;
}

bool IPV4TCPServerConnectionHandler::Receive(ByteBuffer & buffer)
{
    bool result {};
    SCOPEDTRACE(nullptr, [&] () {
        return utility::FormatString("result={}", result);
    });
    std::size_t bytesReceived = m_clientSocket.ReceiveBuffer(buffer, 4096, 0);
    result = (bytesReceived != 0);
    return result;
}

bool IPV4TCPServerConnectionHandler::Send(const ByteBuffer & buffer)
{
    bool result {};
    SCOPEDTRACE(nullptr, [&] () {
        return utility::FormatString("result={}", result);
    });

    result = m_clientSocket.SendBuffer(buffer, 0);
    return result;
}

} // namespace network
