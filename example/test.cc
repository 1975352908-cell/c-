#include"../logs/mylog.h"
#include"../logs/log_server.hpp"
#include"../logs/sink.hpp"
#include <thread>
#include <chrono>
#include <atomic>
#include"../logs/db_sink.hpp"
//util.cpp单元测试;
//#include"util.hpp"
// int main()
// {
//     std::cout<<mylog::util::Date::now()<<std::endl;
//     std::string pathname="./abc/bcd/a.txt";
//     mylog::util::File::createDirectory(mylog::util::File::path(pathname));
//     return 0;
// }




//#include"level.hpp"
// int main()
// {
//     std::cout<<mylog::LogLevel::ToString(mylog::LogLevel::value::DEBUG)<<std::endl;
//     std::cout<<mylog::LogLevel::ToString(mylog::LogLevel::value::INFO)<<std::endl;
//     std::cout<<mylog::LogLevel::ToString(mylog::LogLevel::value::WORN)<<std::endl;
//     std::cout<<mylog::LogLevel::ToString(mylog::LogLevel::value::ERROR)<<std::endl;
//     std::cout<<mylog::LogLevel::ToString(mylog::LogLevel::value::FATAL)<<std::endl;
//     std::cout<<mylog::LogLevel::ToString(mylog::LogLevel::value::UNKNOW)<<std::endl;

//     return 0;
// }


// int main()
// {
//     mylog::LogMsg msg(
//         mylog::LogLevel::value::DEBUG,
//         30,
//         "main.cc",
//         "root",
//         "功能测试"
//     );

//     mylog::Formatter fmt;  // 用默认模式 "[%d{%H:%M:%S}][%t][%p][%c][%f:%l] %m%n"

//     std::string str = fmt.format(msg);
//     std::cout << str;

//     return 0;
// }

//测试sink落地模块;
// int main()
// {
//     mylog::LogMsg msg(mylog::LogLevel::value::INFO,53,"main.c","root","格式化功能测试...");
//     mylog::Formatter fmt;
//     std::string str=fmt.format(msg);
//     mylog::LogSink::ptr stdout_lsp=mylog::SinkFactory::create<mylog::StdoutSink>();
//     mylog::LogSink::ptr file_lsp=mylog::SinkFactory::create<mylog::FileSink>("./logs/test.file");
//     mylog::LogSink::ptr roll_lsp=mylog::SinkFactory::create<mylog::RollSink>("./logs/roll-",1024*1024);
//     //输出日志;
//     stdout_lsp->log(str.c_str(),str.size());
//     file_lsp->log(str.c_str(),str.size());

//     size_t cur_size=0;
//     size_t count=0;
//     while(cur_size<1024*1024*10)
//     {
//         std::string tmp=str+ std::to_string(count++);
//         roll_lsp->log(tmp.c_str(),tmp.size());
//         cur_size+=tmp.size();
//     }
    
//     return 0;
// }


////////////////////////////测试logger模块;
// #include "util.hpp"
// #include "level.hpp"
// #include "message.hpp"
// #include "format.hpp"
// #include"sink.hpp"
// #include <iostream>
// #include "logger.hpp"
// int main()
// {
//     // 创建输出目标：输出到标准输出(控制台)
//     std::vector<mylog::LogSink::ptr> sinks;
//     sinks.push_back(mylog::SinkFactory::create<mylog::StdoutSink>());
    
//     mylog::Logger::ptr logger=std::make_shared<mylog::SyncLogger>("root",mylog::LogLevel::value::DEBUG,mylog::Formatter::ptr(new mylog::Formatter()),sinks);
//     logger->debug("main.cc",30,"功能测试");
//     logger->info("main.cc",30,"功能测试");
//     logger->warn("main.cc",30,"功能测试");
//     logger->error("main.cc",30,"功能测试");
//     logger->fatal("main.cc",30,"功能测试");
//     return 0;
// }

