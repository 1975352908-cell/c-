/*
    ========================================
    远程日志客户端 - 在本地机器上运行
    ========================================
    
    功能：
    1. 从配置文件加载日志器配置
    2. 获取 remote_logger 日志器
    3. 发送日志到远程服务器
    
    编译：
        g++ -std=c++17 -o client client.cc -lpthread
    
    运行：
        ./client
*/

#include "../logs/mylog.h"
#include <unistd.h>

int main()
{
    std::cout << "========================================" << std::endl;
    std::cout << "     远程日志客户端启动中...           " << std::endl;
    std::cout << "========================================" << std::endl;
    
    // 一行代码初始化：从配置文件加载所有日志器
    if (!mylog::init("./client.conf"))
    {
        std::cerr << "初始化失败！" << std::endl;
        return 1;
    }
    
    // 获取远程日志器
    auto remote = mylog::get("remote_logger");
    if (!remote)
    {
        std::cerr << "获取 remote_logger 失败！" << std::endl;
        return 1;
    }
    
    std::cout << "========================================" << std::endl;
    std::cout << "  开始发送日志到远程服务器...          " << std::endl;
    std::cout << "========================================" << std::endl;
    
    // 发送一些测试日志（使用宏，自动填充 __FILE__ 和 __LINE__）
    for (int i = 1; i <= 10; i++)
    {
        LOGGER_DEBUG(remote, "这是第 %d 条 DEBUG 日志", i);
        LOGGER_INFO(remote, "这是第 %d 条 INFO 日志", i);
        LOGGER_WARN(remote, "这是第 %d 条 WARN 日志", i);
        LOGGER_ERROR(remote, "这是第 %d 条 ERROR 日志", i);
        
        std::cout << "已发送第 " << i << " 批日志" << std::endl;
        usleep(100000);  // 100ms
    }
    
    std::cout << "========================================" << std::endl;
    std::cout << "  日志发送完成！                       " << std::endl;
    std::cout << "  请到服务端查看 remote_logs/data.log  " << std::endl;
    std::cout << "========================================" << std::endl;
    
    sleep(1);  // 等待异步日志落地
    return 0;
}

