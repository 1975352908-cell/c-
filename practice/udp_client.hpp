#include<iostream>
#include<string>
#include<cstring>
#include<unistd.h>
#include<sys/socket.h>
#include<netinet/in.h>
#include<arpa/inet.h>

class UdpClient
{
private:
    int _sockfd;
    struct sockaddr_in _servaddr;
    socklen_t _servaddr_len=sizeof(_servaddr);
    int _port;
    std::string _ip;
public:
    UdpClient(int port,std::string ip)
    :_port(port),_ip(ip)
    {
        _sockfd=socket(AF_INET,SOCK_DGRAM,0);
        if(_sockfd<0)
        {
            std::cerr<<"socket error"<<std::endl;
            return;
        }
        memset(&_servaddr,0,sizeof(_servaddr));
        _servaddr.sin_family=AF_INET;
        _servaddr.sin_port=htons(_port);
        _servaddr.sin_addr.s_addr=inet_addr(_ip.c_str());
        _servaddr_len=sizeof(_servaddr);
    }
    ~UdpClient()
    {
        stop();
    }
    UdpClient(const UdpClient&) = delete;
    UdpClient& operator=(const UdpClient&) = delete;
    void stop()
    {
        if (_sockfd >= 0) 
        {
            close(_sockfd);
            _sockfd = -1;
            std::cout << "udp client stop success" << std::endl;
        }
    }
    void send(std::string msg)
    {
        int n=sendto(_sockfd,msg.c_str(),msg.size(),0,(struct sockaddr*)&_servaddr,_servaddr_len);
        if(n<0)
        {
            std::cerr<<"sendto error"<<std::endl;
            return;
        }
    }
    void recv(std::string& msg)
    {
        char buf[1024];
        struct sockaddr_in peeraddr;
        socklen_t len=sizeof(peeraddr);
        int n=recvfrom(_sockfd,buf,sizeof(buf),0,(struct sockaddr*)&peeraddr,&len);
        if(n<0)
        {
            std::cerr<<"recvfrom error"<<std::endl;
            return;
        }
         // 设置输出参数
        msg = std::string(buf,n);
        std::cout << "recv from " << inet_ntoa(peeraddr.sin_addr)
        << ":" << ntohs(peeraddr.sin_port)
        << " " << msg << std::endl;
    }
};