// #include <iostream>
// #include "logger.hpp"
// #include "buffer.hpp"
// #include "looper.hpp"
// #include "sink.hpp"
// void Test_logger()
// {
//     std::unique_ptr<mylog::LoggerBuilder> builder=std::make_unique<mylog::LocalLoggerBuilder>();
//     builder->buildLoggerName("root");
//     mylog::Logger::ptr logger=builder->build();
//     logger->debug("main.cc",30,"功能测试");
// }
// void Test_buffer()
// {
//     //读入文件数据，一点一点写入缓冲区，最终将缓冲区数据写入文件，保证写入文件的正确性;
//     std::ifstream ifs("/root/work-wjk/logs/test.txt",std::ios::binary);
//     if(ifs.is_open()==false)
//     {
//         std::cout<<"文件打开失败"<<std::endl;
//         return;
//     }
//     ifs.seekg(0,std::ios::end);
//     size_t file_size=ifs.tellg();
//     ifs.seekg(0,std::ios::beg);
//     std::string body;
//     body.resize(file_size);
//     ifs.read(&body[0],file_size);
//     if(ifs.good()==false)
//     {
//         std::cout<<"文件读取失败"<<std::endl;
//         return;
//     }
//     ifs.close();

//     mylog::Buffer buffer;
//     for(size_t i=0;i<body.size();i++)
//     {
//         buffer.push(&body[i],1);
//     }
//     std::ofstream ofs("/root/work-wjk/logs/test.txt.bak");
//     //ofs.write(buffer.begin(),buffer.readAbleSize());
//     size_t sz=buffer.readAbleSize();
//     for(size_t i=0;i<sz;i++)
//     {
//         ofs.write(buffer.begin(),1);
//         buffer.moveReader(1);
//     }
//     ofs.close();
// }
// void Test_looper()
// {
//     // mylog::AsyncLooper::ptr looper=std::make_shared<mylog::AsyncLooper>();
//     // looper->push("hello",5);
//     // looper->push("world",5);
//     // looper->push("hello",5);
//     // looper->push("world",5);
//     // looper->stop();
// }
// void Test_AsyncLogger()
// {
//     // std::unique_ptr<mylog::LoggerBuilder> builder(new mylog::LocalLoggerBuilder());
//     // builder->buildLoggerName("async_logger");
//     // builder->buildFormatter("%m%n");
//     // builder->buildSink<mylog::StdoutSink>();
//     // builder->buildLoggerType(mylog::LoggerType::LOGGER_ASYNC);  // 关键：设置为异步日志器
//     // builder->buildEnableUnsafeAsync();  // 可选：启用不安全模式
//     // mylog::Logger::ptr logger = builder->build();
    
//     // logger->debug("main.cc", 30, "异步日志测试");
    
//     // std::this_thread::sleep_for(std::chrono::milliseconds(100));
//     // std::cout << "Test_AsyncLogger 完成" << std::endl;

//     std::unique_ptr<mylog::LoggerBuilder> builder=std::make_unique<mylog::LocalLoggerBuilder>();
//     builder->buildLoggerName("async_logger");
//     builder->buildFormatter("[%c]%m%n");
//     builder->buildLoggerType(mylog::LoggerType::LOGGER_ASYNC);
//     builder->buildLoggerLevel(mylog::LogLevel::value::WORN);
//     builder->buildSink<mylog::StdoutSink>();
//     builder->buildSink<mylog::FileSink>("./logs/async_logger.log");
//     mylog::Logger::ptr logger=builder->build();
//     int count=10000;
//     for(int i=0;i<count;i++)
//     {
//         logger->fatal("async_logger.cc",20,"异步日志测试%d",i);
//     }
// }

// void test_log()
// {
//     mylog::Logger::ptr logger=mylog::LoggerManager::getInstance().getLogger("async_logger");
//     int count=10000;
//     for(int i=0;i<count;i++)
//     {
//         logger->fatal("async_logger.cc",20,"异步日志测试%d",i);
//     }
// }


// // ==================== 测试2: 控制台颜色输出 ====================
// void test_colored_stdout()
// {
//     std::cout << "\n========== 测试2: 控制台颜色输出 ==========\n";

