/*
    ========================================
    远程日志服务端 - 在远程机器上运行
    ========================================
    
    功能：
    1. 启动 TCP 服务器（端口 9010），接收 TCP 日志
    2. 启动 UDP 服务器（端口 9011），接收 UDP 日志  
    3. 将接收到的日志落地到 ./remote_logs/data.log
    
    编译：
        g++ -std=c++17 -o server server.cc -lpthread
    
    运行：
        ./server
*/

#include "../logs/log_server.hpp"

int main()
{
    std::cout << "========================================" << std::endl;
    std::cout << "     远程日志服务器启动中...           " << std::endl;
    std::cout << "========================================" << std::endl;
    
    // 创建日志服务器
    // 参数说明：
    //   9010  - TCP监听端口（客户端用 TcpSink 连接这个端口）
    //   9011  - UDP监听端口（客户端用 UdpSink 发送到这个端口）
    //   路径  - 日志落地文件路径
    //   10000 - 内存中保留的最近日志条数
    auto server = std::make_shared<mylog::LogServer>(
        9010,                          // TCP 端口
        9011,                          // UDP 端口
        "./remote_logs/data.log",      // 日志保存路径
        10000                          // 保留最近10000条
    );
    
    std::cout << "========================================" << std::endl;
    std::cout << "  服务器已启动，等待客户端连接...      " << std::endl;
    std::cout << "  TCP 端口: 9010                       " << std::endl;
    std::cout << "  UDP 端口: 9011                       " << std::endl;
    std::cout << "  日志文件: ./remote_logs/data.log     " << std::endl;
    std::cout << "========================================" << std::endl;
    
    // 启动服务器（阻塞运行）
    server->start();
    
    return 0;
}

