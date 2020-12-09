#pragma once

#if defined(PLATFORM_LINUX)

#include <strings.h>
#include <netdb.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <net/if.h>
#include <ifaddrs.h>
#include <sys/ioctl.h>
#include <unistd.h>

#elif defined(PLATFORM_WINDOWS)

#pragma warning(disable: 5039)
#include <winsock2.h>
#include <ws2tcpip.h>
#pragma warning(default: 5039)

// int inet_aton(const char *cp, struct in_addr *inp);

// in_addr_t inet_addr(const char *cp);

// in_addr_t inet_network(const char *cp);

// char *inet_ntoa(struct in_addr in);

// struct in_addr inet_makeaddr(int net, int host);

// in_addr_t inet_lnaof(struct in_addr in);

// in_addr_t inet_netof(struct in_addr in);

#else

#error Unsupported platform

#endif

