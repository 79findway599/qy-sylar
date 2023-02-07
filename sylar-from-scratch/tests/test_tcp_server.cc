/**
 * @file test_tcp_server.cc
 * @brief TcpServer类测试
 * @version 0.1
 * @date 2021-09-18
 */
#include "sylar/sylar.h"

static sylar::Logger::ptr g_logger = SYLAR_LOG_ROOT();

static char str[1024] = {0};

/**
 * @brief 自定义TcpServer类，重载handleClient方法
 */
class MyTcpServer : public sylar::TcpServer {
protected:
    virtual void handleClient(sylar::Socket::ptr client) override;
};

void MyTcpServer::handleClient(sylar::Socket::ptr client) {
    SYLAR_LOG_INFO(g_logger) << "new client: " << client->toString();

    client->send(str,1024);
    
    static std::string buf;
    buf.resize(4096);
    client->recv(&buf[0], buf.length()); // 这里有读超时，由tcp_server.read_timeout配置项进行配置，默认120秒
    SYLAR_LOG_INFO(g_logger) << "recv: " << buf;
    client->close();
}

void run() {
    sylar::TcpServer::ptr server(new MyTcpServer); // 内部依赖shared_from_this()，所以必须以智能指针形式创建对象
    auto addr = sylar::Address::LookupAny("0.0.0.0:12345");
    SYLAR_ASSERT(addr);

    auto addr2 = sylar::Address::LookupAny("127.0.0.1:9999");
    SYLAR_ASSERT(addr2);
    std::vector<sylar::Address::ptr> addrs; 
    addrs.push_back(addr);
    addrs.push_back(addr2);

    std::vector<sylar::Address::ptr> fails;
    while(!server->bind(addrs, fails)) {
        sleep(2);
    }
    
    SYLAR_LOG_INFO(g_logger) << "bind success, " << server->toString();

    server->start();
}

int main(int argc, char *argv[]) {
    sylar::EnvMgr::GetInstance()->init(argc, argv);
    sylar::Config::LoadFromConfDir(sylar::EnvMgr::GetInstance()->getConfigPath());

    for(int i = 0; i < 1023; ++i)
    {
        str[i] = 'i';
    }
    str[1023] = '\0'; 

    sylar::IOManager iom(2);
    iom.schedule(&run);
    
    return 0;
}