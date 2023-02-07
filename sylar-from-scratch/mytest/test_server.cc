// #include <arpa/inet.h>
// #include <fcntl.h>
// #include <netinet/in.h>
// #include <sylar/sylar.h>
// #include <sys/socket.h>

// static sylar::Logger::ptr g_logger = SYLAR_LOG_ROOT();



// //void watch_read();
// static int create_socket() 
// {
//     int sock = socket(AF_INET, SOCK_STREAM, 0);

//     const char *ip   = "127.0.0.1";

//     fcntl(sock,F_SETFL,O_NONBLOCK);
//     int opt = 1;
//     setsockopt(sock,SOL_SOCKET,SO_REUSEADDR,&opt,sizeof(opt));

//     sockaddr_in local_addr;
//     local_addr.sin_family      = AF_INET;
//     local_addr.sin_port        = ntohs(8888);
//     local_addr.sin_addr.s_addr = inet_addr(ip);

//     if (bind(sock, (const sockaddr *)&local_addr, sizeof(local_addr)) < 0) {
//         SYLAR_LOG_ERROR(g_logger) << "bind error!";
//         close(sock);
//         return -1;
//     }

//     if (listen(sock, 10) < 0) {
//         SYLAR_LOG_ERROR(g_logger) << "listen error!";
//         close(sock);
//         return -1;
//     }

//     return sock;
// }

// int sock_fd = create_socket();

// void handle_read()
// {
//     sockaddr_in recv_addr;
//     socklen_t len = sizeof(recv_addr);

//     int accept_fd = accept(sock_fd,(sockaddr *)&recv_addr, &len);
//     fcntl(accept_fd,F_SETFL,O_NONBLOCK);
    
//     sylar::IOManager::GetThis()->schedule(watch_read);
// }

// void handle_write()
// {

//     char buf[1024] = {"echo server!"};

//     send(sock_fd,buf,strlen(buf) + 1,0);

// }


//  void watch_read()
//  {
    
//      sylar::IOManager::GetThis()->addEvent(sock_fd,sylar::IOManager::READ,handle_read);   
//  }

// void fun() 
// {
//     //sylar::IOManager::GetThis()->addEvent(sock_fd, sylar::IOManager::READ,handle_read);  
//     sylar::IOManager::GetThis()->addEvent(sock_fd, sylar::IOManager::WRITE,handle_write);  
// }

// int main() 
// {
//     SYLAR_LOG_INFO(g_logger) << "main begin";
//     sylar::IOManager iom;
//     iom.schedule(&fun);
    
//     SYLAR_LOG_INFO(g_logger) << "main end";
//     return 0;
// }