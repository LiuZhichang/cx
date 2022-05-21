#pragma once

#include <iostream>
#include <map>
#include <memory>
#include <string>
#include <vector>

#include "cx/common/internal.h"
#include "cx/net/enums.h"

namespace cx::net {

class IPAddress;

/**
 * @brief 网络地址基类
 */
class Address {
 public:
  typedef std::shared_ptr<Address> ptr;

  /**
   * @brief 析构函数
   */
  virtual ~Address() {}

  /**
   * @brief 获取协议簇
   *
   * @return 网络地址对应的协议簇
   */
  AddressFamily family() const;

  /**
   * @brief 创建Address
   *
   * @param[in] address sockaddr指针
   * @param[in] addrlen sockaddr的长度
   *
   * @return 返回与sockaddr相匹配的Address,失败返回nullptr
   */
  CX_STATIC Address::ptr Generate(const sockaddr* address, socklen_t addrlen);

  /**
   * @brief 通过host地址返回对应的所有Address
   *
   * @param[in] result   结果集
   * @param[in] host	   域名,服务器名称等
   * @param[in] family   协议簇
   * @param[in] type	   socket类型
   * @param[in] protocol 协议
   *
   * @return 是否转换成功
   */
  CX_STATIC bool Lookup(std::vector<Address::ptr>& result,
                        const std::string& host,
                        AddressFamily family = AddressFamily::eUnSpec,
                        SocketType type = SocketType::eTcp,
                        IpProtocol protocol = IpProtocol::eIp);

  /**
   * @brief 通过host地址返回对应的任意一个Address
   *
   * @param[in] host		域名,服务器名称等
   * @param[in] family	协议簇
   * @param[in] type		socket类型
   * @param[in] protocol	协议
   *
   * @return 返回满足条件的任意Address,否则返回false
   */
  CX_STATIC Address::ptr LookupAny(
      const std::string& host, AddressFamily family = AddressFamily::eUnSpec,
      SocketType type = SocketType::eTcp,
      IpProtocol protocol = IpProtocol::eIp);

  /**
   * @brief 通过host地址返回对应的任意一个Address
   *
   * @param[in] host		域名,服务器名称等
   * @param[in] family	协议簇
   * @param[in] type		socket类型
   * @param[in] protocol	协议
   *
   * @return 返回满足条件的任意Address,否则返回false
   */
  CX_STATIC std::shared_ptr<IPAddress> LookupAnyIPAddress(
      const std::string& host, AddressFamily family = AddressFamily::eUnSpec,
      SocketType type = SocketType::eTcp,
      IpProtocol protocol = IpProtocol::eIp);

  /**
   * @brief 返回本机所有网卡的[网卡名,地址,子网掩码位数]
   *
   * @param[in] result	结果集
   * @param[in] family	协议簇
   *
   * @return 是否获取成功
   */
  CX_STATIC bool GetInterfaceAddress(
      std::multimap<std::string, std::pair<Address::ptr, uint32_t>>& result,
      AddressFamily family = AddressFamily::eUnSpec);

  /**
   * @brief 获取指定网卡的地址和子网掩码位数
   *
   * @param[in] result	结果集
   * @param[in] iface		网卡名称
   * @param[in] family	协议簇
   *
   * @return 是否获取成功
   */
  CX_STATIC bool GetInterfaceAddress(
      std::vector<std::pair<Address::ptr, uint32_t>>& result,
      const std::string& iface, AddressFamily family = AddressFamily::eUnSpec);

  /**
   * @brief 获取sockaddr指针
   *
   * @return
   */
  virtual const sockaddr* address() const = 0;

  /**
   * @brief 获取sockaddr的长度
   *
   * @return
   */
  virtual socklen_t address_len() const = 0;

  virtual std::string to_string() = 0;

  bool operator<(const Address& rhs) const;
  bool operator==(const Address& rhs) const;
  bool operator!=(const Address& rhs) const;
};

/**
 * @brief IP地址
 */
class IPAddress : public Address {
 public:
  typedef std::shared_ptr<IPAddress> ptr;

  /**
   * @brief 通过域名,IP,服务器名称创建Address
   *
   * @param[in] address	域名,IP,服务器名称
   * @param[in] port		端口号
   *
   * @return 成功返回IPAddress，失败返回nullptr
   */
  CX_STATIC IPAddress::ptr Generate(const char* address, uint16_t port = 0);

  /**
   * @brief 获取广播地址
   *
   * @param[in] prefix_len	子网掩码位数
   *
   * @return	成功返回IPAddress，失败返回nullptr
   */
  virtual IPAddress::ptr broadcast_address(uint32_t prefix_len) = 0;

