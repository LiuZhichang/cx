#include "cx/common/logger.h"
#include "cx/net/socket.h"

using namespace cx::net;
using namespace cx::log;

class TcpServer {
 public:
  typedef std::shared_ptr<TcpServer> ptr;

  TcpServer() = default;
  ~TcpServer() { m_sock->close(); }

  bool bind(Address::ptr address) {
    m_sock = Socket::GenerateTCP(address);
    if (!m_sock->bind(address)) {
      LOG_ERROR(Loggers::engine) << "bind failed: " << address->to_string();
      return false;
    }

    if (!m_sock->listen()) {
      LOG_ERROR(Loggers::engine) << "listen failed: " << address->to_string();
      return false;
    }

    LOG_INFO(Loggers::engine)
        << "bind successful, address: " << address->to_string();

    return true;
  }

  bool start() {
    LOG_INFO(Loggers::engine) << "server started";
    if (!m_is_running || !m_sock) {
      return false;
    }

    m_sock->listen();
    while (m_is_running) {
      Socket::ptr client = m_sock->accept();
      LOG_INFO(Loggers::engine)
          << "new client connect:" << client->local_address()->to_string();
      std::string buf;
      buf.resize(64);
      int ret = client->recv(buf.data(), buf.size());
      LOG_INFO(Loggers::engine)
          << "recv msgm len:" << ret << ", content:" << buf;
      if (ret) {
        client->send(buf.data(), buf.size());
      }
    }
  }
  bool stop() {
    if (m_is_running) {
      m_is_running = false;
    }
  }

 private:
  Socket::ptr m_client;
  Socket::ptr m_sock;
  bool m_is_running = true;
};

IPAddress::ptr test_address() {
  IPAddress::ptr address = Address::LookupAnyIPAddress("lzc365.top");
  address->set_port(80);
  LOG_INFO(Loggers::engine) << "baidu address:" << address->to_string();

  return address;
}

void test_http() {
  auto address = test_address();
  Socket::ptr sock = Socket::GenerateTCP(address);
  if (sock->connect(address)) {
    LOG_INFO(Loggers::engine)
        << "connect " << address->to_string() << " successful";
  }

  const char buf[] = "Get /archives/ HTTP/1.0\r\n\r\n";
  int ret = sock->send(buf, sizeof(buf));
  if (ret < 0) {
    LOG_INFO(Loggers::engine) << "send failed :" << ret;
    return;
  }

  std::string http_resp;
  http_resp.resize(4096);

  ret = sock->recv(http_resp.data(), http_resp.size());
  if (ret < 0) {
    LOG_INFO(Loggers::engine) << "recv failed :" << ret;
    return;
  }

  http_resp.resize(ret);

  LOG_INFO(Loggers::engine) << "recv response msg:" << http_resp;
}

void test_server() {
  TcpServer::ptr server(new TcpServer, [](TcpServer* pserver) {
    delete pserver;
    LOG_INFO(Loggers::engine) << "server stop";
  });

  Address::ptr address = IPAddress::LookupAny("127.0.0.1:9999");

  server->bind(address);

  server->start();
}

int main(int argc, char const* argv[]) {
  Loggers::engine->addAppender(StdOutLogAppender::Create());

  // test_address();
  test_http();

  // test_server();

  return 0;
}
