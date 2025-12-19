#include <arpa/inet.h>
#include <cstring>
#include<iostream>
#include<sys/socket.h>
#include<netinet/in.h>
#include<string>
#include<sys/cdefs.h>
#include<memory.h>
#include <features.h>
#include <unistd.h>
#include<thread>
#include <netinet/in.h>	
#include <cassert>
#include <fstream>
#include <chrono>

class TcpServer
{
public:
    TcpServer(int port,std::string ip)
    :_port(port)
    ,_ip(ip)
    ,_tcp_server_len(sizeof(_tcp_server))
    {
        //创建fd
        _fd=socket(AF_INET,SOCK_STREAM, 0);
        if(_fd<0)
        {
            perror("socket error");
            exit(1);
        }
        int opt = 1;
        ::setsockopt(_fd,SOL_SOCKET,SO_REUSEADDR,(const void*)& opt ,sizeof(opt));
        //清零操作
        ::memset(&_tcp_server, 0,sizeof(_tcp_server) );
        _tcp_server.sin_family=AF_INET;
        _tcp_server.sin_port=htons(_port);
        _tcp_server.sin_addr.s_addr=INADDR_ANY;
        //bind绑定;
        if(bind(_fd,(struct sockaddr*)&_tcp_server, _tcp_server_len)<0)
        {
            perror("bind error");
            return;
        }
        if(::listen(_fd,10)<0)
        {
            std::cout<<"listen error"<<std::endl;
            return;
        }
        std::cout<<"服务端成功启动"<<std::endl;
    }
    ~TcpServer()
    {
        stop();
    }
    void start()
    {
        while(true)
        {
            int child_fd=::accept(_fd, (struct sockaddr *)&_tcp_server,&_tcp_server_len);
            if(child_fd>0)
            std::thread(&TcpServer::work,this,child_fd).detach();
        }
    }
    TcpServer(const TcpServer& server)=delete;
private:
    void work(int child_fd)
    {
        char buffer[1024];
        while(true)
        {
            int n=::recv(child_fd,buffer,sizeof(buffer),0);
            if(n<0)
            {
                ::close(child_fd);
                break;
            }
            ::send(child_fd,buffer,n,0);
        }
    }
    void stop()
    {
        if(_fd>0)
        {
            ::close(_fd);
            return;
        }
    }
private:
    int _fd;
    short _port;
    std::string _ip;
    struct sockaddr_in _tcp_server;
    socklen_t _tcp_server_len;
};