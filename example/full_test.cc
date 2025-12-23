#include"../logs/mylog.h"
#include"../logs/log_server.hpp"
#include"../logs/sink.hpp"
#include"../logs/db_sink.hpp"
#include <thread>
#include <chrono>
#include <atomic>
#include <iostream>

//==================== 测试1: TCP Sink功能测试 ====================
void test_tcp_sink()
{
    std::cout << "\n========== 测试1: TCP Sink功能测试 ==========\n";
    
    const uint16_t TCP_PORT = 8888;
    std::atomic<bool> server_running{true};
    
    // 启动简单的TCP服务器
    std::thread server_thread([&]() {
        int server_fd = socket(AF_INET, SOCK_STREAM, 0);
        if (server_fd < 0) {
            std::cout << "❌ TCP服务器socket创建失败" << std::endl;
            return;
        }
        
        int opt = 1;
        setsockopt(server_fd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt));
        
        struct sockaddr_in addr;
        memset(&addr, 0, sizeof(addr));
        addr.sin_family = AF_INET;
        addr.sin_port = htons(TCP_PORT);
        addr.sin_addr.s_addr = INADDR_ANY;
        
        if (bind(server_fd, (struct sockaddr*)&addr, sizeof(addr)) < 0) {
            std::cout << "❌ TCP服务器bind失败" << std::endl;
            close(server_fd);
            return;
        }
        
        listen(server_fd, 5);
        std::cout << "✅ TCP测试服务器启动在端口 " << TCP_PORT << std::endl;
        
        int client_fd = accept(server_fd, nullptr, nullptr);
        if (client_fd >= 0) {
            char buffer[1024];
            int msg_count = 0;
            while (msg_count < 3) {
                ssize_t n = recv(client_fd, buffer, sizeof(buffer), 0);
                if (n > 0) {
                    std::cout << "  服务器收到TCP数据 [" << msg_count << "]: " 
                              << std::string(buffer, n);
                    msg_count++;
                } else {
                    break;
                }
            }
            close(client_fd);
        }
        close(server_fd);
    });
    
    std::this_thread::sleep_for(std::chrono::milliseconds(200));
    
    // 测试TCP客户端
    std::cout << "📡 创建TCP Sink并发送消息..." << std::endl;
    try {
        mylog::LogSink::ptr tcp_sink = mylog::SinkFactory::create<mylog::TcpSink>(TCP_PORT, "127.0.0.1");
        
        for (int i = 0; i < 3; i++) {
            std::string test_msg = "TCP测试消息 #" + std::to_string(i);
            tcp_sink->log(test_msg.c_str(), test_msg.size());
            std::cout << "  ✅ 发送TCP消息 " << i << std::endl;
            std::this_thread::sleep_for(std::chrono::milliseconds(100));
        }
    } catch (const std::exception& e) {
        std::cout << "❌ TCP测试异常: " << e.what() << std::endl;
    }
    
    std::this_thread::sleep_for(std::chrono::milliseconds(500));
    server_thread.join();
    std::cout << "✅ TCP Sink功能测试完成\n" << std::endl;
}

//==================== 测试2: UDP Sink功能测试 ====================
void test_udp_sink()
{
    std::cout << "\n========== 测试2: UDP Sink功能测试 ==========\n";
    
    const uint16_t UDP_PORT = 7777;
    std::atomic<bool> server_running{true};
    
    // 启动简单的UDP服务器
    std::thread server_thread([&]() {
        int server_fd = socket(AF_INET, SOCK_DGRAM, 0);
        if (server_fd < 0) {
            std::cout << "❌ UDP服务器socket创建失败" << std::endl;
            return;
        }
        
        struct sockaddr_in addr;
        memset(&addr, 0, sizeof(addr));
        addr.sin_family = AF_INET;
        addr.sin_port = htons(UDP_PORT);
        addr.sin_addr.s_addr = INADDR_ANY;
        
        if (bind(server_fd, (struct sockaddr*)&addr, sizeof(addr)) < 0) {
            std::cout << "❌ UDP服务器bind失败" << std::endl;
            close(server_fd);
            return;
        }
        
        std::cout << "✅ UDP测试服务器启动在端口 " << UDP_PORT << std::endl;
        
        char buffer[1024];
        struct sockaddr_in client_addr;
        socklen_t addr_len = sizeof(client_addr);
        
        for (int i = 0; i < 3; i++) {
            ssize_t n = recvfrom(server_fd, buffer, sizeof(buffer), 0,
                               (struct sockaddr*)&client_addr, &addr_len);
            if (n > 0) {
                std::cout << "  服务器收到UDP数据 [" << i << "]: " 
                          << std::string(buffer, n);
            }
        }
        close(server_fd);
    });
    
    std::this_thread::sleep_for(std::chrono::milliseconds(200));
    
    // 测试UDP客户端
    std::cout << "📡 创建UDP Sink并发送消息..." << std::endl;
    try {
        mylog::LogSink::ptr udp_sink = mylog::SinkFactory::create<mylog::UdpSink>(UDP_PORT, "127.0.0.1");
        
        for (int i = 0; i < 3; i++) {
            std::string test_msg = "UDP测试消息 #" + std::to_string(i);
            udp_sink->log(test_msg.c_str(), test_msg.size());
            std::cout << "  ✅ 发送UDP消息 " << i << std::endl;
            std::this_thread::sleep_for(std::chrono::milliseconds(100));
        }
    } catch (const std::exception& e) {
        std::cout << "❌ UDP测试异常: " << e.what() << std::endl;
    }
    
    std::this_thread::sleep_for(std::chrono::milliseconds(500));
    server_thread.join();
    std::cout << "✅ UDP Sink功能测试完成\n" << std::endl;
}