  /**
   * @brief 获取网段
   *
   * @param[in] prefix_len	子网掩码位数
   *
   * @return 成功返回IPAddress，失败返回nullptr
   */
  virtual IPAddress::ptr network_address(uint32_t prefix_len) = 0;

  /**
   * @brief 获取子网掩码地址
   *
   * @param[in] prefix_len	子网掩慢位数
   *
   * @return 成功返回IPAddress，失败返回nullptr
   */
  virtual IPAddress::ptr subnet_mask(uint32_t prefix_len) = 0;

  /**
   * @brief 获取端口号
   *
   * @return 端口号
   */
  virtual uint32_t port() const = 0;

  /**
   * @brief 设置端口号
   *
   * @param[in] port 端口号
   */
  virtual void set_port(uint16_t port) = 0;
};

/**
 * @brief IPv4地址
 */
class IPv4Address : public IPAddress {
 public:
  typedef std::shared_ptr<IPv4Address> ptr;

  /**
   * @brief 构造函数
   *
   * @param[in] addr sockaddr_in结构体
   */
  IPv4Address(const sockaddr_in& addr);

  /**
   * @brief 通过二进制地址构造IPv4Address
   *
   * @param[in] address	二进制地址
   * @param[in] port		端口号
   */
  IPv4Address(uint32_t address = INADDR_ANY, uint16_t port = 0);

  /**
   * @brief 使用点分十进制地址创建IPv4Address
   *
   * @param[in] address	点分十进制地址
   * @param[in] port		端口号
   *
   * @return 成功返回IPv4Address,失败返回nullptr
   */
  CX_STATIC ptr Generate(const char* address, uint16_t port = 0);

  const sockaddr* address() const override;
  socklen_t address_len() const override;

  IPAddress::ptr broadcast_address(uint32_t prefix_len) override;
  IPAddress::ptr network_address(uint32_t prefix_len) override;
  IPAddress::ptr subnet_mask(uint32_t prefix_len) override;

  uint32_t port() const override;
  void set_port(uint16_t port) override;

  std::string to_string() override;

 private:
  sockaddr_in m_addr;
};

/**
 * @brief IPv6地址
 */
class IPv6Address : public IPAddress {
 public:
  typedef std::shared_ptr<IPv6Address> ptr;

  /**
   * @brief 无参构造
   */
  IPv6Address();

  /**
   * @brief 通过sockaddr_in6串构造IPv6Address
   *
   * @param[in] addr sockaddr_in6结构体
   */
  IPv6Address(const sockaddr_in6& addr);

  /**
   * @brief 通过IPv6二进制地址构造IPv6Address
   *
   * @param[in] address	二进制地址
   * @param[in] port		端口号
   */
  IPv6Address(const uint8_t address[16], uint16_t port = 0);

  /**
   * @brief 通过IPv6地址字符串构造IPv6Address
   *
   * @param[in] address	地址字符串
   * @param[in] port		端口号
   *
   * @return 成功返回IP6Address，失败返回nullptr
   */
  CX_STATIC IPv6Address::ptr Generate(const char* address, uint16_t port = 0);

  const sockaddr* address() const override;
  socklen_t address_len() const override;

  IPAddress::ptr broadcast_address(uint32_t prefix_len) override;
  IPAddress::ptr network_address(uint32_t prefix_len) override;
  IPAddress::ptr subnet_mask(uint32_t prefix_len) override;

  uint32_t port() const override;
  void set_port(uint16_t port) override;

  std::string to_string() override;

 private:
  sockaddr_in6 m_addr;
};

/**
 * @brief UunixSocket地址
 */
class UnixAddress : public Address {
 public:
  typedef std::shared_ptr<UnixAddress> ptr;

  /**
   * @brief 无参构造
   */
  UnixAddress();

  /**
   * @brief 通过路径构造UnixAddress
   *
   * @param[in] path 路径
   */
  UnixAddress(const std::string& path);

  const sockaddr* address() const override;
  socklen_t address_len() const override;

  void set_address_len(uint32_t len);

  std::string to_string() override;

 private:
  sockaddr_un m_addr;
  socklen_t m_len;
};

class UnkonwAddress : public Address {
 public:
  typedef std::shared_ptr<UnkonwAddress> ptr;

  UnkonwAddress(AddressFamily family);
  UnkonwAddress(const sockaddr& addr);

  const sockaddr* address() const override;
  socklen_t address_len() const override;

  std::string to_string() override;

 private:
  sockaddr m_addr;
};

}  // namespace cx::net
