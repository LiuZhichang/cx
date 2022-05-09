#pragma once


#if defined(CX_PLATFORM_WINDOWS)

typedef SOCKET socket_type;

#elif defined(CX_PLATFORM_LINUX) || defined(CX_PLATFORM_MAC)

typedef int socket_type;

#endif

inline void cx_init_socket() {
#if defined(CX_PLATFORM_WINDOWS)
  WSADATA wsaData;
  WSAStartup(MAKEWORD(2, 2), &wsaData);
#endif
}

inline void cx_cleanup_socket() {
#if defined(CX_PLATFORM_WINDOWS)
  WSACleanup();
#endif
}