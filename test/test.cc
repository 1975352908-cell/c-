
#define MYLOG_ENABLE_MYSQL
#include"../logs/mylog.h"
using namespace mylog;
//测试日志打印
void test1()
{
    LOG_INFO("Hello, World!");
    LOG_DEBUG("Hello, World!");
    LOG_WARN("Hello, World!");
    LOG_ERROR("Hello, World!");
    LOG_FATAL("Hello, World!");
}
//测试数据库落地
void test2()
{
    std::shared_ptr<LoggerBuilder> builder=std::make_shared<GlobalLoggerBuilder>();
    //测试异步日志器;
    builder->buildEnableUnsafeAsync();
    builder->buildLoggerType(LoggerType::LOGGER_SYNC);
    builder->buildLoggerName("test");
    builder->buildLoggerLevel(LogLevel::value::DEBUG);
    builder->buildSink<MysqlSink>("localhost","root","","db_logs",3306);
    std::shared_ptr<Logger> logger=builder->build();
    LOGGER_DEBUG(logger,"Hello, World!");
    logger->error(__FILE__,__LINE__,"Hello, World!");
}
//测试配置文件·初始化日志器
void test3()
{
    init("../test/test.conf");
    auto logger=get("db_logger");
    if(logger)
    {
        LOGGER_DEBUG(logger,"这是测试配置文件的日志!");
        logger->error(__FILE__,__LINE__,"这是测试配置文件的错误日志!");
    }
}

int main()
{
    test3();
    return 0;
}