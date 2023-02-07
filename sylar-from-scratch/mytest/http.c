#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <unistd.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <pthread.h>

#define BUFFSIZE 1024

int socket_create(int port);
int socket_accept(int st);
void setdaemon();//设置守护进程

int main(int argc,char *argv[])
{
	if(argc<2)
	{
		printf("usage:server port \n");
		return 0;
	}
	 
	int port = atoi(argv[1]);
	if(port <= 0)
	{
		printf("port must be positive integer: \n");
		return 0;
	}

	int st =socket_create(port);
	if(st == 0)
	{
		return 0;
	}
	setdaemon();//设置守护进程
	socket_accept(st);
	close(st);
}

char LOGBUF[BUFFSIZE];//日志缓冲

void save_log(char *buf)
{
	FILE *fp = fopen("log.txt","a+");
	fputs(buf,fp);
	fclose(fp);
}

int socket_create(int port)
{
	int st = socket(AF_INET, SOCK_STREAM, 0);
	int option = 1;//设置端口重用
	if (st == -1)
	{
		memset(LOGBUF,0,sizeof(LOGBUF));
		//__FILE__  表示当前文件名
		//__LINE__  表示当前行数 
		sprintf(LOGBUF,"%s,%d:socket error %s\n", __FILE__, __LINE__, strerror(errno));
		save_log(LOGBUF);
		return 0;
	}
	
	if (setsockopt(st, SOL_SOCKET, SO_REUSEADDR, &option, sizeof(option)) == -1)//监听端口重用
	{
		memset(LOGBUF,0,sizeof(LOGBUF));
		sprintf(LOGBUF,"setsockopt failed %s\n", strerror(errno));
		save_log(LOGBUF);
		return 0;
	}
	
	struct sockaddr_in sockaddr;
	memset(&sockaddr, 0, sizeof(sockaddr));
	sockaddr.sin_port = htons(port);
	sockaddr.sin_family = AF_INET;
	sockaddr.sin_addr.s_addr = htonl(INADDR_ANY);//0.0.0.0
	if (bind(st, (struct sockaddr *) &sockaddr, sizeof(sockaddr)) == -1)
	{
		memset(LOGBUF,0,sizeof(LOGBUF));
		sprintf(LOGBUF,"%s,%d:bind error %s \n", __FILE__, __LINE__, strerror(errno));
		save_log(LOGBUF);
		return 0;
	}

	if (listen(st, 100) == -1)
	{
		memset(LOGBUF,0,sizeof(LOGBUF));
		sprintf(LOGBUF,"%s,%d:listen failture %s\n", __FILE__, __LINE__,strerror(errno));
		save_log(LOGBUF);
		return 0;
	}
	printf("start server success!\n");
	return st;
}

void setdaemon()
{
	pid_t pid;
	pid = fork();
	if (pid < 0)
	{
		memset(LOGBUF,0,sizeof(LOGBUF));
		sprintf(LOGBUF,"fork failed %s\n", strerror(errno));
		save_log(LOGBUF);
		exit(1);
	}
	if (pid > 0)
	{
		exit (1);
	}

	if (setsid() < 0)
	{
		printf("setsid failed %s\n", strerror(errno));
		exit (1);
	}

	 umask(0);
	 close(STDIN_FILENO);
	 close(STDOUT_FILENO);
	 close(STDERR_FILENO);
}

void *http_thread(void *argc);//线程处理函数

int socket_accept(int st)
{
	int client_st;
	struct sockaddr_in client_sockaddr;
	socklen_t len = sizeof(client_sockaddr);

	pthread_t thrd_t;
	pthread_attr_t attr;//线程属性，设置线程分离,将线程的回收工作交由系统来自动完成
	pthread_attr_init(&attr);//初始化线程属性
	//设置线程分离属性  PTHREAD_CREATE_DETACHED 表示不需要用pthread_join来回收线程资源，由系统自动回收
	pthread_attr_setdetachstate(&attr,PTHREAD_CREATE_DETACHED);
	memset(&client_sockaddr, 0, sizeof(client_sockaddr));

	while (1)
	{
		client_st = accept(st, (struct sockaddr *) &client_sockaddr, &len);
		
		if (client_st == -1)
		{
			memset(LOGBUF,0,sizeof(LOGBUF));
			sprintf(LOGBUF,"%s,%d:accept failture %s \n", __FILE__, __LINE__,strerror(errno));
			save_log(LOGBUF);
			return 0;
		} 
		else
		{
			//创建线程处理函数
			int *temp = (int*)malloc(sizeof(int));
			*temp = client_st;
			pthread_create(&thrd_t, &attr, http_thread, temp);//客户端句柄当成线程函数参数
		}
	}
	pthread_atrr_destory(&attr);
}

void get_http_command(char *http_msg, char *command);//解析 http 协议中的 URL

int make_http_content(const char *command, char **content);//获取 http 请求的文件内容

void *http_thread(void *argc)
{
	if(argc == NULL)
	{
		return NULL;
	}
	int st = *(int*) argc;
	free((int*)argc);

	char buf[BUFFSIZE];//用来保存请求报文 
	memset(buf, 0, sizeof(buf));
	int rc = recv(st, buf, sizeof(buf), 0);
	if (rc <= 0)//网络中断返回0
	{
		memset(LOGBUF,0,sizeof(LOGBUF));
		sprintf(LOGBUF,"recv failed %s\n", strerror(errno));
		save_log(LOGBUF);
	} 
	else
	{
		char command[BUFFSIZE];// URL 缓冲区
		memset(command, 0, sizeof(command));
		get_http_command(buf, command);//获取 URL 
		char *content = NULL;
		int ilen = make_http_content(command, &content);//获得文件内容
		if (ilen > 0)
		{
			send(st, content, ilen, 0);
			free(content);
		}
	}
	close(st);
	return NULL;
}

