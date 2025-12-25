/*
    简化封装头文件 - 一键使用日志系统
    
    使用方法：
    1. 最简单用法（零配置，直接用）:
        #include "easylog.h"
        LOG_DEBUG("hello %s", "world");
    
    2. 从配置文件初始化（支持远程落地）:
        #include "easylog.h"
        mylog::init("mylog.conf");  // 一行搞定
        auto logger = mylog::get("remote_logger");
        logger->info("发送到远程服务器");
    
    3. 如需 MySQL 支持，在包含本文件前定义：
        #define MYLOG_ENABLE_MYSQL
        #include "mylog.h"
*/
#ifndef __M_EASYLOG_H__
#define __M_EASYLOG_H__

#include "logger.hpp"
#include "config.hpp"
#include "sink.hpp"
#include <string>
#include <memory>

// 仅在启用 MySQL 时才包含
#ifdef MYLOG_ENABLE_MYSQL
#include "db_sink.hpp"
#endif

namespace mylog
{
    // ============ 从配置文件初始化所有日志器 ============
    inline bool init(const std::string& config_file)
    {
        // 1. 加载配置文件
        if (!ConfigManager::getInstance().loadConfig(config_file))
        {
            std::cerr << "❌ 配置文件加载失败: " << config_file << std::endl;
            return false;
        }
        
        // 2. 遍历所有 logger 配置，创建对应的日志器
        auto& loggers_cfg = ConfigManager::getInstance().loggers();
        for (auto& pair : loggers_cfg)
        {
            const std::string& name = pair.first;
            LoggerConfig::ptr cfg = pair.second;
            
            // 创建 builder
            std::unique_ptr<GlobalLoggerBuilder> builder(new GlobalLoggerBuilder());
            builder->buildLoggerName(cfg->name());
            builder->buildLoggerLevel(cfg->level());
            builder->buildFormatter(cfg->formatter());
            
            // 设置日志器类型
            if (cfg->type() == LoggerConfig::LoggerType::ASYNC)
            {
                builder->buildLoggerType(LoggerType::LOGGER_ASYNC);
            }
            
            // 添加所有 sink
            for (auto& sink_pair : cfg->sinks())
            {
                SinkConfig::ptr sink_cfg = sink_pair.second;
                
                switch (sink_cfg->type())
                {
                case SinkConfig::Type::STDOUT:
                {
                    auto sc = std::dynamic_pointer_cast<StdoutSinkConfig>(sink_cfg);
                    builder->buildSink<StdoutSink>(sc->colored());
                    break;
                }
                case SinkConfig::Type::FILE:
                {
                    auto fc = std::dynamic_pointer_cast<FileSinkConfig>(sink_cfg);
                    builder->buildSink<FileSink>(fc->path());
                    break;
                }
                case SinkConfig::Type::TCP:
                {
                    auto tc = std::dynamic_pointer_cast<TcpSinkConfig>(sink_cfg);
                    builder->buildSink<TcpSink>(tc->port(), tc->host());
                    std::cout << "✅ TCP Sink → " << tc->host() << ":" << tc->port() << std::endl;
                    break;
                }
                case SinkConfig::Type::UDP:
                {
                    auto uc = std::dynamic_pointer_cast<UdpSinkConfig>(sink_cfg);
                    builder->buildSink<UdpSink>(uc->port(), uc->host());
                    std::cout << "✅ UDP Sink → " << uc->host() << ":" << uc->port() << std::endl;
                    break;
                }
                case SinkConfig::Type::MYSQL:
                {
#ifdef MYLOG_ENABLE_MYSQL
                    auto mc = std::dynamic_pointer_cast<MysqlSinkConfig>(sink_cfg);
                    // MysqlSink 参数顺序: host, user, password, database, port
                    builder->buildSink<MysqlSink>(mc->host(), mc->user(), mc->password(), mc->database(), mc->port());
#else
                    std::cerr << "⚠️  MySQL Sink 需要定义 MYLOG_ENABLE_MYSQL 并链接 MySQL 库" << std::endl;
#endif
                    break;
                }
                default:
                    break;
                }
            }
            
            // 构建日志器
            builder->build();
            std::cout << "✅ 日志器 [" << cfg->name() << "] 已创建" << std::endl;
        }
        
        return true;
    }
    
    // ============ 快捷获取日志器 ============
    inline Logger::ptr get(const std::string& name)
    {
        return LoggerManager::getInstance().getLogger(name);
    }
    
    inline Logger::ptr root()
    {
        return LoggerManager::getInstance().rootLogger();
    }
}

// ============ 零配置宏：直接用默认日志器输出 ============
#define LOG_DEBUG(fmt, ...) mylog::root()->debug(__FILE__, __LINE__, fmt, ##__VA_ARGS__)
#define LOG_INFO(fmt, ...)  mylog::root()->info(__FILE__, __LINE__, fmt, ##__VA_ARGS__)
#define LOG_WARN(fmt, ...)  mylog::root()->warn(__FILE__, __LINE__, fmt, ##__VA_ARGS__)
#define LOG_ERROR(fmt, ...) mylog::root()->error(__FILE__, __LINE__, fmt, ##__VA_ARGS__)
#define LOG_FATAL(fmt, ...) mylog::root()->fatal(__FILE__, __LINE__, fmt, ##__VA_ARGS__)

// ============ 指定日志器的宏：用于远程日志等场景 ============
#define LOGGER_DEBUG(logger, fmt, ...) (logger)->debug(__FILE__, __LINE__, fmt, ##__VA_ARGS__)
#define LOGGER_INFO(logger, fmt, ...)  (logger)->info(__FILE__, __LINE__, fmt, ##__VA_ARGS__)
#define LOGGER_WARN(logger, fmt, ...)  (logger)->warn(__FILE__, __LINE__, fmt, ##__VA_ARGS__)
#define LOGGER_ERROR(logger, fmt, ...) (logger)->error(__FILE__, __LINE__, fmt, ##__VA_ARGS__)
#define LOGGER_FATAL(logger, fmt, ...) (logger)->fatal(__FILE__, __LINE__, fmt, ##__VA_ARGS__)

#endif

