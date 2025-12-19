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
#include <netinet/in.h>	
#include <fstream>
#include <chrono>

class TcpClient
{
public:
    TcpClient(int port,std::string ip)
    :_ip(ip),_port(port),len(sizeof(_tcp_client))
    {
        _fd=socket(AF_INET,SOCK_STREAM, 0);
        if(_fd<0)
        {
            perror("client socket error");
            exit(1);
        }
        ::memset(&_tcp_client, 0, len);
        _tcp_client.sin_family=AF_INET;
        _tcp_client.sin_port=htons(_port);
        _tcp_client.sin_addr.s_addr=inet_addr(_ip.c_str());
        if(connect(_fd, (struct sockaddr*)&_tcp_client,len)<0)
        {
            perror("connect error");
            exit(1);
        }
        std::cout<<"clinet build success"<<std::endl;
    }
    ~TcpClient()
    {
        stop();
    }
    void start()
    {
        while(true)
        {
            //不断地像服务端发送数据;
            for(int i=0;i<10;i++)
            {
                std::string message="hello"+std::to_string(i);
                send(message);
                rev(message);
            }
        }
    }
    void send(const std::string& message)
    {
        int n=::send(_fd,message.c_str(),message.size(), 0);
        if(n>0)
        {
            std::cout<<"send success"<<std::endl;
        }
    }
    int rev(std::string& message)
    {
        char buffer[1024];
        ::memset(buffer, 0, sizeof(buffer));  // 确保每次接收前清空缓冲区
        int n=::recv(_fd,buffer,sizeof(buffer)-1, 0);
        if (n > 0)
        {
            buffer[n] = '\0';  // 确保字符串以 '\0' 结尾
            message = buffer;  // 设置消息
            std::cout << "Received from server: " << message << std::endl;
        }
        else if (n == 0)
        {
            std::cout << "Server closed connection" << std::endl;
        }
        else
        {
            perror("recv error");
        }
        return n;
    }
private:
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
    int _port;
    std::string _ip;
    struct sockaddr_in _tcp_client;
    socklen_t len;    
};