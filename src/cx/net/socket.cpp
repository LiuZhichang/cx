#include "socket.h"

#include <cstring>

#include "cx/net/address.h"

#if defined(CX_PLATFORM_WINDOWS)
struct __SockIniter {
  __SockIniter() {
    WSADATA wsaData;
    WSAStartup(MAKEWORD(2, 2), &wsaData);
  }

  ~__SockIniter() { WSACleanup(); }
};
static __SockIniter __sock_initer;
#endif

//---------------------------------------------------------------- namespace
// cx::net
namespace cx::net {

Socket::ptr Socket::GenerateTCP(Address::ptr address) {
  Socket::ptr sock(new Socket(address->family(), SocketType::eTcp));
  return sock;
}

Socket::ptr Socket::GenerateUDP(Address::ptr address) {
  Socket::ptr sock(new Socket(address->family(), SocketType::eUdp));
  return sock;
}

Socket::ptr Socket::GenerateTCPSocket() {
  Socket::ptr sock(new Socket(AddressFamily::eIPv4, SocketType::eTcp));
  return sock;
}

Socket::ptr Socket::GenerateUDPSocket() {
  Socket::ptr sock(new Socket(AddressFamily::eIPv4, SocketType::eUdp));
  return sock;
}

Socket::ptr Socket::GenerateTCPSocket6() {
  Socket::ptr sock(new Socket(AddressFamily::eIPv6, SocketType::eTcp));
  return sock;
}

Socket::ptr Socket::GenerateUDPSocket6() {
  Socket::ptr sock(new Socket(AddressFamily::eIPv6, SocketType::eUdp));
  return sock;
}
Socket::ptr Socket::GenerateUnixTCPSocket() {
  Socket::ptr sock(new Socket(AddressFamily::eUnix, SocketType::eTcp));
  return sock;
}
Socket::ptr Socket::GenerateUnixUDPSocket() {
  Socket::ptr sock(new Socket(AddressFamily::eUnix, SocketType::eUdp));
  return sock;
}

Socket::Socket(AddressFamily family, SocketType type, IpProtocol protocol)
    : m_sock(-1),
      m_family(family),
      m_type(type),
      m_protocol(protocol),
      m_is_connected(false) {}

Socket::~Socket() { close(); }

int64_t Socket::get_send_timeout() {
  struct timeval tv;
  get_option(SOL_SOCKET, SO_SNDTIMEO, tv);

  return (tv.tv_sec * 1000 + (tv.tv_usec % 1000));
}

void Socket::set_send_timeout(uint64_t time) {
  struct timeval tv {
    int(time / 1000), int(time % 1000 * 1000)
  };
  set_option(SOL_SOCKET, SO_SNDTIMEO, tv);
}

int64_t Socket::get_recv_timeout() {
  struct timeval tv;
  get_option(SOL_SOCKET, SO_RCVTIMEO, tv);

  return (tv.tv_sec * 1000 + (tv.tv_usec % 1000));
}

void Socket::set_recv_timeout(uint64_t time) {
  struct timeval tv {
    int(time / 1000), int(time % 1000 * 1000)
  };
  set_option(SOL_SOCKET, SO_RCVTIMEO, tv);
}

bool Socket::get_option(int level, int opt, void* result, socklen_t* len) {
  int rt = getsockopt(m_sock, level, opt, result, (socklen_t*)len);
  if (rt) {
    return false;
  }
  return true;
}

bool Socket::set_option(int level, int opt, const void* result, socklen_t len) {
  int rt = setsockopt(m_sock, level, opt, result, (socklen_t)len);
  if (rt) {
    return false;
  }
  return true;
}

Socket::ptr Socket::accept() {
  Socket::ptr sock(new Socket(m_family, m_type, m_protocol));
  int newsock = ::accept(m_sock, nullptr, nullptr);
  if (newsock == -1) {
    return nullptr;
  }
  if (sock->init(newsock)) {
    return sock;
  }
  return nullptr;
}

bool Socket::bind(const Address::ptr addr) {
  if (!is_valid()) {
    new_sock();
    if (!is_valid()) {
      return false;
    }
  }
  if (addr->family() != m_family) {
    return false;
  }

  if (::bind(m_sock, addr->address(), addr->address_len())) {
    return false;
  }
  local_address();
  return true;
}

bool Socket::connect(const Address::ptr addr, uint64_t timeout_ms) {
  m_remote_address = addr;
  if (!is_valid()) {
    new_sock();
    if (!is_valid()) {
      return false;
    }
  }
  if (addr->family() != m_family) {
    return false;
  }

  if (timeout_ms == (uint64_t)-1) {
    if (::connect(m_sock, addr->address(), addr->address_len())) {
      close();
      return false;
    }
  }
  m_is_connected = true;
  remote_address();
  local_address();
  return true;
}

bool Socket::listen(int backlog) {
  if (!is_valid()) {
    return false;
  }
  if (::listen(m_sock, backlog)) {
    return false;
  }
  return true;
}

bool Socket::close() {
  if (!m_is_connected && m_sock == -1) {
    return true;
  }

  m_is_connected = false;
  if (m_sock != -1) {
    ::close(m_sock);
    m_sock = -1;
  }
  return false;
}

int Socket::send(const void* buffer, size_t len, int flags) {
  if (is_connected()) {
    return ::send(m_sock, buffer, len, flags);
  }
  return -1;
}

int Socket::send(const iovec* buffers, size_t len, int flags) {
  if (is_connected()) {
    msghdr msg;
    memset(&msg, 0, sizeof(msg));
    msg.msg_iov = (iovec*)buffers;
    msg.msg_iovlen = len;
    return ::sendmsg(m_sock, &msg, flags);
  }
  return -1;
}

int Socket::send_to(const void* buffer, size_t len, const Address::ptr to,
                    int flags) {
  if (is_connected()) {
    return ::sendto(m_sock, buffer, len, flags, to->address(),
                    to->address_len());
  }
  return -1;
}

int Socket::send_to(const iovec* buffers, size_t len, const Address::ptr to,
                    int flags) {
  if (is_connected()) {
    msghdr msg;
    memset(&msg, 0, sizeof(msg));
    msg.msg_iov = (iovec*)buffers;
    msg.msg_iovlen = len;
    msg.msg_name = (void*)to->address();
    msg.msg_namelen = to->address_len();
    return ::sendmsg(m_sock, &msg, flags);
  }
  return -1;
}

int Socket::recv(void* buffer, size_t len, int flags) {
  if (is_connected()) {
    return ::recv(m_sock, buffer, len, flags);
  }
  return -1;
}

int Socket::recv(iovec* buffers, size_t len, int flags) {
  if (is_connected()) {
    msghdr msg;
    memset(&msg, 0, sizeof(msg));
    msg.msg_iov = (iovec*)buffers;
    msg.msg_iovlen = len;
    return ::recvmsg(m_sock, &msg, flags);
  }
  return -1;
}

int Socket::recv_from(void* buffer, size_t len, Address::ptr from, int flags) {
  if (is_connected()) {
    socklen_t len = from->address_len();
    return ::recvfrom(m_sock, buffer, len, flags,
                      const_cast<sockaddr*>(from->address()), &len);
  }
  return -1;
}

int Socket::recv_from(iovec* buffers, size_t len, Address::ptr from,
                      int flags) {
  if (is_connected()) {
    msghdr msg;
    memset(&msg, 0, sizeof(msg));
    msg.msg_iov = (iovec*)buffers;
    msg.msg_iovlen = len;
    msg.msg_name = (sockaddr*)from->address();
    msg.msg_namelen = from->address_len();
    return ::recvmsg(m_sock, &msg, flags);
  }
  return -1;
}

Address::ptr Socket::remote_address() {
  if (m_remote_address) {
    return m_remote_address;
  }

  Address::ptr result;
  switch (m_family) {
    case AddressFamily::eIPv4:
      result.reset(new IPv4Address);
      break;
    case AddressFamily::eIPv6:
      result.reset(new IPv6Address);
      break;
    case AddressFamily::eUnix:
      result.reset(new UnixAddress);
      break;
    default:
      result.reset(new UnkonwAddress(m_family));
      break;
  }

  socklen_t addrlen = result->address_len();
  if (getpeername(m_sock, const_cast<sockaddr*>(result->address()), &addrlen)) {
    return Address::ptr(new UnkonwAddress(m_family));
  }
  if (m_family == AddressFamily::eUnix) {
    UnixAddress::ptr addr = std::dynamic_pointer_cast<UnixAddress>(result);
    addr->set_address_len(addrlen);
  }
  m_remote_address = result;
  return m_remote_address;
}

Address::ptr Socket::local_address() {
  if (m_local_address) {
    return m_local_address;
  }

  Address::ptr result;
  switch (m_family) {
    case AddressFamily::eIPv4:
      result.reset(new IPv4Address);
      break;
    case AddressFamily::eIPv6:
      result.reset(new IPv6Address);
      break;
    case AddressFamily::eUnix:
      result.reset(new UnixAddress);
      break;
    default:
      result.reset(new UnkonwAddress(m_family));
      break;
  }

  socklen_t addrlen = result->address_len();
  if (getsockname(m_sock, const_cast<sockaddr*>(result->address()), &addrlen)) {
    return Address::ptr(new UnkonwAddress(m_family));
  }
  if (m_family == AddressFamily::eUnix) {
    UnixAddress::ptr addr = std::dynamic_pointer_cast<UnixAddress>(result);
    addr->set_address_len(addrlen);
  }
  m_local_address = result;
  return m_local_address;
}

bool Socket::is_valid() const { return m_sock != -1; }

int Socket::get_error() {
  int error = 0;
  socklen_t len = sizeof(error);
  if (!get_option(SOL_SOCKET, SO_ERROR, &error, &len)) {
    error = errno;
  }
  return error;
}

void Socket::init_sock() {
  int val = 1;
  set_option(SOL_SOCKET, SO_REUSEADDR, val);
  if (m_type == SocketType::eTcp) {
    set_option(IPPROTO_TCP, TCP_NODELAY, val);
  }
}

void Socket::new_sock() {
  m_sock = ::socket(static_cast<int>(m_family), static_cast<int>(m_type),
                    static_cast<int>(m_protocol));
  if (m_sock != -1) {
    init_sock();
  }
}

bool Socket::init(socket_type sock) {
  if (!sock) return false;

  m_sock = sock;
  m_is_connected = true;
  init_sock();
  local_address();
  remote_address();
  return true;
}

}  // namespace cx::net
