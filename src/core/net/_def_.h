/**
 * core/net/_def_.h
 *
 * Copyright (c) 2019-2020 Yuriy Lisovskiy
 *
 * Purpose: core/net module's definitions.
 */

// TODO: refactor net/_def_.h

#pragma once

#include "../_def_.h"


// core::net
#define __NET_BEGIN__ __CORE_BEGIN__ namespace net {
#define __NET_END__ } __CORE_END__

// core::net::internal
#define __NET_INTERNAL_BEGIN__ __NET_BEGIN__ namespace internal {
#define __NET_INTERNAL_END__ } __NET_END__


#if defined(_WIN32) || defined(_WIN64)

#include <winsock2.h>
#include <ws2tcpip.h>

//#pragma comment (lib, "Ws2_32.lib")

__NET_INTERNAL_BEGIN__

typedef SOCKET socket_t;
typedef int msg_size_t;

#define WSA_LAST_ERR WSAGetLastError()
#define SOCKET_SEND SD_SEND
#define SOCKET_RECEIVE SD_RECEIVE

//static inline int poll(struct pollfd* pfd, int nfds, int timeout)
//{
//    return WSAPoll(pfd, nfds, timeout);
//}

__NET_INTERNAL_END__

#elif defined(__unix__) || defined(__linux__)

#include <unistd.h>
#include <arpa/inet.h>

__NET_INTERNAL_BEGIN__

typedef int socket_t;
typedef ssize_t msg_size_t;

__NET_INTERNAL_END__

#define INVALID_SOCKET (socket_t)(-1)
#define SOCKET_ERROR (-1)
#define SOCKET_SEND SHUT_RDWR
#define SOCKET_RECEIVE SHUT_RDWR
#define WSA_LAST_ERR errno

#else
#error Library is not supported on this platform

#endif
