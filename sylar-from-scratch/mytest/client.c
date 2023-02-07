#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <string.h>
#include <sys/socket.h>//socket相关头文件
#include <netinet/in.h>//sockaddr_in头文件
#include <arpa/inet.h>//大端转小端相关头文件


int main(int argc,char *argv[])
{
    if(argc != 3)//调用可执行程序时，必需传递ip地址和端口号
    {
        printf("please use : %s [IP] [port]\n",argv[0]);
        exit(1);
    }


    int sock;//sock套接字
    sock = socket(AF_INET,SOCK_STREAM,0);//1、创建套接字
    if(sock < 0)
    {
        perror("create socket error\n");
        return 1;
    }

    //端口及ip用服务端的
    struct sockaddr_in local;
    local.sin_family = AF_INET;//该处的参数应和创建socket是参数1一致
    //主机数据和网络数据是有区别
    local.sin_port = htons(atoi(argv[2]));//主机字节序转网络字节序，主机字节序是小端，网络字节序是大端存储
    local.sin_addr.s_addr = inet_addr(argv[1]);//ip地址是一个点分十进制字符串，需要转成网络字节序的整数

    //客户端不需要绑定和监听，直接连接即可
    int ret = connect(sock,(struct sockaddr*)&local,sizeof(local));
    if(ret < 0)
    {
        perror("connet error\n");
        close(sock);
        return 1;
    }
    printf("connect success!\n");

    char buf[1024] = {};
    //开始通讯
    while(1)
    {
        memset(buf,0,sizeof(buf));
        printf("please Enter:");
        fflush(stdout);

        scanf("%s",buf);
        ret = strlen(buf);
        //发送数据
        if(ret > 0)
        {
            send(sock,buf,strlen(buf) + 1,0);

            //接收数据
            int ret = recv(sock,buf,sizeof(buf) - 1,0);//0表示一次收发
            if(ret > 0)
            {
                //忽略大小写比较字符串，比较参数3表示的n个字符
                if(strncasecmp(buf,"quit",4) == 0)
                {
                    printf("quit\n");
                    break;
                }
                buf[ret] = '\0';
                printf("server say : %s\n",buf);
            }
        }

        

        // memset(buf,0,sizeof(buf));
        
    }
    close(sock);
    return 0;
}