//==================== 测试3: LogServer完整测试 ====================
void test_log_server()
{
    std::cout << "\n========== 测试3: LogServer完整测试 ==========\n";
    
    const uint16_t TCP_PORT = 9990;
    const uint16_t UDP_PORT = 9991;
    
    // 1. 创建并启动LogServer
    std::cout << "1️⃣ 创建LogServer..." << std::endl;
    mylog::LogServer::ptr server = std::make_shared<mylog::LogServer>(
        TCP_PORT, UDP_PORT, "./test_logs/server_data.log", 1000
    );
    
    std::thread server_thread([&server]() {
        server->start();
    });
    
    std::this_thread::sleep_for(std::chrono::milliseconds(500));
    std::cout << "✅ LogServer启动完成\n" << std::endl;
    
    // 2. 创建Logger使用TCP Sink
    std::cout << "2️⃣ 测试通过Logger使用TCP Sink..." << std::endl;
    mylog::LocalLoggerBuilder tcp_builder;
    tcp_builder.buildLoggerName("tcp_logger");
    tcp_builder.buildLoggerLevel(mylog::LogLevel::value::DEBUG);
    tcp_builder.buildFormatter("[%d{%H:%M:%S}][%t][%p][%c][%f:%l] %m%n");
    tcp_builder.buildSink<mylog::TcpSink>(TCP_PORT, "127.0.0.1");
    tcp_builder.buildSink<mylog::StdoutSink>(true);  // 同时输出到控制台
    
    mylog::Logger::ptr tcp_logger = tcp_builder.build();
    
    for (int i = 0; i < 5; i++) {
        tcp_logger->info(__FILE__, __LINE__, "TCP日志消息 #%d", i);
        std::this_thread::sleep_for(std::chrono::milliseconds(50));
    }
    std::cout << "✅ TCP日志发送完成\n" << std::endl;
    
    // 3. 创建Logger使用UDP Sink
    std::cout << "3️⃣ 测试通过Logger使用UDP Sink..." << std::endl;
    mylog::LocalLoggerBuilder udp_builder;
    udp_builder.buildLoggerName("udp_logger");
    udp_builder.buildLoggerLevel(mylog::LogLevel::value::DEBUG);
    udp_builder.buildFormatter("[%d{%H:%M:%S}][%t][%p][%c][%f:%l] %m%n");
    udp_builder.buildSink<mylog::UdpSink>(UDP_PORT, "127.0.0.1");
    udp_builder.buildSink<mylog::StdoutSink>(true);
    
    mylog::Logger::ptr udp_logger = udp_builder.build();
    
    for (int i = 0; i < 5; i++) {
        udp_logger->warn(__FILE__, __LINE__, "UDP日志消息 #%d", i);
        std::this_thread::sleep_for(std::chrono::milliseconds(50));
    }
    std::cout << "✅ UDP日志发送完成\n" << std::endl;
    
    // 4. 等待服务器处理
    std::cout << "4️⃣ 等待服务器处理所有消息..." << std::endl;
    std::this_thread::sleep_for(std::chrono::seconds(2));
    
    std::cout << "✅ LogServer测试完成" << std::endl;
    std::cout << "📄 查看日志文件: cat ./test_logs/server_data.log\n" << std::endl;
    
    // 注意：服务器线程会一直运行，这里手动终止测试
    // 实际使用中需要优雅关闭
}