//     // 创建带颜色的 stdout sink
//     mylog::LocalLoggerBuilder builder;
//     builder.buildLoggerName("color_logger");
//     builder.buildLoggerLevel(mylog::LogLevel::value::DEBUG);
//     auto logger = builder.build();

//     logger->debug("DEBUG - 青色 (Cyan)");
//     logger->info("INFO - 绿色 (Green)");
//     logger->warn("WARN - 黄色 (Yellow)");
//     logger->error("ERROR - 红色 (Red)");
//     logger->fatal("FATAL - 品红色 (Magenta)");

//     std::cout << "✅ 颜色输出测试完成\n";
// }
// //====================测试9: 日志等级过滤 ====================
// void test_level_filter()
// {
//     std::cout << "\n========== 测试9: 日志等级过滤 ==========\n";

//     mylog::LocalLoggerBuilder builder;
//     builder.buildLoggerName("level_filter_logger");
//     builder.buildLoggerLevel(mylog::LogLevel::value::WORN);  // 只输出 WARN 及以上
//     builder.buildSink<mylog::StdoutSink>(true);
//     auto logger = builder.build();

//     std::cout << "设置最低等级为 WARN,以下应只显示 WARN/ERROR/FATAL:\n";
//     logger->debug("DEBUG - 不应显示");
//     logger->info("INFO - 不应显示");
//     logger->warn("WARN - 应该显示");
//     logger->error("ERROR - 应该显示");
//     logger->fatal("FATAL - 应该显示");

//     std::cout << "✅ 日志等级过滤测试完成\n";
// }
// //====================测试10: 多 Sink 输出 ====================
// void test_multi_sink()
// {
//     std::cout << "\n========== 测试10: 多 Sink 输出 ==========\n";

//     mylog::LocalLoggerBuilder builder;
//     builder.buildLoggerName("multi_sink_logger");
//     builder.buildLoggerLevel(mylog::LogLevel::value::DEBUG);
//     builder.buildSink<mylog::StdoutSink>(true);                          // 控制台
//     builder.buildSink<mylog::FileSink>("./test_logs/multi_sink.log");    // 文件
//     auto logger = builder.build();

//     logger->info("这条日志同时输出到控制台和文件");
//     logger->error("错误日志也同时输出到两处");

//     std::cout << "✅ 多 Sink 输出测试完成\n";
// }

// //====================测试11: TCP/UDP Sink 基本功能测试 ====================
// void test_tcp_udp_logging()
// {
//     std::cout << "\n========== 测试11: TCP/UDP Sink 基本功能测试 ==========\n";

//     // 创建测试目录
//     mylog::util::File::createDirectory(mylog::util::File::path("./test_logs"));

//     const uint16_t TEST_PORT = 9990;

//     std::cout << "测试TCP和UDP Sink的基本构造和发送功能..." << std::endl;

//     // 测试TCP Sink构造
//     std::cout << "1. 创建TCP Sink..." << std::endl;
//     try {
//         mylog::LogSink::ptr tcp_sink = mylog::SinkFactory::create<mylog::TcpSink>(TEST_PORT, "127.0.0.1");
//         std::cout << "   ✅ TCP Sink创建成功" << std::endl;

//         // 测试发送数据
//         std::string tcp_test_msg = "[INFO][2024-12-18 12:00:00][test] TCP测试消息\n";
//         tcp_sink->log(tcp_test_msg.c_str(), tcp_test_msg.size());
//         std::cout << "   ✅ TCP数据发送完成" << std::endl;
//     } catch (const std::exception& e) {
//         std::cout << "   ❌ TCP Sink错误: " << e.what() << std::endl;
//     }

//     // 测试UDP Sink构造
//     std::cout << "2. 创建UDP Sink..." << std::endl;
//     try {
//         mylog::LogSink::ptr udp_sink = mylog::SinkFactory::create<mylog::UdpSink>(TEST_PORT, "127.0.0.1");
//         std::cout << "   ✅ UDP Sink创建成功" << std::endl;

