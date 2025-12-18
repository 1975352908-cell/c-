#include "udp_server.hpp"

int main()
{
    UdpServer server(8082, "127.0.0.1");

    // 启动服务器接收循环
    server.start();

    return 0;
}