#include "GoogleTest.h"

#include "osal/ThreadFunctions.h"
#include "network-osal/SocketAPIMock.h"
#include "tracing/ScopedTracing.h"
#include "tracing/Tracing.h"
#include "network/IPV4TCPClientSocket.h"
#include "Utility.h"

namespace network {

using ::testing::_;
using ::testing::Return;

class IPV4TCPClientSocketTest : public ::testing::Test
{
public:
    tracing::CategorySet<tracing::TraceCategory> m_savedTraceFilter;

    IPV4TCPClientSocketTest()
        : m_savedTraceFilter()
    {}

    void SetUp() override
    {
        m_savedTraceFilter = tracing::GetDefaultTraceFilter();
        tracing::SetDefaultTraceFilter(tracing::TraceCategory::Information | tracing::TraceCategory::Data);
        osal::SetThreadNameSelf("IPV4TCPClientSocketTest");
    }
    void TearDown() override
    {
        tracing::SetDefaultTraceFilter(m_savedTraceFilter);
    }
};

TEST_F(IPV4TCPClientSocketTest, Construct)
{
    testing::SocketAPIMock api;

    EXPECT_CALL(api, Open(SocketFamily::InternetV4, SocketType::Stream, _)).Times(0);
    EXPECT_CALL(api, Close(_)).Times(0);

    IPV4TCPClientSocket client(api);
    EXPECT_FALSE(client.IsConnected());
}

TEST_F(IPV4TCPClientSocketTest, ConnectNoTimeout)
{
    testing::SocketAPIMock api;
    EXPECT_CALL(api, Open(SocketFamily::InternetV4, SocketType::Stream, _)).Times(1);
    EXPECT_CALL(api, Close(_)).Times(1);
    EXPECT_CALL(api, Connect(_, _, _)).WillOnce(Return(0));
    EXPECT_CALL(api, SetBlockingMode(_, false)).Times(0);
    EXPECT_CALL(api, SetBlockingMode(_, true)).Times(2);

    IPV4EndPoint serverAddress(IPV4Address::LocalHost, TestPort);
    IPV4TCPClientSocket client(api);
    EXPECT_TRUE(client.Connect(serverAddress, std::chrono::milliseconds::max()));
    EXPECT_TRUE(client.IsConnected());
}

TEST_F(IPV4TCPClientSocketTest, ConnectWithTimeout)
{
    testing::SocketAPIMock api;
    EXPECT_CALL(api, Open(SocketFamily::InternetV4, SocketType::Stream, _)).Times(1);
    EXPECT_CALL(api, Close(_)).Times(1);
    EXPECT_CALL(api, Connect(_, _, _)).WillOnce(Return(0));
    EXPECT_CALL(api, SetBlockingMode(_, false)).Times(1);
    EXPECT_CALL(api, SetBlockingMode(_, true)).Times(1);

    IPV4EndPoint serverAddress(IPV4Address::LocalHost, TestPort);
    IPV4TCPClientSocket client(api);
    EXPECT_TRUE(client.Connect(serverAddress, std::chrono::seconds(1)));
    EXPECT_TRUE(client.IsConnected());
}

TEST_F(IPV4TCPClientSocketTest, ConnectTwiceFails)
{
    testing::SocketAPIMock api;
    EXPECT_CALL(api, Open(SocketFamily::InternetV4, SocketType::Stream, _)).Times(1);
    EXPECT_CALL(api, Close(_)).Times(1);
    EXPECT_CALL(api, Connect(_, _, _)).WillOnce(Return(0));
    EXPECT_CALL(api, SetBlockingMode(_, false)).Times(1);
    EXPECT_CALL(api, SetBlockingMode(_, true)).Times(1);

    IPV4EndPoint serverAddress(IPV4Address::LocalHost, TestPort);
    IPV4TCPClientSocket client(api);
    EXPECT_TRUE(client.Connect(serverAddress, std::chrono::seconds(1)));
    EXPECT_TRUE(client.IsConnected());
    EXPECT_FALSE(client.Connect(serverAddress, std::chrono::seconds(1)));
    EXPECT_TRUE(client.IsConnected());
}

TEST_F(IPV4TCPClientSocketTest, ConnectAfterConnectAndDisconnectSucceeds)
{
    testing::SocketAPIMock api;
    EXPECT_CALL(api, Open(SocketFamily::InternetV4, SocketType::Stream, _)).Times(2);
    EXPECT_CALL(api, Close(_)).Times(2);
    EXPECT_CALL(api, Connect(_, _, _)).WillOnce(Return(0)).WillOnce(Return(0));
    EXPECT_CALL(api, SetBlockingMode(_, false)).Times(2);
    EXPECT_CALL(api, SetBlockingMode(_, true)).Times(2);

    IPV4EndPoint serverAddress(IPV4Address::LocalHost, TestPort);
    IPV4TCPClientSocket client(api);
    EXPECT_TRUE(client.Connect(serverAddress, std::chrono::seconds(1)));
    EXPECT_TRUE(client.IsConnected());
    client.Disconnect();
    EXPECT_FALSE(client.IsConnected());
    EXPECT_TRUE(client.Connect(serverAddress, std::chrono::seconds(1)));
    EXPECT_TRUE(client.IsConnected());
}

} // namespace network
