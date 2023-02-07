#include "sylar/sylar.h"


sylar::Logger::ptr m_logger = SYLAR_LOG_ROOT();


class MyTcpServer : public sylar::TcpServer
{
    virtual void handleClient(sylar::Socket::ptr client) override;
};

void MyTcpServer::handleClient(sylar::Socket::ptr client)
{
    char buf[1024] = "www.bilibili.com";
    SYLAR_LOG_INFO(m_logger) << "new client" << client->toString();
    
    client->send(buf, strlen(buf) + 1);

    char buf2[3];
    //memset(buf, 0, sizeof(buf));
    client->recv(buf2, sizeof(buf2));
    SYLAR_LOG_INFO(m_logger) << "recv" << buf2;
    SYLAR_LOG_INFO(m_logger) << "***************************************";
    
    client->close();
}


void test_tcp_server()
{
    sylar::TcpServer::ptr server(new MyTcpServer);

    auto addr = sylar::Address::LookupAny("127.0.0.1:8080");
    auto addr2 = sylar::Address::LookupAnyIPAddress("127.0.0.1:9090");



    std::vector<sylar::Address::ptr> addrs;
    addrs.push_back(addr);
    addrs.push_back(addr2);

    std::vector<sylar::Address::ptr> fails_addrs;

    server->bind(addrs, fails_addrs);
    SYLAR_LOG_INFO(m_logger) << "bind success" << server->toString();
    
    server->start();
}


int main()
{
    sylar::IOManager iom(2);
    iom.schedule(test_tcp_server);

    return 0;
}