//         // 测试发送数据
//         std::string udp_test_msg = "[INFO][2024-12-18 12:00:00][test] UDP测试消息\n";
//         udp_sink->log(udp_test_msg.c_str(), udp_test_msg.size());
//         std::cout << "   ✅ UDP数据发送完成" << std::endl;
//     } catch (const std::exception& e) {
//         std::cout << "   ❌ UDP Sink错误: " << e.what() << std::endl;
//     }

//     std::cout << "✅ TCP/UDP Sink基本功能测试完成" << std::endl;
//     std::cout << "注意：如果没有服务器运行在端口" << TEST_PORT << "，连接会失败但不会崩溃" << std::endl;
// }

// //====================测试12: 单独测试TCP客户端 ====================
// void test_tcp_client_only()
// {
//     std::cout << "\n========== 测试12: TCP客户端单独测试 ==========\n";

//     const uint16_t TEST_PORT = 8888;
//     std::atomic<bool> server_running{true};

//     // 启动简单的TCP服务器用于测试
//     std::thread server_thread([&]() {
//         int server_fd = socket(AF_INET, SOCK_STREAM, 0);
//         if (server_fd < 0) {
//             std::cout << "服务器socket创建失败" << std::endl;
//             return;
//         }

//         int opt = 1;
//         setsockopt(server_fd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt));

//         struct sockaddr_in addr;
//         memset(&addr, 0, sizeof(addr));
//         addr.sin_family = AF_INET;
//         addr.sin_port = htons(TEST_PORT);
//         addr.sin_addr.s_addr = INADDR_ANY;

//         if (bind(server_fd, (struct sockaddr*)&addr, sizeof(addr)) < 0) {
//             std::cout << "服务器bind失败" << std::endl;
//             close(server_fd);
//             return;
//         }

//         listen(server_fd, 5);
//         std::cout << "TCP测试服务器启动在端口 " << TEST_PORT << std::endl;

//         int client_fd = accept(server_fd, nullptr, nullptr);
//         if (client_fd >= 0) {
//             char buffer[1024];
//             ssize_t n = recv(client_fd, buffer, sizeof(buffer), 0);
//             if (n > 0) {
//                 std::cout << "服务器收到TCP数据: " << std::string(buffer, n) << std::endl;
//             }
//             close(client_fd);
//         }
//         close(server_fd);
//     });

//     std::this_thread::sleep_for(std::chrono::milliseconds(200));

//     // 测试TCP客户端
//     std::cout << "测试TCP客户端连接..." << std::endl;
//     mylog::LogSink::ptr tcp_sink = mylog::SinkFactory::create<mylog::TcpSink>(TEST_PORT, "127.0.0.1");

//     std::string test_msg = "TCP客户端测试消息\n";
//     tcp_sink->log(test_msg.c_str(), test_msg.size());

//     std::this_thread::sleep_for(std::chrono::milliseconds(500));
//     server_thread.join();

//     std::cout << "✅ TCP客户端单独测试完成" << std::endl;
// }

// //====================测试13: 单独测试UDP客户端 ====================
// void test_udp_client_only()
// {
//     std::cout << "\n========== 测试13: UDP客户端单独测试 ==========\n";

//     const uint16_t TEST_PORT = 7777;
//     std::atomic<bool> server_running{true};

//     // 启动简单的UDP服务器用于测试
//     std::thread server_thread([&]() {
//         int server_fd = socket(AF_INET, SOCK_DGRAM, 0);
//         if (server_fd < 0) {
//             std::cout << "UDP服务器socket创建失败" << std::endl;
//             return;
//         }

//         struct sockaddr_in addr;
//         memset(&addr, 0, sizeof(addr));
//         addr.sin_family = AF_INET;
//         addr.sin_port = htons(TEST_PORT);
//         addr.sin_addr.s_addr = INADDR_ANY;

//         if (bind(server_fd, (struct sockaddr*)&addr, sizeof(addr)) < 0) {
//             std::cout << "UDP服务器bind失败" << std::endl;
//             close(server_fd);
//             return;
//         }

//         std::cout << "UDP测试服务器启动在端口 " << TEST_PORT << std::endl;

//         char buffer[1024];
//         struct sockaddr_in client_addr;
//         socklen_t addr_len = sizeof(client_addr);

