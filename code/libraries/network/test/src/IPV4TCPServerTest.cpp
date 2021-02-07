#include "GoogleTest.h"

#include "osal/Thread.h"
#include "core/Observable.h"
#include "network-osal/SocketAPIMock.h"
#include "tracing/ScopedTracing.h"
#include "tracing/Tracing.h"
#include "network/IPV4TCPServerConnectionHandler.h"
#include "network/IPV4TCPServerHandler.h"
#include "network/IPV4TCPServer.h"
#include "Utility.h"

namespace network {

using ::testing::_;
using ::testing::DoAll;
using ::testing::Return;
using ::testing::InvokeWithoutArgs;
using ::testing::SaveArg;
using namespace std::placeholders;

#if defined(PLATFORM_WINDOWS)
#pragma warning(disable: 4355)
#endif

class IPV4TCPServerTest : public ::testing::Test
{
public:
    tracing::CategorySet<tracing::TraceCategory> m_savedTraceFilter;
    testing::SocketAPIMock m_api;
    IPV4TCPServerHandler m_serverHandler;
    int m_dataCallbackInvokedTimes;

    IPV4TCPServerTest()
        : m_savedTraceFilter()
        , m_api()
        , m_serverHandler(m_api, std::bind(&IPV4TCPServerTest::DataCallback, this, _1, _2))
        , m_dataCallbackInvokedTimes()
    {}

    void SetUp() override
    {
        m_savedTraceFilter = tracing::GetDefaultTraceFilter();
        tracing::SetDefaultTraceFilter(tracing::TraceCategory::Message | tracing::TraceCategory::Data);
        osal::SetThreadNameSelf("IPV4TCPServerTest");
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

const int NumListeners = 1;

TEST_F(IPV4TCPServerTest, Construct)
{
    IPV4TCPServer server(m_serverHandler);
    EXPECT_FALSE(server.IsStarted());
}

TEST_F(IPV4TCPServerTest, Start)
{
    EXPECT_CALL(m_api, Open(SocketFamily::InternetV4, SocketType::Stream, _)).Times(1);
    EXPECT_CALL(m_api, Close(_)).Times(1);
    EXPECT_CALL(m_api, Bind(_, _, _)).Times(1);
    EXPECT_CALL(m_api, Listen(_, NumListeners)).Times(1);

    IPV4TCPServer server(m_serverHandler);
    server.Start(TestPort, NumListeners, std::chrono::seconds::max());
    EXPECT_TRUE(server.IsStarted());
}

TEST_F(IPV4TCPServerTest, StartStop)
{
    EXPECT_CALL(m_api, Open(SocketFamily::InternetV4, SocketType::Stream, _)).Times(1);
    EXPECT_CALL(m_api, Close(_)).Times(1);
    EXPECT_CALL(m_api, Bind(_, _, _)).Times(1);
    EXPECT_CALL(m_api, Listen(_, NumListeners)).Times(1);
    EXPECT_CALL(m_api, SetBlockingMode(0, true)).Times(4);
    EXPECT_CALL(m_api, Accept(0, _, _)).WillRepeatedly(DoAll(InvokeWithoutArgs([]() { std::this_thread::sleep_for(std::chrono::seconds(1)); }), Return(-1)));

    IPV4TCPServer server(m_serverHandler);
    server.Start(TestPort, NumListeners, std::chrono::seconds::max());
    EXPECT_TRUE(server.IsStarted());
    std::this_thread::sleep_for(std::chrono::seconds(2));
    server.Stop();
    EXPECT_FALSE(server.IsStarted());
}

TEST_F(IPV4TCPServerTest, Accept)
{
    EXPECT_CALL(m_api, Open(SocketFamily::InternetV4, SocketType::Stream, _)).Times(1);
    EXPECT_CALL(m_api, Close(_)).Times(2);
    EXPECT_CALL(m_api, Bind(_, _, _)).Times(1);
    EXPECT_CALL(m_api, Listen(_, NumListeners)).Times(1);
    EXPECT_CALL(m_api, SetBlockingMode(0, true)).Times(6);
    EXPECT_CALL(m_api, Accept(0, _, _))
        .WillOnce(Return(1))
        .WillRepeatedly(DoAll(InvokeWithoutArgs([]() { std::this_thread::sleep_for(std::chrono::seconds(1)); }), Return(-1)));
    std::uint8_t * buffer {};
    EXPECT_CALL(m_api, Receive(1, _, _, 0))
        .WillOnce(DoAll(SaveArg<1>(&buffer), InvokeWithoutArgs([&]() { std::fill(buffer, buffer + 10, std::uint8_t {0x01}); }), Return(10)))
        .WillRepeatedly(Return(0));
    EXPECT_CALL(m_api, Send(1, _, 10, 0))
        .WillOnce(Return(10))
        .WillRepeatedly(Return(0));

    IPV4TCPServer server(m_serverHandler);
    server.Start(TestPort, NumListeners, std::chrono::seconds::max());
    EXPECT_TRUE(server.IsStarted());
    std::this_thread::sleep_for(std::chrono::seconds(2));
    server.Stop();
    EXPECT_FALSE(server.IsStarted());
}

} // namespace network
