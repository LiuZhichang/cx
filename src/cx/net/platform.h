/**
 * @file platform.h
 * @author liuzhichang (lzc3318619633@163.com)
 * @brief 解决平台差异性
 * @version 0.1
 * @date 2022-05-10
 *
 * @copyright Copyright (c) 2022
 *
 */
#pragma once

#include "cx/common/internal.h"

#if defined(CX_PLATFORM_WINDOWS)
#include <WS2tcpip.h>
#include <windows.h>
#include <winsock2.h>
#pragma comment(lib, "ws2_32.lib")
#elif defined(CX_PLATFORM_MAC) || defined(CX_PLATFORM_LINUX)
#include <arpa/inet.h>
#include <ifaddrs.h>
#include <netdb.h>
#include <netinet/tcp.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <sys/un.h>
#include <unistd.h>
#endif

#if defined(CX_PLATFORM_WINDOWS)

typedef SOCKET socket_type;

#elif defined(CX_PLATFORM_LINUX) || defined(CX_PLATFORM_MAC)

typedef int socket_type;

#endif