//         ssize_t n = recvfrom(server_fd, buffer, sizeof(buffer), 0,
//                            (struct sockaddr*)&client_addr, &addr_len);
//         if (n > 0) {
//             std::cout << "服务器收到UDP数据: " << std::string(buffer, n) << std::endl;
//         }
//         close(server_fd);
//     });

//     std::this_thread::sleep_for(std::chrono::milliseconds(200));

//     // 测试UDP客户端
//     std::cout << "测试UDP客户端发送..." << std::endl;
//     mylog::LogSink::ptr udp_sink = mylog::SinkFactory::create<mylog::UdpSink>(TEST_PORT, "127.0.0.1");

//     std::string test_msg = "UDP客户端测试消息\n";
//     udp_sink->log(test_msg.c_str(), test_msg.size());

//     std::this_thread::sleep_for(std::chrono::milliseconds(500));
//     server_thread.join();

//     std::cout << "✅ UDP客户端单独测试完成" << std::endl;
// }

// #include"../logs/logger.hpp"
// #include"../logs/sink.hpp"
// #include"../logs/log_server.hpp"
// void test()
// {

//     mylog::LogServer::ptr server=std::make_shared<mylog::LogServer>(9990,9991,"./test_logs/data.log",1000);
//     //启动日志服务器
//     std::thread server_thread([&]()
//     {
//         server->start();
//     });
//     // 2. 等待服务器启动完成
//     std::this_thread::sleep_for(std::chrono::milliseconds(500));
//     std::cout << "✅ 服务器启动完成\n" << std::endl;
//         // 3. 创建 TCP 客户端并发送消息
//         std::cout << "📡 测试 TCP 连接..." << std::endl;
//         mylog::LogSink::ptr tcp_sink = mylog::SinkFactory::create<mylog::TcpSink>(
//             9990,           // 端口
//             "127.0.0.1"     // IP地址
//         );
//     // 发送多条测试消息
//     for (int i = 0; i < 5; i++) {
//         std::string msg = "[TCP消息 " + std::to_string(i) + "] 这是一条DEBUG级别的日志";
//         tcp_sink->log(msg.c_str(), msg.size());
//         std::cout << "  发送TCP消息 " << i << std::endl;
//     }
    
//     // 4. 创建 UDP 客户端并发送消息
//     std::cout << "\n📡 测试 UDP 连接..." << std::endl;
//     mylog::LogSink::ptr udp_sink = mylog::SinkFactory::create<mylog::UdpSink>(
//         9991,           // 端口
//         "127.0.0.1"     // IP地址
//     );
    
//     // 发送多条测试消息
//     for (int i = 0; i < 5; i++) {
//         std::string msg = "[UDP消息 " + std::to_string(i) + "] 这是一条INFO级别的日志";
//         udp_sink->log(msg.c_str(), msg.size());
//         std::cout << "  发送UDP消息 " << i << std::endl;
//     }
    
//     // 5. 等待服务器处理完所有消息
//     std::this_thread::sleep_for(std::chrono::seconds(2));
    
//     std::cout << "\n✅ 测试完成！请查看 ./test_logs/data.log 文件" << std::endl;
//     std::cout << "⚠️  按 Ctrl+C 退出程序（服务器会持续运行）" << std::endl;
    
//     // 服务器线程会持续运行，直到进程结束
//     server_thread.join();
// }
// void test_with_logger()
// {
//     std::cout << "========== 集成日志器 TCP/UDP 测试 ==========" << std::endl;
    
//     // 1. 启动服务器
//     mylog::LogServer::ptr server = std::make_shared<mylog::LogServer>(
//         9990, 9991, "./test_logs/data.log", 1000
//     );
    
//     std::thread server_thread([&server]() {
//         server->start();
//     });
    
//     std::this_thread::sleep_for(std::chrono::milliseconds(500));
//     std::cout << "✅ 服务器启动完成\n" << std::endl;
    
//     // 2. 创建一个使用网络Sink的异步日志器
//     std::unique_ptr<mylog::LoggerBuilder> builder = 
//         std::make_unique<mylog::LocalLoggerBuilder>();
    
