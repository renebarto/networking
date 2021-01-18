#pragma once

#include "network-osal/SocketDefinitions.h"

#if defined(PLATFORM_LINUX)
#include <sys/socket.h>
#endif

namespace network {

class ISocketAPI
{
public:
    virtual ~ISocketAPI() {}

    virtual SocketHandle Open(SocketFamily socketFamily, SocketType socketType, SocketProtocol protocol) = 0;
    virtual int Close(SocketHandle handle) = 0;
    virtual int SetSocketOption(SocketHandle handle, SocketOptionLevel level, SocketOption socketOption, const void * optionValue, socklen_t optionLength) = 0;
    virtual int GetSocketOption(SocketHandle handle, SocketOptionLevel level, SocketOption socketOption, void * optionValue, socklen_t * optionLength) = 0;
    virtual int SetBlockingMode(SocketHandle handle, bool value) = 0;
    virtual int GetBlockingMode(SocketHandle handle, bool & value) = 0;
    virtual int Bind(SocketHandle handle, const sockaddr * address, socklen_t addressLength) = 0;
    virtual int Connect(SocketHandle handle, sockaddr const * serverAddress, socklen_t serverAddressLength) = 0;
    virtual int Listen(SocketHandle handle, int numListeners) = 0;
    virtual int Accept(SocketHandle handle, sockaddr * clientAddress, socklen_t * clientAddressLength) = 0;
    virtual int GetLocalAddress(SocketHandle handle, sockaddr * address, socklen_t * addressLength) = 0;
    virtual int GetRemoteAddress(SocketHandle handle, sockaddr * address, socklen_t * addressLength) = 0;
    virtual int Receive(SocketHandle handle, std::uint8_t * data, std::size_t bufferSize, int flags) = 0;
    virtual int Send(SocketHandle handle, const std::uint8_t * data, std::size_t bytesToSend, int flags) = 0;
    virtual int ReceiveFrom(SocketHandle handle, std::uint8_t * data, std::size_t bufferSize, int flags, sockaddr * address, socklen_t * addressLength) = 0;
    virtual int SendTo(SocketHandle handle, const std::uint8_t *data, std::size_t bytesToSend, int flags, const sockaddr * address, socklen_t addressLength) = 0;
};

class SocketAPI
    : public ISocketAPI
{
public:
    SocketAPI();
    ~SocketAPI();

    SocketHandle Open(SocketFamily socketFamily, SocketType socketType, SocketProtocol protocol) override;
    int Close(SocketHandle handle) override;
    int SetSocketOption(SocketHandle handle, SocketOptionLevel level, SocketOption socketOption, const void * optionValue, socklen_t optionLength) override;
    int GetSocketOption(SocketHandle handle, SocketOptionLevel level, SocketOption socketOption, void * optionValue, socklen_t * optionLength) override;
    int SetBlockingMode(SocketHandle handle, bool value) override;
    int GetBlockingMode(SocketHandle handle, bool & value) override;
    int Bind(SocketHandle handle, const sockaddr * address, socklen_t addressLength) override;
    int Connect(SocketHandle handle, sockaddr const * serverAddress, socklen_t serverAddressLength) override;
    int Listen(SocketHandle handle, int numListeners) override;
    int Accept(SocketHandle handle, sockaddr * clientAddress, socklen_t * clientAddressLength) override;
    int GetLocalAddress(SocketHandle handle, sockaddr * address, socklen_t * addressLength) override;
    int GetRemoteAddress(SocketHandle handle, sockaddr * address, socklen_t * addressLength) override;
    int Receive(SocketHandle handle, std::uint8_t * data, std::size_t bufferSize, int flags) override;
    int Send(SocketHandle handle, const std::uint8_t * data, std::size_t bytesToSend, int flags) override;
    int ReceiveFrom(SocketHandle handle, std::uint8_t * data, std::size_t bufferSize, int flags, sockaddr * address, socklen_t * addressLength) override;
    int SendTo(SocketHandle handle, const std::uint8_t *data, std::size_t bytesToSend, int flags, const sockaddr * address, socklen_t addressLength) override;
};

} // namespace network
