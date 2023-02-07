//测试socket模块


#include "sylar/sylar.h"


static sylar::Logger::ptr m_logger = SYLAR_LOG_ROOT();

char buf[1024] = "give you something";

int main()
{
 

    auto socket = sylar::Socket::CreateTCPSocket();
    auto addr = sylar::Address::LookupAnyIPAddress("127.0.0.1:9090");

    int ret;

    ret = socket->bind(addr);
    SYLAR_LOG_INFO(m_logger) << "bind ret = " << ret;
    SYLAR_ASSERT(ret);

    ret = socket->listen();
    SYLAR_LOG_INFO(m_logger) << "listen ret = " << ret;
    SYLAR_ASSERT(ret);

    
    while(1)
    {
        auto client = socket->accept();

        SYLAR_LOG_INFO(m_logger) << "accept client = " << client;
        SYLAR_ASSERT(ret);
        SYLAR_LOG_INFO(m_logger) << "new client:" << client->toString();

        client->send(buf, strlen(buf) + 1);
        sleep(2);
        client->close();
    }

   

    return 0;
}