//     builder->buildLoggerName("network_logger");
//     builder->buildLoggerType(mylog::LoggerType::LOGGER_ASYNC);
//     builder->buildLoggerLevel(mylog::LogLevel::value::DEBUG);
//     builder->buildFormatter("[%d{%H:%M:%S}][%p][%c] %m%n");
    
//     // 添加 TCP Sink
//     builder->buildSink<mylog::TcpSink>(9990, "127.0.0.1");
    
//     // 添加 UDP Sink
//     builder->buildSink<mylog::UdpSink>(9991, "127.0.0.1");
    
//     // 同时输出到控制台方便观察
//     builder->buildSink<mylog::StdoutSink>();
    
//     mylog::Logger::ptr logger = builder->build();
    
//     // 3. 使用日志器发送各种级别的日志
//     std::cout << "📝 开始发送日志..." << std::endl;
    
//     for (int i = 0; i < 10; i++) {
//         logger->debug(__FILE__, __LINE__, "调试消息 #%d", i);
//         logger->info(__FILE__, __LINE__, "信息消息 #%d", i);
//         logger->warn(__FILE__, __LINE__, "警告消息 #%d", i);
//         logger->error(__FILE__, __LINE__, "错误消息 #%d", i);
        
//         std::this_thread::sleep_for(std::chrono::milliseconds(100));
//     }
    
//     std::cout << "\n✅ 日志发送完成！等待服务器处理..." << std::endl;
//     std::this_thread::sleep_for(std::chrono::seconds(2));
    
//     std::cout << "✅ 测试完成！查看 ./test_logs/data.log" << std::endl;
    
//     server_thread.join();
// }

// void test_debug()
// {
//     std::cout << "========== UDP 调试测试 ==========" << std::endl;
    
//     // 1. 启动服务器
//     std::cout << "1️⃣ 创建 LogServer..." << std::endl;
//     mylog::LogServer::ptr server = std::make_shared<mylog::LogServer>(
//         9990, 9991, "./test_logs/data.log", 1000
//     );
    
//     std::cout << "2️⃣ 启动服务器线程..." << std::endl;
//     std::thread server_thread([&]() {
//         server->start();
//     });
    
//     std::this_thread::sleep_for(std::chrono::milliseconds(500));
//     std::cout << "✅ 服务器启动完成\n" << std::endl;
    
//     // 2. TCP 测试
//     std::cout << "3️⃣ 测试 TCP 连接..." << std::endl;
//     try {
//         mylog::LogSink::ptr tcp_sink = mylog::SinkFactory::create<mylog::TcpSink>(9990, "127.0.0.1");
        
//         for (int i = 0; i < 3; i++) {
//             std::string msg = "[TCP] 消息 #" + std::to_string(i);
//             tcp_sink->log(msg.c_str(), msg.size());
//             std::cout << "  ✅ TCP 消息 " << i << " 已发送" << std::endl;
//         }
//     } catch (const std::exception& e) {
//         std::cerr << "❌ TCP 测试失败: " << e.what() << std::endl;
//     }
    
//     std::this_thread::sleep_for(std::chrono::milliseconds(500));
    
//     // 3. UDP 测试（详细调试）
//     std::cout << "\n4️⃣ 测试 UDP 连接..." << std::endl;
//     try {
//         std::cout << "  🔹 创建 UdpSink..." << std::endl;
//         mylog::LogSink::ptr udp_sink = mylog::SinkFactory::create<mylog::UdpSink>(9991, "127.0.0.1");
        
//         std::cout << "  🔹 开始发送 UDP 消息..." << std::endl;
//         for (int i = 0; i < 3; i++) {
//             std::string msg = "[UDP] 消息 #" + std::to_string(i);
//             std::cout << "    📤 准备发送: " << msg << std::endl;
            
//             udp_sink->log(msg.c_str(), msg.size());
            
//             std::cout << "    ✅ UDP 消息 " << i << " 调用完成" << std::endl;
//             std::this_thread::sleep_for(std::chrono::milliseconds(100));
//         }
//     } catch (const std::exception& e) {
//         std::cerr << "❌ UDP 测试失败: " << e.what() << std::endl;
//     }
    
