#pragma once

#include "network/SocketDefinitions.h"

#include <mutex>

namespace network {

class Socket
{
private:
    SocketHandle m_handle;
    using Mutex = std::recursive_mutex;
    using Lock = std::lock_guard<Mutex>;
    Mutex m_mutex;
#if defined(PLATFORM_WINDOWS)
    bool m_isBlocking;
#endif

public:
    Socket();
	Socket(SocketFamily socketFamily, SocketType socketType);
	Socket(const Socket & other) = delete;
	Socket(Socket && other);
    virtual ~Socket();

	Socket & operator = (const Socket & other) = delete;
	Socket & operator = (Socket && other);

	SocketHandle GetHandle() const;
	void   SetHandle(SocketHandle handle);

	void   Open(SocketFamily socketFamily, SocketType socketType);
	void   Close();
	bool   IsOpen();

    void                      SetSocketOption(SocketOptionLevel level, SocketOption socketOption, const void * optionValue, socklen_t optionLength);
    void                      GetSocketOption(SocketOptionLevel level, SocketOption socketOption, void * optionValue, socklen_t * optionLength);
    void                      SetSocketOption(SocketOption socketOption, const void * optionValue, socklen_t optionLength);
    void                      GetSocketOption(SocketOption socketOption, void * optionValue, socklen_t * optionLength);

    bool                      GetSocketOptionBool(SocketOption socketOption);
    void                      SetSocketOptionBool(SocketOption socketOption, bool value);
    int                       GetSocketOptionInt(SocketOption socketOption);
    void                      SetSocketOptionInt(SocketOption socketOption, int value);

    bool                      GetBroadcastOption();
    void                      SetBroadcastOption(bool value);
    bool                      GetBlockingMode();
    void                      SetBlockingMode(bool value);
    bool                      GetReuseAddress();
    void                      SetReuseAddress(bool value);
    std::chrono::milliseconds GetReceiveTimeout();
    void                      SetReceiveTimeout(std::chrono::milliseconds timeout);
    std::chrono::milliseconds GetSendTimeout();
    void                      SetSendTimeout(std::chrono::milliseconds timeout);
};

} 
// namespace network