void get_http_command(char *http_msg, char *command)
{
	char *p_end = NULL;
	char *p_start = http_msg;
	while (*p_start) 
	{
		if (*p_start == '/')
		{
			break;
		}
		p_start++;
	}
	p_start++;

	//strchr 查找 \n 第一次出现的位置 
	p_end = strchr(http_msg, '\n');
	while (p_end != p_start)
	{
		if (*p_end == ' ')
		{
			break;
		}
		p_end--;
	}
	strncpy(command, p_start, p_end - p_start);
}

int  get_file_content(const char *file_name, char **content);//得到文件内容

const char *get_filetype(const char *filename);

int make_http_content(const char *command, char **content)
{
	char *file_buf;
	int file_length;
	char headbuf[BUFFSIZE];//http 报文首部缓冲区

	if (command[0] == 0)
	{
		file_length = get_file_content("index.html", &file_buf);
	} 
	else
	{
		file_length = get_file_content(command, &file_buf);
	}
    
	if (file_length == 0)
	{
		return 0;
	}

	memset(headbuf, 0, sizeof(headbuf));
	
	sprintf(headbuf, "HTTP/1.0 200 OK\n\
                        Content-Type: %s\n\
                        Transfer-Encoding: chunked\n\
						Connection: Keep-Alive\n\
                        Accept-Ranges:bytes\n\
                        Content-Length:%d\n\n", get_filetype(command), file_length);
	
	int iheadlen = strlen(headbuf);
	int itaillen = strlen("\r\n");
	int isumlen = iheadlen + itaillen + file_length;
	*content = (char *) malloc(isumlen);
	if(*content == NULL)
	{
		memset(LOGBUF,0,sizeof(LOGBUF));
		sprintf(LOGBUF,"malloc failed %s\n", strerror(errno));
		save_log(LOGBUF);
	}
	
    //组 http 报文
	char *tmp = *content;
	memcpy(tmp, headbuf, iheadlen);
	memcpy(&tmp[iheadlen], file_buf, file_length);
	memcpy(&tmp[iheadlen] + file_length, "\r\n", itaillen);
	
	if (file_buf)
	{
		free(file_buf);
	}
	return isumlen;
}

//得到文件内容
int  get_file_content(const char *file_name, char **content)
{
	int  file_length = 0;
	FILE *fp = NULL;

	if (file_name == NULL)
	{
		return file_length;
	}
	
	fp = fopen(file_name, "rb");
	if (fp == NULL)
	{
		memset(LOGBUF,0,sizeof(LOGBUF));
		sprintf(LOGBUF,"file name: %s,%s,%d:open file failture %s \n",file_name, __FILE__, __LINE__,strerror(errno));
		save_log(LOGBUF);
		return file_length;
	}

	fseek(fp, 0, SEEK_END);
	file_length = ftell(fp);
	rewind(fp);//将文件内部指针重新指向一个流的开头，理解为将内部指针定位到文件开头

	*content = (char *) malloc(file_length);
	if (*content == NULL)
	{
		memset(LOGBUF,0,sizeof(LOGBUF));
		sprintf(LOGBUF,"%s,%d:malloc failture %s \n", __FILE__, __LINE__,strerror(errno));
		save_log(LOGBUF);
		return 0;
	}
	
	fread(*content, file_length, 1, fp);
	fclose(fp);

	return file_length;
}

const char *get_filetype(const char *filename)
{
	char sExt[32];
	const char *p_start = filename;
	memset(sExt, 0, sizeof(sExt));
	while(*p_start)
	{
		if (*p_start == '.')
		{
			p_start++;
			strncpy(sExt, p_start, sizeof(sExt));
			break;
		}
		p_start++;
	}

	//匹配文件后缀名，返回表示类型的字符串
	if (strncmp(sExt, "bmp", 3) == 0)
		return "image/bmp";

	if (strncmp(sExt, "gif", 3) == 0)
		return "image/gif";

	if (strncmp(sExt, "ico", 3) == 0)
		return "image/x-icon";

	if (strncmp(sExt, "jpg", 3) == 0)
		return "image/jpeg";

	if (strncmp(sExt, "avi", 3) == 0)
		return "video/avi";

	if (strncmp(sExt, "css", 3) == 0)
		return "text/css";

	if (strncmp(sExt, "dll", 3) == 0)
		return "application/x-msdownload";

	if (strncmp(sExt, "exe", 3) == 0)
		return "application/x-msdownload";

	if (strncmp(sExt, "dtd", 3) == 0)
		return "text/xml";

	if (strncmp(sExt, "mp3", 3) == 0)
		return "audio/mp3";

	if (strncmp(sExt, "mpg", 3) == 0)
		return "video/mpg";

	if (strncmp(sExt, "png", 3) == 0)
		return "image/png";

	if (strncmp(sExt, "xls", 3) == 0)
		return "application/vnd.ms-excel";
    
	if (strncmp(sExt, "doc", 3) == 0)
		return "application/msword";

	if (strncmp(sExt, "mp4", 3) == 0)
		return "video/mpeg4";

	if (strncmp(sExt, "ppt", 3) == 0)
		return "application/x-ppt";

	if (strncmp(sExt, "wma", 3) == 0)
		return "audio/x-ms-wma";

	if (strncmp(sExt, "wmv", 3) == 0)
		return "video/x-ms-wmv";

	return "text/html";//都没有匹配
}