//     // 4. 等待处理完成
//     std::cout << "\n5️⃣ 等待服务器处理..." << std::endl;
//     std::this_thread::sleep_for(std::chrono::seconds(2));
    
//     std::cout << "\n✅ 测试完成！查看日志文件:" << std::endl;
//     std::cout << "   cat ./test_logs/data.log" << std::endl;
    
//     // 读取并显示文件内容
//     std::ifstream ifs("./test_logs/data.log");
//     if (ifs.is_open()) {
//         std::cout << "\n📄 文件内容:\n" << std::endl;
//         std::string line;
//         int line_num = 0;
//         while (std::getline(ifs, line)) {
//             std::cout << "  [" << ++line_num << "] " << line << std::endl;
//         }
//         ifs.close();
//     }
    
//     server_thread.join();
// }


void test_db_sink_full_flow()
{
    std::cout << "\n========== 测试14: 数据库落地全流程测试 (预处理语句版) ==========\n";

    try {
        // 1. 初始化构建器
        mylog::LocalLoggerBuilder builder;
        builder.buildLoggerName("db_logger");
        builder.buildLoggerLevel(mylog::LogLevel::value::DEBUG);
        
        // 2. 设置格式器
        // 关键点：MysqlSink 的解析逻辑硬编码了 [%d][%t][%p][%c][%f:%l] 这种结构
        // 所以我们必须确保日志器的输出格式与之匹配
        builder.buildFormatter("[%d{%H:%M:%S}][%t][%p][%c][%f:%l] %m%n");

        // 3. 添加输出目标
        builder.buildSink<mylog::StdoutSink>(true); // 控制台输出，用于对比
        
        // 配置数据库参数 (请根据实际情况修改)
        // 这里的 db_logs 数据库必须已存在
        builder.buildSink<mylog::MysqlSink>("localhost", "root", "", "db_logs", 3306);

        // 4. 生成日志器
        mylog::Logger::ptr logger = builder.build();

        // 5. 发送测试日志
        std::cout << "开始写入日志到数据库...\n";
        
        logger->debug(__FILE__, __LINE__, "数据库预处理语句测试 - DEBUG");
        logger->info(__FILE__, __LINE__, "用户登录，UID:%d", 888);
        logger->warn(__FILE__, __LINE__, "警告：检测到非法访问尝试！");
        logger->error(__FILE__, __LINE__, "数据库写入压力测试，内容包含特殊字符: ' \" ; --");
        logger->fatal(__FILE__, __LINE__, "严重系统错误，触发紧急备份");

        std::cout << "✅ 日志指令已下发。\n";
        std::cout << "🔍 请在终端检查数据库: \n";
        std::cout << "   mysql -u root -p -e \"USE db_logs; SELECT * FROM logs ORDER BY id DESC LIMIT 5;\"\n";

    } catch (const std::exception& e) {
        std::cerr << "流程异常: " << e.what() << std::endl;
    }
}

int main()
{
    //Test_logger();
    //Test_buffer();
    //Test_looper();
    //Test_AsyncLogger();
    // std::unique_ptr<mylog::LoggerBuilder> builder=std::make_unique<mylog::GlobalLoggerBuilder>();
    // builder->buildLoggerName("async_logger");
    // builder->buildFormatter("[%c]%m%n");
    // builder->buildLoggerType(mylog::LoggerType::LOGGER_ASYNC);
    // builder->buildLoggerLevel(mylog::LogLevel::value::WORN);
    // builder->buildSink<mylog::StdoutSink>();
    // builder->buildSink<mylog::FileSink>("./logs/async_logger.log");
    // mylog::Logger::ptr logger=builder->build();






    
    // test_colored_stdout();
    // test_level_filter();
    // test_multi_sink();
    //test_log();

    // TCP/UDP网络日志测试
     //test_tcp_udp_logging();  
    //test_tcp_client_only();
    //test_udp_client_only();
    //test();
    test_db_sink_full_flow();
    return 0;
}