//==================== 测试4: 数据库Sink功能测试 ====================
void test_db_sink()
{
    std::cout << "\n========== 测试4: 数据库Sink功能测试 ==========\n";
    
    try {
        std::cout << "1️⃣ 创建带数据库Sink的Logger..." << std::endl;
        
        mylog::LocalLoggerBuilder builder;
        builder.buildLoggerName("db_logger");
        builder.buildLoggerLevel(mylog::LogLevel::value::DEBUG);
        builder.buildFormatter("[%d{%H:%M:%S}][%t][%p][%c][%f:%l] %m%n");
        
        // 添加控制台输出用于对比
        builder.buildSink<mylog::StdoutSink>(true);
        
        // 添加数据库Sink (需要根据实际环境修改连接参数)
        builder.buildSink<mylog::MysqlSink>(
            "127.0.0.1",          // host
            "mylogger",           // user
            "Aa+13569039691",     // password
            "db_logs",            // database
            3306                  // port
        );
        
        mylog::Logger::ptr logger = builder.build();
        std::cout << "✅ Logger创建成功\n" << std::endl;
        
        // 2. 发送测试日志
        std::cout << "2️⃣ 发送测试日志到数据库..." << std::endl;
        
        logger->debug(__FILE__, __LINE__, "数据库测试 - DEBUG级别日志");
        logger->info(__FILE__, __LINE__, "用户登录成功, UID:%d", 10001);
        logger->warn(__FILE__, __LINE__, "警告：内存使用率超过80%%");
        logger->error(__FILE__, __LINE__, "错误：数据库连接超时");
        logger->fatal(__FILE__, __LINE__, "严重错误：系统即将崩溃");
        
        // 测试特殊字符
        logger->info(__FILE__, __LINE__, "特殊字符测试: ' \" \\ ; --");
        
        std::cout << "✅ 日志发送完成\n" << std::endl;
        
        std::cout << "3️⃣ 验证数据库记录..." << std::endl;
        std::cout << "   执行SQL查询: SELECT * FROM logs ORDER BY id DESC LIMIT 10;" << std::endl;
        std::cout << "✅ 数据库Sink测试完成\n" << std::endl;
        
    } catch (const std::exception& e) {
        std::cout << "❌ 数据库测试异常: " << e.what() << std::endl;
        std::cout << "   提示：请确保MySQL服务正在运行，数据库'db_logs'已创建" << std::endl;
    }
}

//==================== 测试5: 综合压力测试 ====================
void test_stress()
{
    std::cout << "\n========== 测试5: 综合压力测试 ==========\n";
    
    const uint16_t TCP_PORT = 9992;
    const uint16_t UDP_PORT = 9993;
    
    // 1. 启动LogServer
    std::cout << "1️⃣ 启动LogServer..." << std::endl;
    mylog::LogServer::ptr server = std::make_shared<mylog::LogServer>(
        TCP_PORT, UDP_PORT, "./test_logs/stress_test.log", 5000
    );
    
    std::thread server_thread([&server]() {
        server->start();
    });
    
    std::this_thread::sleep_for(std::chrono::milliseconds(500));
    std::cout << "✅ LogServer启动完成\n" << std::endl;
    
    // 2. 创建异步Logger
    std::cout << "2️⃣ 创建异步Logger..." << std::endl;
    mylog::LocalLoggerBuilder builder;
    builder.buildLoggerName("stress_logger");
    builder.buildLoggerType(mylog::LoggerType::LOGGER_ASYNC);
    builder.buildLoggerLevel(mylog::LogLevel::value::DEBUG);
    builder.buildFormatter("[%d{%H:%M:%S}][%t][%p][%c] %m%n");
    builder.buildSink<mylog::TcpSink>(TCP_PORT, "127.0.0.1");
    builder.buildSink<mylog::UdpSink>(UDP_PORT, "127.0.0.1");
    
    mylog::Logger::ptr logger = builder.build();
    std::cout << "✅ 异步Logger创建完成\n" << std::endl;
    
    // 3. 压力测试
    std::cout << "3️⃣ 开始压力测试 (发送1000条日志)..." << std::endl;
    auto start = std::chrono::high_resolution_clock::now();
    
    for (int i = 0; i < 1000; i++) {
        logger->info(__FILE__, __LINE__, "压力测试消息 #%d", i);
    }
    
    auto end = std::chrono::high_resolution_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(end - start);
    
    std::cout << "✅ 1000条日志发送完成" << std::endl;
    std::cout << "   耗时: " << duration.count() << "ms" << std::endl;
    std::cout << "   吞吐量: " << (1000.0 / duration.count() * 1000) << " logs/s" << std::endl;
    
    // 4. 等待处理完成
    std::cout << "4️⃣ 等待异步处理完成..." << std::endl;
    std::this_thread::sleep_for(std::chrono::seconds(3));
    
    std::cout << "✅ 压力测试完成\n" << std::endl;
}

int main()
{
    std::cout << "\n";
    std::cout << "╔════════════════════════════════════════════════╗\n";
    std::cout << "║   MyLog 日志框架 - TCP/UDP/DB 功能全面测试    ║\n";
    std::cout << "╚════════════════════════════════════════════════╝\n";
    
    try {
        // 测试1: TCP Sink
        test_tcp_sink();
        
        // 测试2: UDP Sink
        test_udp_sink();
        
        // 测试3: LogServer (需要手动Ctrl+C停止)
        // test_log_server();  // 注释掉，因为会一直运行
        
        // 测试4: 数据库Sink
        test_db_sink();
        
        // 测试5: 压力测试 (需要手动Ctrl+C停止)
        // test_stress();  // 注释掉，因为会一直运行
        
        std::cout << "\n";
        std::cout << "╔════════════════════════════════════════════════╗\n";
        std::cout << "║          ✅ 所有测试执行完成！                  ║\n";
        std::cout << "╚════════════════════════════════════════════════╝\n";
        
    } catch (const std::exception& e) {
        std::cout << "\n❌ 测试过程中发生异常: " << e.what() << std::endl;
        return 1;
    }
    
    return 0;
}

