#include "address.h"

#include <cstdlib>
#include <cstring>
#include <memory>
#include <sstream>
#include <utility>

#include "cx/net/endian.h"

namespace cx::net {

template <class T>
static T GenerateMask(uint32_t bits) {
  return (1 << (sizeof(T) * 8 - bits)) - 1;
}

template <class T>
uint32_t CountBytes(T val) {
  int cnt = 0;
  while (val) {
    val &= (val - 1);
    ++cnt;
  }
  return cnt;
}

AddressFamily Address::family() const {
  return static_cast<AddressFamily>(address()->sa_family);
}

Address::ptr Address::Generate(const sockaddr* address, socklen_t addrlen) {
  if (address == nullptr) {
    return nullptr;
  }

  Address::ptr result;
  switch (address->sa_family) {
    case AF_INET:
      result.reset(new IPv4Address(*(const sockaddr_in*)address));
      break;
    case AF_INET6:
      result.reset(new IPv6Address(*(const sockaddr_in6*)address));
      break;
    default:
      result.reset(new UnkonwAddress(*address));
      break;
  }
  return result;
}

bool Address::Lookup(std::vector<Address::ptr>& result, const std::string& host,
                     AddressFamily family, SocketType type,
                     IpProtocol protocol) {
  addrinfo hints, *rt, *next;
  hints.ai_flags = 0;
  hints.ai_family = static_cast<int>(family);
  hints.ai_socktype = static_cast<int>(type);
  hints.ai_protocol = static_cast<int>(protocol);
  hints.ai_addrlen = 0;
  hints.ai_canonname = NULL;
  hints.ai_addr = NULL;
  hints.ai_next = NULL;

  std::string node;
  const char* service = NULL;

  if (!host.empty() && host[0] == '[') {
    const char* endipv6 =
        (const char*)memchr(host.c_str() + 1, ']', host.size() - 1);
    if (endipv6) {
      if (*(endipv6 + 1) == ':') {
        service = endipv6 + 2;
      }
      node = host.substr(1, endipv6 - host.c_str() - 1);
    }
  }

  if (node.empty()) {
    service = (const char*)memchr(host.c_str(), ':', host.size());
    if (service) {
      if (!memchr(service + 1, ':', host.c_str() + host.size() - service - 1)) {
        node = host.substr(0, service - host.c_str());
        ++service;
      }
    }
  }

  if (node.empty()) {
    node = host;
  }
  int error = getaddrinfo(node.c_str(), service, &hints, &rt);
  if (error) {
    return false;
  }

  next = rt;
  while (next) {
    result.push_back(Generate(next->ai_addr, (socklen_t)next->ai_addrlen));
    next = next->ai_next;
  }

  freeaddrinfo(rt);
  return !result.empty();
}

Address::ptr Address::LookupAny(const std::string& host, AddressFamily family,
                                SocketType type, IpProtocol protocol) {
  std::vector<Address::ptr> result;
  if (Lookup(result, host, family, type, protocol)) {
    return result[0];
  }
  return nullptr;
}

std::shared_ptr<IPAddress> Address::LookupAnyIPAddress(const std::string& host,
                                                       AddressFamily family,
                                                       SocketType type,
                                                       IpProtocol protocol) {
  std::vector<Address::ptr> result;
  if (Lookup(result, host, family, type, protocol)) {
    for (auto& i : result) {
      IPAddress::ptr p = std::dynamic_pointer_cast<IPAddress>(i);
      if (p) {
        return p;
      }
    }
  }
  return nullptr;
}

bool Address::GetInterfaceAddress(
    std::multimap<std::string, std::pair<Address::ptr, uint32_t>>& result,
    AddressFamily family) {
  struct ifaddrs *next, *results;
  if (getifaddrs(&results) != 0) {
    return false;
  }

  try {
    for (next = results; next; next = next->ifa_next) {
      Address::ptr addr;
      uint32_t prefix_len = ~0u;
      if (family != AddressFamily::eUnSpec &&
          (int)family != next->ifa_addr->sa_family) {
        continue;
      }
      switch (next->ifa_addr->sa_family) {
        case AF_INET: {
          addr = Generate(next->ifa_addr, sizeof(sockaddr_in));
          uint32_t netmask = ((sockaddr_in*)next->ifa_netmask)->sin_addr.s_addr;
          prefix_len = CountBytes(netmask);
        } break;
        case AF_INET6: {
          addr = Generate(next->ifa_addr, sizeof(sockaddr_in6));
          in6_addr& netmask = ((sockaddr_in6*)next->ifa_netmask)->sin6_addr;
          for (int i = 0; i < 16; ++i) {
            prefix_len += CountBytes(netmask.s6_addr[i]);
          }
        } break;
        default:
          break;
      }
      if (addr) {
        result.insert(
            std::make_pair(next->ifa_name, std::make_pair(addr, prefix_len)));
      }
    }
  } catch (...) {
    freeifaddrs(results);
    return false;
  }
  freeifaddrs(results);
  return true;
}

bool Address::GetInterfaceAddress(
    std::vector<std::pair<Address::ptr, uint32_t>>& result,
    const std::string& iface, AddressFamily family) {
  if (iface.empty() || iface == "*") {
    if (family == AddressFamily::eIPv4 || family == AddressFamily::eUnSpec) {
      result.push_back(std::make_pair(Address::ptr(new IPv4Address()), 0u));
    }
    if (family == AddressFamily::eIPv6 || family == AddressFamily::eUnSpec) {
      result.push_back(std::make_pair(Address::ptr(new IPv6Address()), 0u));
    }
    return true;
  }

  std::multimap<std::string, std::pair<Address::ptr, uint32_t>> results;
  if (!GetInterfaceAddress(results, family)) {
    return false;
  }

  auto its = results.equal_range(iface);
  while (its.first != its.second) {
    result.push_back(its.first->second);
    ++its.first;
  }
  return true;
}

bool Address::operator<(const Address& rhs) const {
  socklen_t minlen = std::min(address_len(), rhs.address_len());
  int result = memcmp(address(), rhs.address(), minlen);
  if (result < 0) {
    return true;
  } else if (result > 0) {
    return false;
  } else if (address_len() < rhs.address_len()) {
    return true;
  }
  return false;
}

bool Address::operator==(const Address& rhs) const {
  return address_len() == rhs.address_len() &&
         memcmp(address(), rhs.address(), address_len()) == 0;
}

bool Address::operator!=(const Address& rhs) const { return !(*this == rhs); }

IPAddress::ptr IPAddress::Generate(const char* address, uint16_t port) {
  addrinfo hints, *result;
  memset(&hints, 0, sizeof(hints));
  hints.ai_flags = AI_NUMERICHOST;
  hints.ai_family = AF_UNSPEC;

  int error = getaddrinfo(address, NULL, &hints, &result);
  if (error) {
    return nullptr;
  }

  try {
    IPAddress::ptr rt = std::dynamic_pointer_cast<IPAddress>(
        Address::Generate(result->ai_addr, (socklen_t)result->ai_addrlen));
    if (rt) {
      rt->set_port(port);
    }
    freeaddrinfo(result);
    return rt;
  } catch (...) {
    freeaddrinfo(result);
    return nullptr;
  }
}

IPv4Address::IPv4Address(uint32_t address, uint16_t port) {
  memset(&m_addr, 0, sizeof(m_addr));
  m_addr.sin_family = AF_INET;
  m_addr.sin_port = byteswapOnLittleEndian(port);
  m_addr.sin_addr.s_addr = byteswapOnLittleEndian(address);
}

const sockaddr* IPv4Address::address() const { return (sockaddr*)&m_addr; }

socklen_t IPv4Address::address_len() const { return sizeof(m_addr); }

IPv4Address::IPv4Address(const sockaddr_in& addr) { m_addr = addr; }

IPAddress::ptr IPv4Address::broadcast_address(uint32_t prefix_len) {
  if (prefix_len > 32) {
    return nullptr;
  }
  sockaddr_in addr(m_addr);
  addr.sin_addr.s_addr |=
      byteswapOnLittleEndian(GenerateMask<uint32_t>(prefix_len));
  return IPv4Address::ptr(new IPv4Address(addr));
}

IPv4Address::ptr IPv4Address::Generate(const char* address, uint16_t port) {
  IPv4Address::ptr addr(new IPv4Address);
  addr->m_addr.sin_port = byteswapOnLittleEndian(port);
  int res = inet_pton(AF_INET, address, &addr->m_addr.sin_addr);
  if (res <= 0) {
    return nullptr;
  }
  return addr;
}

IPAddress::ptr IPv4Address::network_address(uint32_t prefix_len) {
  if (prefix_len > 32) {
    return nullptr;
  }
  sockaddr_in addr(m_addr);
  addr.sin_addr.s_addr &=
      byteswapOnLittleEndian(GenerateMask<uint32_t>(prefix_len));
  return IPv4Address::ptr(new IPv4Address(addr));
}

IPAddress::ptr IPv4Address::subnet_mask(uint32_t prefix_len) {
  sockaddr_in subnet;
  memset(&subnet, 0, sizeof(subnet));
  subnet.sin_family = AF_INET;
  subnet.sin_addr.s_addr =
      ~byteswapOnLittleEndian(GenerateMask<uint32_t>(prefix_len));
  return IPv4Address::ptr(new IPv4Address(subnet));
}

uint32_t IPv4Address::port() const {
  return byteswapOnLittleEndian(m_addr.sin_port);
}
void IPv4Address::set_port(uint16_t port) {
  m_addr.sin_port = byteswapOnLittleEndian(port);
}

std::string IPv4Address::to_string() {
  uint32_t addr = byteswapOnLittleEndian(m_addr.sin_addr.s_addr);
  std::stringstream ss;
  ss << ((addr >> 24) & 0xff) << "." << ((addr >> 16) & 0xff) << "."
     << ((addr >> 8) & 0xff) << "." << (addr & 0xff) << ":"
     << byteswapOnLittleEndian(m_addr.sin_port);

  return ss.str();
}

IPv6Address::IPv6Address() {
  memset(&m_addr, 0, sizeof(m_addr));
  m_addr.sin6_family = AF_INET6;
}

IPv6Address::IPv6Address(const sockaddr_in6& addr) { m_addr = addr; }

IPv6Address::IPv6Address(const uint8_t address[16], uint16_t port) {
  memset(&m_addr, 0, sizeof(m_addr));
  m_addr.sin6_family = AF_INET6;
  m_addr.sin6_port = byteswapOnLittleEndian(port);
  memcpy(&m_addr.sin6_addr, address, 16);
}

IPv6Address::ptr IPv6Address::Generate(const char* address, uint16_t port) {
  IPv6Address::ptr addr(new IPv6Address);
  addr->m_addr.sin6_port = byteswapOnLittleEndian(port);
  int res = inet_pton(AF_INET6, address, &addr->m_addr.sin6_addr);
  if (res <= 0) {
    return nullptr;
  }
  return addr;
}

const sockaddr* IPv6Address::address() const { return (sockaddr*)&m_addr; }

socklen_t IPv6Address::address_len() const { return sizeof(m_addr); }

IPAddress::ptr IPv6Address::broadcast_address(uint32_t prefix_len) {
  sockaddr_in6 addr(m_addr);
  addr.sin6_addr.s6_addr[prefix_len / 8] |=
      GenerateMask<uint8_t>(prefix_len % 8);
  for (int i = prefix_len / 8 + 1; i < 16; ++i) {
    addr.sin6_addr.s6_addr[i] = 0xff;
  }
  return IPv6Address::ptr(new IPv6Address(addr));
}

IPAddress::ptr IPv6Address::network_address(uint32_t prefix_len) {
  sockaddr_in6 addr(m_addr);
  addr.sin6_addr.s6_addr[prefix_len / 8] &=
      GenerateMask<uint8_t>(prefix_len % 8);
  for (int i = prefix_len / 8 + 1; i < 16; ++i) {
    addr.sin6_addr.s6_addr[i] = 0x00;
  }
  return IPv6Address::ptr(new IPv6Address(addr));
}

IPAddress::ptr IPv6Address::subnet_mask(uint32_t prefix_len) {
  sockaddr_in6 subnet;
  memset(&subnet, 0, sizeof(subnet));
  subnet.sin6_family = AF_INET;
  subnet.sin6_addr.s6_addr[prefix_len / 8] =
      ~GenerateMask<uint8_t>(prefix_len % 8);

  for (int i = 0; i < prefix_len / 8; ++i) {
    subnet.sin6_addr.s6_addr[i] = 0xff;
  }
  return IPv6Address::ptr(new IPv6Address(subnet));
}

uint32_t IPv6Address::port() const {
  return byteswapOnLittleEndian(m_addr.sin6_port);
}

void IPv6Address::set_port(uint16_t port) {
  m_addr.sin6_port = byteswapOnLittleEndian(port);
}

std::string IPv6Address::to_string() {
  std::stringstream ss;
  ss << "[";
  uint16_t* addr = (uint16_t*)m_addr.sin6_addr.s6_addr;
  bool used_zeros = false;
  for (size_t i = 0; i < 8; ++i) {
    if (addr[i] == 0 && !used_zeros) {
      continue;
    }
    if (i && addr[i - 1] == 0 && !used_zeros) {
      ss << ":";
      used_zeros = true;
    }
    if (i) {
      ss << ":";
    }
    ss << std::hex << (int)byteswapOnLittleEndian(addr[i]) << std::dec;
  }

  if (!used_zeros && addr[7] == 0) {
    ss << "::";
  }

  ss << "]:" << byteswapOnLittleEndian(m_addr.sin6_port);

  return ss.str();
}

static const size_t MAX_PATH_LEN = (sizeof((sockaddr_un*)0)->sun_path) - 1;

UnixAddress::UnixAddress() {
  memset(&m_addr, 0, sizeof(m_addr));
  m_addr.sun_family = AF_UNIX;
  m_len = offsetof(sockaddr_un, sun_path) + MAX_PATH_LEN;
}

UnixAddress::UnixAddress(const std::string& path) {
  memset(&m_addr, 0, sizeof(m_addr));
  m_addr.sun_family = AF_UNIX;
  m_len = path.size() + 1;

  if (!path.empty() && path[0] == '\0') {
    --m_len;
  }

  if (m_len <= sizeof(m_addr.sun_path)) {
    throw std::logic_error("path too long");
  }
  memcpy(&m_addr.sun_path, path.c_str(), m_len);
  m_len += offsetof(sockaddr_un, sun_path);
}

const sockaddr* UnixAddress::address() const { return (sockaddr*)&m_addr; }

socklen_t UnixAddress::address_len() const { return m_len; }

void UnixAddress::set_address_len(uint32_t len) { m_len = len; }

std::string UnixAddress::to_string() {
  std::stringstream ss;
  if (m_len > offsetof(sockaddr_un, sun_path) && m_addr.sun_path[0] == '\0') {
    ss << "\\0"
       << std::string(m_addr.sun_path + 1,
                      m_len - offsetof(sockaddr_un, sun_path) - 1);

    return ss.str();
  }

  ss << m_addr.sun_path;

  return ss.str();
}

UnkonwAddress::UnkonwAddress(AddressFamily family) {
  memset(&m_addr, 0, sizeof(m_addr));
  m_addr.sa_family = static_cast<int>(family);
}

UnkonwAddress::UnkonwAddress(const sockaddr& addr) { m_addr = addr; }

const sockaddr* UnkonwAddress::address() const { return &m_addr; }

socklen_t UnkonwAddress::address_len() const { return sizeof(m_addr); }

std::string UnkonwAddress::to_string() {
  std::stringstream ss;
  ss << "[UnknownAddress family=" << m_addr.sa_family << "]";

  return ss.str();
}

}  // namespace cx::net
