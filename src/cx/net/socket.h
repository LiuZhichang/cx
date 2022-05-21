#pragma once

#include <memory>

#include "cx/common/internal.h"
#include "cx/common/noncopyable.h"
#include "cx/net/address.h"
#include "cx/net/enums.h"
namespace cx::net {

class Socket : public std::enable_shared_from_this<Socket>, Noncopyable {
 public:
  typedef std::shared_ptr<Socket> ptr;
  typedef std::weak_ptr<Socket> weak_ptr;

  CX_STATIC Socket::ptr GenerateTCP(Address::ptr address);
  CX_STATIC Socket::ptr GenerateUDP(Address::ptr address);

  CX_STATIC Socket::ptr GenerateTCPSocket();
  CX_STATIC Socket::ptr GenerateUDPSocket();

  CX_STATIC Socket::ptr GenerateTCPSocket6();
  CX_STATIC Socket::ptr GenerateUDPSocket6();

  CX_STATIC Socket::ptr GenerateUnixTCPSocket();
  CX_STATIC Socket::ptr GenerateUnixUDPSocket();

  Socket(AddressFamily family, SocketType type,
         IpProtocol protocol = IpProtocol::eIp);
  virtual ~Socket();

  int64_t get_send_timeout();
  void set_send_timeout(uint64_t time);

  int64_t get_recv_timeout();
  void set_recv_timeout(uint64_t time);

  bool get_option(int level, int opt, void* result, socklen_t* len);
  template <typename T>
  bool get_option(int level, int opt, T& result) {
    socklen_t len = sizeof(T);
    return get_option(level, opt, &result, &len);
  }

  bool set_option(int level, int opt, const void* result, socklen_t len);
  template <typename T>
  bool set_option(int level, int opt, const T& value) {
    return set_option(level, opt, &value, sizeof(T));
  }

  virtual Socket::ptr accept();
  virtual bool bind(const Address::ptr addr);
  virtual bool connect(const Address::ptr addr, uint64_t timeout_ms = -1);
  virtual bool listen(int backlog = SOMAXCONN);
  virtual bool close();

  virtual int send(const void* buffer, size_t len, int flags = 0);
  virtual int send(const iovec* buffers, size_t len, int flags = 0);
  virtual int send_to(const void* buffer, size_t len, const Address::ptr to,
                      int flags = 0);
  virtual int send_to(const iovec* buffers, size_t len, const Address::ptr to,
                      int flags = 0);

  virtual int recv(void* buffer, size_t len, int flags = 0);
  virtual int recv(iovec* buffers, size_t len, int flags = 0);
  virtual int recv_from(void* buffer, size_t len, Address::ptr from,
                        int flags = 0);
  virtual int recv_from(iovec* buffers, size_t len, Address::ptr from,
                        int flags = 0);

  Address::ptr remote_address();
  Address::ptr local_address();

  AddressFamily family() const { return m_family; };

  SocketType type() const { return m_type; };

  IpProtocol protocol() const { return m_protocol; };

  socket_type socket() const { return m_sock; };

  bool is_connected() const { return m_is_connected; };
  bool is_valid() const;

  int get_error();

 private:
  void init_sock();
  void new_sock();
  bool init(socket_type sock);

 private:
  socket_type m_sock;
  AddressFamily m_family;
  SocketType m_type;
  IpProtocol m_protocol;
  bool m_is_connected;

  Address::ptr m_local_address;
  Address::ptr m_remote_address;
};

}  // namespace cx::net
