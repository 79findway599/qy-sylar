//测试socket模块结合Iomanager模块  测试失败。。。待重来




// #include "sylar/sylar.h"

// static sylar::Logger::ptr m_logger = SYLAR_LOG_ROOT();
// static auto socket                 = sylar::Socket::CreateTCPSocket();
// char buf[30]                = "something give you";

// void watch_io_read();
// void watch_io_write();

// void read_call() {
//     // auto client = socket->accept();

//     // client->toString();
//     char tempbuf[10];
//     memset(tempbuf, 0, sizeof(tempbuf));
//     socket->recv(tempbuf, sizeof(tempbuf));
//     sylar::IOManager::GetThis()->schedule(watch_io_read);

// }

// void write_call() {
//     // auto client = socket->accept();
//     // client->send(buf, strlen(buf) + 1);
//     // client->close();

//     socket->send(buf, strlen(buf) + 1);
//     sylar::IOManager::GetThis()->schedule(watch_io_write);
// }

// void watch_io_read() {

//     sylar::IOManager::GetThis()->addEvent(socket, sylar::IOManager::READ, read_call);
// }

// void watch_io_write()
// {
//     sylar::IOManager::GetThis()->addEvent(socket, sylar::IOManager::WRITE, write_call);
// }

// void test_socket() {

//     auto addr = sylar::Address::LookupAnyIPAddress("127.0.0.1:8888");

//     int ret;
//     ret = socket->bind(addr);
//     SYLAR_LOG_INFO(m_logger) << "bind ret = " << ret;
//     SYLAR_ASSERT(ret);

//     ret = socket->listen(3);
//     SYLAR_LOG_INFO(m_logger) << "listen ret = " << ret;
//     SYLAR_ASSERT(ret);

//     socket->accept();
//     sylar::IOManager::GetThis()->addEvent(socket, sylar::IOManager::WRITE, write_call);
//     sylar::IOManager:: : GetThis()->addEvent(socket, sylar::IOManager::READ, read_call);
// }

// int main() {
//     sylar::IOManager iom;
//     iom.schedule(test_socket);

//     return 0;
// }