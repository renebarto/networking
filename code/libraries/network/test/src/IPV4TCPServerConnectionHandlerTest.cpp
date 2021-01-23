#include "GoogleTest.h"

#include "network-osal/SocketAPIMock.h"
#include "network/IPV4TCPServerConnectionHandler.h"
#include "Utility.h"

namespace network {

using ::testing::_;
using ::testing::DoAll;
using ::testing::Return;
using namespace std::placeholders;

class IPV4TCPServerConnectionHandlerTest : public ::testing::Test
{
public:
    tracing::CategorySet<tracing::TraceCategory> m_savedTraceFilter;
    int m_dataCallbackInvokedTimes;

    IPV4TCPServerConnectionHandlerTest()
        : m_savedTraceFilter()
        , m_dataCallbackInvokedTimes()
    {}

    void SetUp() override
    {
        m_savedTraceFilter = tracing::GetDefaultTraceFilter();
        tracing::SetDefaultTraceFilter(tracing::TraceCategory::Message | tracing::TraceCategory::Data | tracing::TraceCategory::All);
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

TEST_F(IPV4TCPServerConnectionHandlerTest, Construct)
{
    testing::SocketAPIMock api;

    EXPECT_CALL(api, Open(SocketFamily::InternetV4, SocketType::Stream, _)).Times(0);
    EXPECT_CALL(api, Close(_)).Times(0);

    IPV4TCPServerConnectionHandler handler(api, std::bind(&IPV4TCPServerConnectionHandlerTest::DataCallback, this, _1, _2));
    EXPECT_EQ(0, m_dataCallbackInvokedTimes);
    EXPECT_FALSE(handler.IsStarted());
}

#if defined(PLATFORM_WINDOWS)
#pragma warning(disable: 4100)
#endif
ACTION_P(SetArg1ToMCValue, value) { *reinterpret_cast<std::uint8_t *>(arg1) = value; return 1; }
#if defined(PLATFORM_WINDOWS)
#pragma warning(default: 4100)
#endif

TEST_F(IPV4TCPServerConnectionHandlerTest, Start)
{
    testing::SocketAPIMock api;

    EXPECT_CALL(api, Open(SocketFamily::InternetV4, SocketType::Stream, _)).Times(1);
    EXPECT_CALL(api, Close(_)).Times(1);
    EXPECT_CALL(api, Receive(0, _, _, 0)).WillOnce(SetArg1ToMCValue(std::uint8_t {0x00})).WillOnce(SetArg1ToMCValue(std::uint8_t {0x00}));
    EXPECT_CALL(api, Send(0, _, _, 0)).WillOnce(Return(1));

    IPV4TCPServerConnectionHandler handler(api, std::bind(&IPV4TCPServerConnectionHandlerTest::DataCallback, this, _1, _2));
    IPV4TCPSocket clientSocket(api);
    IPV4EndPoint clientEndPoint(IPV4Address::LocalHost, TestPort);
    clientSocket.Open();
    EXPECT_TRUE(handler.Start(std::move(clientSocket), clientEndPoint));
    EXPECT_EQ(0, m_dataCallbackInvokedTimes);
    EXPECT_TRUE(handler.IsStarted());
    std::this_thread::sleep_for(std::chrono::seconds(2));
}

TEST_F(IPV4TCPServerConnectionHandlerTest, Stop)
{
    testing::SocketAPIMock api;

    EXPECT_CALL(api, Open(SocketFamily::InternetV4, SocketType::Stream, _)).Times(1);
    EXPECT_CALL(api, Close(_)).Times(1);
    EXPECT_CALL(api, Receive(_, _, _, 0)).WillOnce(SetArg1ToMCValue(std::uint8_t {0x00})).WillOnce(SetArg1ToMCValue(std::uint8_t {0x00}));
    EXPECT_CALL(api, Send(_, _, _, 0)).WillOnce(Return(1));

    IPV4TCPServerConnectionHandler handler(api, std::bind(&IPV4TCPServerConnectionHandlerTest::DataCallback, this, _1, _2));
    IPV4TCPSocket clientSocket(api);
    IPV4EndPoint clientEndPoint(IPV4Address::LocalHost, TestPort);
    clientSocket.Open();
    EXPECT_TRUE(handler.Start(std::move(clientSocket), clientEndPoint));
    EXPECT_EQ(0, m_dataCallbackInvokedTimes);
    EXPECT_TRUE(handler.IsStarted());
    std::this_thread::sleep_for(std::chrono::seconds(2));
    handler.Stop();
    EXPECT_FALSE(handler.IsStarted());
}

} // namespace network
