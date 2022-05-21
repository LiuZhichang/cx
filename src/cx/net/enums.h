#pragma once

#include "cx/net/platform.h"

namespace cx::net {

enum class IpProtocol : uint8_t {
  eIp = IPPROTO_IP,
  eIPv4 = IPPROTO_IPV4,
  eIPv6 = IPPROTO_IPV6
};

enum class SocketType : uint8_t { eTcp = SOCK_STREAM, eUdp = SOCK_DGRAM };

enum class AddressFamily : uint8_t {
  eUnSpec = AF_UNSPEC,
  eIPv4 = AF_INET,
  eIPv6 = AF_INET6,
  eUnix = AF_UNIX
};
}  // namespace cx::net