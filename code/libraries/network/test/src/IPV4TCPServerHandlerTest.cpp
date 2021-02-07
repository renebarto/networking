#include "GoogleTest.h"

#include "osal/Thread.h"
#include "core/Observable.h"
#include "network-osal/SocketAPIMock.h"
#include "tracing/ScopedTracing.h"
#include "tracing/Tracing.h"
#include "network/IPV4TCPClient.h"
#include "network/IPV4TCPServerHandler.h"
#include "Utility.h"

namespace network {

using ::testing::_;
using ::testing::DoAll;
using ::testing::InvokeWithoutArgs;
using ::testing::Return;
using namespace std::placeholders;

static const int NumListeners = 1;

class IPV4TCPServerHandlerForTest
    : public IPV4TCPServerHandler
{
public:
    int m_doConnectionCleanup;
    int m_forceConnectionClose;
    int m_readyToAccept;
    int m_onAccepted;
    int m_onConnectionClosed;

    IPV4TCPServerHandlerForTest(ISocketAPI& api, DataCallback dataCallback)
        : IPV4TCPServerHandler(api, dataCallback)
        , m_doConnectionCleanup()
        , m_forceConnectionClose()
        , m_readyToAccept()
        , m_onAccepted()
        , m_onConnectionClosed()
    {}

    void DoConnectionCleanup() override
    {
        ++m_doConnectionCleanup;
        IPV4TCPServerHandler::DoConnectionCleanup();
    }
    void ForceConnectionClose() override
    {
        ++m_forceConnectionClose;
        IPV4TCPServerHandler::ForceConnectionClose();
    }
    bool ReadyToAccept() override
    {
        ++m_readyToAccept;
        return IPV4TCPServerHandler::ReadyToAccept();
    }
    bool OnAccepted(IPV4TCPSocket&& clientSocket, const IPV4EndPoint& clientAddress) override
    {
        ++m_onAccepted;
        return IPV4TCPServerHandler::OnAccepted(std::move(clientSocket), clientAddress);
    }

    // Runs on connection handler thread
    void OnConnectionClosed(IPV4TCPServerConnectionThread* connection) override
    {
        ++m_onConnectionClosed;
        IPV4TCPServerHandler::OnConnectionClosed(connection);
    }

};


class IPV4TCPServerHandlerTest : public ::testing::Test
{
public:
    tracing::CategorySet<tracing::TraceCategory> m_savedTraceFilter;
    int m_dataCallbackInvokedTimes;

    IPV4TCPServerHandlerTest()
        : m_savedTraceFilter()
        , m_dataCallbackInvokedTimes()
    {}

    void SetUp() override
    {
        m_savedTraceFilter = tracing::GetDefaultTraceFilter();
        tracing::SetDefaultTraceFilter(tracing::TraceCategory::Message | tracing::TraceCategory::Data);
        osal::SetThreadNameSelf("IPV4TCPServerConnectionHandlerTest");
    }
    void TearDown() override
    {
        tracing::SetDefaultTraceFilter(m_savedTraceFilter);
    }
    bool DataCallback(const ByteBuffer & dataReceived, ByteBuffer & dataToSend)
    {
        TraceMessage(__FILE__, __LINE__, __func__, "Data in: {}", serialization::SerializeData(dataReceived.data(), dataReceived.size()));
        dataToSend = dataReceived;
        ++m_dataCallbackInvokedTimes;
        if (m_dataCallbackInvokedTimes >= 2)
            return false;
        return true;
    }
};


TEST_F(IPV4TCPServerHandlerTest, Construct)
{
    testing::SocketAPIMock api;

    EXPECT_CALL(api, Open(SocketFamily::InternetV4, SocketType::Stream, _)).Times(0);
    EXPECT_CALL(api, Close(_)).Times(0);

    IPV4TCPServerHandler handler(api, std::bind(&IPV4TCPServerHandlerTest::DataCallback, this, _1, _2));
    EXPECT_FALSE(handler.IsStarted());
}

TEST_F(IPV4TCPServerHandlerTest, Start)
{
    testing::SocketAPIMock api;

    EXPECT_CALL(api, Open(SocketFamily::InternetV4, SocketType::Stream, _)).Times(1);
    EXPECT_CALL(api, Close(_)).Times(2); // One plus on for Accept
    EXPECT_CALL(api, Bind(0, _, _)).Times(1);
    EXPECT_CALL(api, Listen(0, NumListeners)).Times(1);
    EXPECT_CALL(api, SetBlockingMode(0, false)).Times(2); // Will twice to Accept, second will fail
    EXPECT_CALL(api, SetBlockingMode(0, true)).Times(2);
    EXPECT_CALL(api, Accept(0, _, _)).WillOnce(Return(0)).WillRepeatedly(DoAll(InvokeWithoutArgs([]() { std::this_thread::sleep_for(std::chrono::seconds(1)); }), Return(-1)));
    EXPECT_CALL(api, Receive(0, _, 4096, 0)).Times(1);

    IPV4TCPServerHandlerForTest handler(api, std::bind(&IPV4TCPServerHandlerTest::DataCallback, this, _1, _2));
    EXPECT_TRUE(handler.Start(TestPort, NumListeners, std::chrono::seconds(1)));
    EXPECT_TRUE(handler.IsStarted());
    std::this_thread::sleep_for(std::chrono::seconds(1));
    EXPECT_EQ(2, handler.m_doConnectionCleanup);
    EXPECT_EQ(0, handler.m_forceConnectionClose);
    EXPECT_EQ(2, handler.m_readyToAccept);
    EXPECT_EQ(1, handler.m_onAccepted);
    EXPECT_EQ(1, handler.m_onConnectionClosed);
}

TEST_F(IPV4TCPServerHandlerTest, StartStop)
{
    testing::SocketAPIMock api;

    EXPECT_CALL(api, Open(SocketFamily::InternetV4, SocketType::Stream, _)).Times(1);
    EXPECT_CALL(api, Close(_)).Times(2); // One plus on for Accept
    EXPECT_CALL(api, Bind(0, _, _)).Times(1);
    EXPECT_CALL(api, Listen(0, NumListeners)).Times(1);
    EXPECT_CALL(api, SetBlockingMode(0, false)).Times(2); // Will twice to Accept, second will fail
    EXPECT_CALL(api, SetBlockingMode(0, true)).Times(2);
    EXPECT_CALL(api, Accept(0, _, _)).WillOnce(Return(0)).WillRepeatedly(DoAll(InvokeWithoutArgs([]() { std::this_thread::sleep_for(std::chrono::seconds(1)); }), Return(-1)));
    EXPECT_CALL(api, Receive(0, _, 4096, 0)).Times(1);

    IPV4TCPServerHandlerForTest handler(api, std::bind(&IPV4TCPServerHandlerTest::DataCallback, this, _1, _2));
    EXPECT_TRUE(handler.Start(TestPort, NumListeners, std::chrono::seconds(1)));
    EXPECT_TRUE(handler.IsStarted());
    std::this_thread::sleep_for(std::chrono::seconds(1));
    handler.Stop();
    EXPECT_EQ(2, handler.m_doConnectionCleanup);
    EXPECT_EQ(1, handler.m_forceConnectionClose);
    EXPECT_EQ(2, handler.m_readyToAccept);
    EXPECT_EQ(1, handler.m_onAccepted);
    EXPECT_EQ(1, handler.m_onConnectionClosed);

}

} // namespace network
