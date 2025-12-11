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


#include <iostream>
#include "logger.hpp"
#include"buffer.hpp"
#include"looper.hpp"
void Test_logger()
{
    std::unique_ptr<mylog::LoggerBuilder> builder=std::make_unique<mylog::LocalLoggerBuilder>();
    builder->buildLoggerName("root");
    mylog::Logger::ptr logger=builder->build();
    logger->debug("main.cc",30,"功能测试");
}
void Test_buffer()
{
    //读入文件数据，一点一点写入缓冲区，最终将缓冲区数据写入文件，保证写入文件的正确性;
    std::ifstream ifs("/root/work-wjk/logs/test.txt",std::ios::binary);
    if(ifs.is_open()==false)
    {
        std::cout<<"文件打开失败"<<std::endl;
        return;
    }
    ifs.seekg(0,std::ios::end);
    size_t file_size=ifs.tellg();
    ifs.seekg(0,std::ios::beg);
    std::string body;
    body.resize(file_size);
    ifs.read(&body[0],file_size);
    if(ifs.good()==false)
    {
        std::cout<<"文件读取失败"<<std::endl;
        return;
    }
    ifs.close();

    mylog::Buffer buffer;
    for(size_t i=0;i<body.size();i++)
    {
        buffer.push(&body[i],1);
    }
    std::ofstream ofs("/root/work-wjk/logs/test.txt.bak");
    //ofs.write(buffer.begin(),buffer.readAbleSize());
    size_t sz=buffer.readAbleSize();
    for(size_t i=0;i<sz;i++)
    {
        ofs.write(buffer.begin(),1);
        buffer.moveReader(1);
    }
    ofs.close();
}
void Test_looper()
{
    mylog::AsyncLooper::ptr looper=std::make_shared<mylog::AsyncLooper>();
    looper->push("hello",5);
    looper->push("world",5);
    looper->push("hello",5);
    looper->push("world",5);
    looper->stop();
}
int main()
{
    //Test_logger();
    //Test_buffer();
    Test_looper();
    return 0;
}