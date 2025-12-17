
// #include<stdio.h>
// #define LOG(fmt,...) printf("[%s:%d]" fmt "\n" ,__FILE__,__LINE__,##__VA_ARGS__)
// int main()
// {
//     LOG("%s-%s","hello","world");
//     LOG("%d,%d",1,2);
//     return 0;
// }

#include"udp_server.hpp"
#include"udp_client.hpp"
int main()
{
    UdpServer server(8080,"127.0.0.1");
    UdpClient client(8080,"127.0.0.1");
    
    // 客户端发送消息
    client.send("Hello Server!");
    
    // 服务器接收并回复
    std::string recv_msg;
    server.recv(recv_msg);
    
    // 客户端接收服务器回复
    std::string reply;
    client.recv(reply);
}