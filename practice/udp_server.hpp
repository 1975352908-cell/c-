/* 练习udp服务器实现
*/
#include<iostream>
#include<string>
#include<cstring>
#include<unistd.h>
#include<sys/socket.h>
#include<netinet/in.h>
#include<arpa/inet.h>

class UdpServer
{
private:
    int _sockfd;  //socket文件描述符
    struct sockaddr_in _servaddr;  //服务器地址
    struct sockaddr_in _cliaddr;   //客户端地址
    socklen_t _cliaddr_len;//客户端地址长度
    int _port;  //服务器端口号
    std::string _ip;  //服务器ip地址
public:
    UdpServer(int port,std::string ip)
    :_port(port),_ip(ip)
    {
        _sockfd=socket(AF_INET,SOCK_DGRAM,0);
        if(_sockfd<0)
        {
            std::cerr<<"socket error"<<std::endl;
            return;
        }
        memset(&_servaddr,0,sizeof(_servaddr)); //清空服务器地址
        _servaddr.sin_family=AF_INET; //地址族
        _servaddr.sin_port=htons(_port); //端口号,htons将主机字节序转换为网络字节序,即主机字节序的端口号转换为网络字节序的端口号
        _servaddr.sin_addr.s_addr=inet_addr(_ip.c_str()); //ip地址,inet_addr将ip地址转换为网络字节序,即点分十进制转换为网络字节序的ip地址
        if(bind(_sockfd,(struct sockaddr*)&_servaddr,sizeof(_servaddr))<0)
        {
            std::cerr<<"bind error"<<std::endl;
            return;
        }
        std::cout<<"udp server start success"<<std::endl;
    }
    ~UdpServer()
    {
        stop();
    }
    UdpServer(const UdpServer&) = delete;
    UdpServer& operator=(const UdpServer&) = delete;
    void start()
    {
        while(1)
        {
            char buf[1024];//接收缓冲区
            struct sockaddr_in cliaddr;//客户端地址
            socklen_t len=sizeof(cliaddr);//客户端地址长度
            int n=recvfrom(_sockfd,buf,sizeof(buf),0,(struct sockaddr*)&cliaddr,&len);//接收数据
            if(n<0)
            {
                std::cerr<<"recvfrom error"<<std::endl;
                continue;//如果接收失败,则继续接收
            }
            _cliaddr = cliaddr;
            _cliaddr_len = len;
            std::cout<<"recv from "<<inet_ntoa(cliaddr.sin_addr)<<":"<<ntohs(cliaddr.sin_port)<<" "<<std::string(buf,n)<<std::endl;//打印接收到的数据
            send(std::string(buf,n));//发送数据
        }
    }
    void stop()
    {
        if (_sockfd >= 0) 
        {
            close(_sockfd);
            _sockfd = -1;
            std::cout << "udp server stop success" << std::endl;
        }
    }
    void send(std::string msg)
    {
        int n=sendto(_sockfd,msg.c_str(),msg.size(),0,(struct sockaddr*)&_cliaddr,_cliaddr_len);
        if(n<0)
        {
            std::cerr<<"sendto error"<<std::endl;
            return;
        }
        std::cout<<"send to "<<inet_ntoa(_cliaddr.sin_addr)<<":"<<ntohs(_cliaddr.sin_port)<<" "<<std::string(msg.c_str(),n)<<std::endl;
    }
    void recv(std::string& msg)
    {
        char buf[1024];
        struct sockaddr_in cliaddr;
        socklen_t len=sizeof(cliaddr);
        int n=recvfrom(_sockfd,buf,sizeof(buf),0,(struct sockaddr*)&cliaddr,&len);
        if(n<0)
        {
            std::cerr<<"recvfrom error"<<std::endl;
            return;
        }
        //保存客户端地址
        _cliaddr = cliaddr;
        _cliaddr_len = len;
        msg = std::string(buf,n);
        std::cout<<"recv from "<<inet_ntoa(_cliaddr.sin_addr)<<":"<<ntohs(_cliaddr.sin_port)<<" "<<std::string(msg.c_str(),n)<<std::endl;
    }
};