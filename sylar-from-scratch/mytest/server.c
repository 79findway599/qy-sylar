#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <string.h>
#include <sys/socket.h>//socket相关头文件
#include <netinet/in.h>//sockaddr_in头文件
#include <arpa/inet.h>//大端转小端相关头文件
#include <pthread.h>//线程函数头文件
#include <unistd.h>

void thread_run(void *arg)
{
    printf("create a new thread\n");
    int fd = (int)arg;//fd表示连接的socket句柄，通过该变量可以进行网络通讯
    char buf[1024] = {"www.qy.com"};

    //6、开始通讯
    while(1)
    {
        // memset(buf,0,sizeof(buf));
        // //6-1、接收数据
        // int ret = recv(fd,buf,sizeof(buf) - 1,0);//0表示一次收发
        // if(ret > 0)
        // {
        //     buf[ret] = '\0';
        //     printf("client say : %s\n",buf);
        // }

        //memset(buf,0,sizeof(buf));
        //printf("please Enter:");
        //fflush(stdout);

        //scanf("%s",buf);
        int ret = strlen(buf);
        //6-2、发送数据
        if(ret > 0)
        {
            send(fd,buf,strlen(buf) + 1,0);
        }
        close(fd);
        break;
    }
}

int main(int argc,char *argv[])
{
    if(argc != 3)//调用可执行程序时，必需传递ip地址和端口号
    {
        printf("please use : %s [IP] [port]\n",argv[0]);
        exit(1);
    }

    int sock;//sock套接字
    sock = socket(AF_INET,SOCK_STREAM,IPPROTO_TCP);//1、创建套接字
    if(sock < 0)
    {
        perror("create socket error\n");
        return 1;
    }

    //2、初始化协议地址簇
    struct sockaddr_in local;
    local.sin_family = AF_INET;//该处的参数应和创建socket是参数1一致
    //主机数据和网络数据是有区别
    local.sin_port = htons(atoi(argv[2]));//主机字节序转网络字节序，主机字节序是小端，网络字节序是大端存储
    local.sin_addr.s_addr = inet_addr(argv[1]);//ip地址是一个点分十进制字符串，需要转成网络字节序的整数

    //3、服务端需要绑定
    if(bind(sock,(struct sockaddr *)&local,sizeof(local)) < 0)
    {
        perror("bind error\n");
        close(sock);
        return 1;
    }

    //4、服务端需要监听
    if(listen(sock,10) < 0)
    {
        perror("listen error\n");
        close(sock);
        return 1;
    }
    printf("bind and listen success! wait accept ... \n");

    //5、接收连接
    struct sockaddr_in clientSock;
    socklen_t len = sizeof(clientSock);
    while(1)//死循环来接收连接，及接收数据
    {
        //accept函数为阻塞函数
        int fd = accept(sock,(struct sockaddr *)&clientSock,&len);
        if(fd < 0)
        {
            perror("accept error\n");
            close(sock);
            return 1;
        }

        printf("get connect, ip is : %s port is : %d\n",
            inet_ntoa(clientSock.sin_addr),ntohs(clientSock.sin_port));

        //6、通讯（用多线程来演示）
        pthread_t id;
        pthread_create(&id,NULL,thread_run,(void *)fd);
        pthread_detach(id);
    }

    //7、
    close(sock);
	

    return 0;
}