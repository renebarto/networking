#include "network-osal/SocketAPI.h"

#include "gmock/gmock.h"

namespace network {
namespace testing {

using ::testing::_;
using ::testing::NiceMock;
using ::testing::NaggyMock;

class SocketAPIMock
    : public ISocketAPI
{
public:
    MOCK_METHOD(SocketHandle, Open, (SocketFamily socketFamily, SocketType socketType, SocketProtocol protocol), (override));
    MOCK_METHOD(int, Close, (SocketHandle handle), (override));
    MOCK_METHOD(int, SetSocketOption, (SocketHandle handle, SocketOptionLevel level, SocketOption socketOption, const void * optionValue, socklen_t optionLength), (override));
    MOCK_METHOD(int, GetSocketOption, (SocketHandle handle, SocketOptionLevel level, SocketOption socketOption, void * optionValue, socklen_t * optionLength), (override));
    MOCK_METHOD(int, SetBlockingMode, (SocketHandle handle, bool value), (override));
    MOCK_METHOD(int, GetBlockingMode, (SocketHandle handle, bool & value), (override));
    MOCK_METHOD(int, Bind, (SocketHandle handle, const sockaddr * address, socklen_t addressLength), (override));
    MOCK_METHOD(int, Connect, (SocketHandle handle, sockaddr const * serverAddress, socklen_t serverAddressLength), (override));
    MOCK_METHOD(int, Listen, (SocketHandle handle, int numListeners), (override));
    MOCK_METHOD(int, Accept, (SocketHandle handle, sockaddr * clientAddress, socklen_t * clientAddressLength), (override));
    MOCK_METHOD(int, GetLocalAddress, (SocketHandle handle, sockaddr * address, socklen_t * addressLength), (override));
    MOCK_METHOD(int, GetRemoteAddress, (SocketHandle handle, sockaddr * address, socklen_t * addressLength), (override));
    MOCK_METHOD(int, Receive, (SocketHandle handle, std::uint8_t * data, std::size_t bufferSize, int flags), (override));
    MOCK_METHOD(int, Send, (SocketHandle handle, const std::uint8_t * data, std::size_t bytesToSend, int flags), (override));
    MOCK_METHOD(int, ReceiveFrom, (SocketHandle handle, std::uint8_t * data, std::size_t bufferSize, int flags, sockaddr * address, socklen_t * addressLength), (override));
    MOCK_METHOD(int, SendTo, (SocketHandle handle, const std::uint8_t *data, std::size_t bytesToSend, int flags, const sockaddr * address, socklen_t addressLength), (override));
};

} // namespace testing
} // namespace network
