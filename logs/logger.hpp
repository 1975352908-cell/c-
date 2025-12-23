/*完成日志器模块:
  1.抽象日志器基类
  2.派生出不同的子类(同步日志器类和异步日志器类)
*/
#ifndef __M_LOGGER_H__
#define __M_LOGGER_H__
#include "buffer.hpp"
#include "level.hpp"
#include "message.hpp"
#include "format.hpp"
#include "sink.hpp"
#include"looper.hpp"
#include <functional>
#include<vector>
#include<cstdarg>
#include<mutex>
#include<unordered_map>
#include<atomic>
namespace mylog
{
   enum class LoggerType
   {
       LOGGER_SYNC, //同步日志器
       LOGGER_ASYNC //异步日志器
   };
   class Logger
   {
    public:
       using ptr=std::shared_ptr<Logger>;
       Logger(std::string logger_name,LogLevel::value limit_value,Formatter::ptr fmt,std::vector<LogSink::ptr> sinks)
       :_logger_name(logger_name)
       ,_limit_value(limit_value)
       ,_fmt(fmt)
       ,_sinks(sinks.begin(),sinks.end())
       {}

       std::string loggerName()
       {
          return _logger_name;
       }
       LogLevel::value limitValue()
       {
          return _limit_value;
       }
       //完成日志构造日志消息对象，然后进行格式化，落地输出;

       //需要注意，_limit_value限制了最低的输出等级，实现逻辑要进行判断;
       void debug(const std::string&file,size_t line,const std::string &fmt,...)
       {
           if(LogLevel::value::DEBUG<_limit_value)
           {
              return;   
           }
           //格式化日志消息;
           va_list ap;
           //将可变参数列表转换为字符串;
           va_start(ap,fmt);
           char*res;
           //将可变参数列表转换为字符串,就是要输出的日志内容;
           int ret=vasprintf(&res,fmt.c_str(),ap);
           if(ret==-1)
           {
             std::cout<<"vasprintf failed"<<std::endl;
           }
           //释放资源;
           va_end(ap);
           //序列化日志消息;
           serialize(LogLevel::value::DEBUG,file,line,res);
           //释放资源;
           free(res);
       }
       void info(const std::string&file,size_t line,const std::string &fmt,...)
       {
         if(LogLevel::value::INFO<_limit_value)
         {
            return;
         }
         va_list ap;
         va_start(ap,fmt);
         char*res;
         int ret=vasprintf(&res,fmt.c_str(),ap);
         if(ret==-1)
         {
            std::cout<<"vasprintf failed"<<std::endl;
         }
         va_end(ap);
         serialize(LogLevel::value::INFO,file,line,res);
         free(res);  
       }
       void warn(const std::string&file,size_t line,const std::string &fmt,...)
       {
         if(LogLevel::value::WORN<_limit_value)
         {
            return;
         }
         va_list ap;
         va_start(ap,fmt);
         char*res;
         int ret=vasprintf(&res,fmt.c_str(),ap);
         if(ret==-1)
         {
            std::cout<<"vasprintf failed"<<std::endl;
         }
         va_end(ap);
         serialize(LogLevel::value::WORN,file,line,res);
         free(res);
       }
       void error(const std::string&file,size_t line,const std::string &fmt,...)
       {
         if(LogLevel::value::ERROR<_limit_value)
         {
            return;
         }
         va_list ap;
         va_start(ap,fmt);
         char*res;
         int ret=vasprintf(&res,fmt.c_str(),ap);
         if(ret==-1)
         {
            std::cout<<"vasprintf failed"<<std::endl;
         }
         va_end(ap);
         serialize(LogLevel::value::ERROR,file,line,res);
         free(res);
       }
       void fatal(const std::string&file,size_t line,const std::string &fmt,...)
       {
         if(LogLevel::value::FATAL<_limit_value)
         {
            return;
         }
         va_list ap;
         va_start(ap,fmt);
         char*res;
         int ret=vasprintf(&res,fmt.c_str(),ap);
         if(ret==-1)
         {
            std::cout<<"vasprintf failed"<<std::endl;
         }
         va_end(ap);
         serialize(LogLevel::value::FATAL,file,line,res);
         free(res);
       }
    protected:
       void serialize(LogLevel::value level,const std::string&file,size_t line,char*str)
       {
         //构造日志消息对象;
         LogMsg msg(level,line,file,_logger_name,str);
         //格式化日志消息;
         std::stringstream ss;
         _fmt->format(ss,msg);
         //落地输出;
         log(ss.str().c_str(),ss.str().size());
       }
       //抽象接口的日志实质的落地输出;
       virtual void log(const char*data,size_t len)=0;
    //成员对象;
    protected:
       AsyncType _looper_type;
       std::mutex _mutex;//锁
       std::string _logger_name; //日志器名称
       std::atomic<LogLevel::value> _limit_value;//日志等级
       Formatter::ptr _fmt; //指向格式化类的指针
       std::vector<LogSink::ptr> _sinks;//落地类数组；
    };
    class SyncLogger : public Logger
    {
    public:
       using ptr=std::shared_ptr<SyncLogger>;
       SyncLogger(std::string logger_name,LogLevel::value limit_value,Formatter::ptr fmt,std::vector<LogSink::ptr> sinks)
       :Logger(logger_name,limit_value,fmt,sinks)
       {}
    protected:
       //同步日志器，直接在当前线程中进行输出;
       void log(const char*data,size_t len)
       {
         //加锁，保证线程安全;
         std::unique_lock<std::mutex> lock(_mutex);
         //遍历所有落地类，进行输出;
         for(auto&sink:_sinks)
         {
            sink->log(data,len);
         }  
       }
    };
    class AsyncLogger : public Logger
    {
    public:
       using ptr=std::shared_ptr<AsyncLogger>;
       AsyncLogger(const std::string& logger_name,LogLevel::value limit_value,Formatter::ptr& fmt,std::vector<LogSink::ptr>& sinks,AsyncType looper_type)
       :Logger(logger_name,limit_value,fmt,sinks)
       ,_looper(std::make_shared<AsyncLooper>(std::bind(&AsyncLogger::realLog,this,std::placeholders::_1),looper_type))
       {}
       //实际落地函数(将缓冲区的数据落地)
       void realLog(Buffer&buffer)
       {
         for(auto&sink:_sinks)
         {
            sink->log(buffer.begin(),buffer.readAbleSize());
         }
       }
    protected:
       void log(const char*data,size_t len)
       {
         _looper->push(data,len);
       }
   private:
      AsyncLooper::ptr _looper;
       
    };
    /*
    建造者模式：不让用户去构建日志器对象，而是通过建造者模式来构建日志器对象;
  */ 
   // 1.抽象一个日志建造这类(完成日志器对象零件的构建&日志器的构建)
    //设置日志器类型，将不同的日志器放到同一个builder中建造;
    class LoggerBuilder
    {
     public:
         LoggerBuilder()
         :_logger_type(LoggerType::LOGGER_SYNC)
         ,_looper_type(AsyncType::ASYNC_SAFE)
         ,_level(LogLevel::value::DEBUG)
         {} 
         void buildEnableUnsafeAsync() {_looper_type=AsyncType::ASYNC_UNSAFE;}
         void buildLoggerType(LoggerType type)
         {
            _logger_type=type;
         }
         void buildLoggerName(const std::string& name)
         {
            _logger_name=name;
         }
         void buildLoggerLevel(LogLevel::value level)
         {
            _level=level;
         }
         void buildFormatter(const std::string& pattern)
         {
            _formatter=std::make_shared<Formatter>(pattern);
         }
         template<class SinkType,class ...Args>
         void buildSink(Args&& ...args)
         {
            LogSink::ptr psink=SinkFactory::create<SinkType>(std::forward<Args>(args)...);
            _sinks.push_back(psink);
         }
         virtual Logger::ptr build()=0;
     protected:
         AsyncType _looper_type;
         LoggerType _logger_type;
         std::string _logger_name;
         LogLevel::value _level;
         Formatter::ptr _formatter;
         std::vector<LogSink::ptr> _sinks;
    };
    //2.派生出具体的建造者类：局部日志器的建造者和全局日志器的建造者;
    class LocalLoggerBuilder : public LoggerBuilder
    {
     public:
         virtual Logger::ptr build() override
         {
            if(_logger_name.empty())
            {
               throw std::runtime_error("logger name is empty");
            }
            if(_formatter==nullptr)
            {
                _formatter=std::make_shared<Formatter>();
            }
            if(_sinks.empty())
            {
               buildSink<StdoutSink>(); //默认输出到标准输出;  
            }
            //异步日志器;
            if(_logger_type==LoggerType::LOGGER_ASYNC)
            {
               return std::make_shared<AsyncLogger>(_logger_name,_level,_formatter,_sinks,_looper_type);
            }
            //同步日志器;
            return std::make_shared<SyncLogger>(_logger_name,_level,_formatter,_sinks);
         }
    };

    //单例模式，突破日志器的作用域限制，让日志器在全局范围内可以访问;
    //懒汉模式：程序启动时不实例化，在第一次使用时实例化;
    class LoggerManager
    {
     public:
         static LoggerManager& getInstance()
         {
            //c++11后，静态变量可以保证线程安全;
            static LoggerManager _logger_manager;
            return _logger_manager;
         }
        void addLogger(Logger::ptr logger)
        {
            if(hasLogger(logger->loggerName())) return;

            std::unique_lock<std::mutex> lock(_mutex);
            _loggers.insert({logger->loggerName(),logger});
        }
         bool hasLogger(const std::string& name)
         {
            return _loggers.find(name)!=_loggers.end();
         }
         Logger::ptr getLogger(const std::string& name)
         {
            auto it=_loggers.find(name);
            if(it!=_loggers.end())
            {
                return it->second;
            }
            return nullptr;
         }
         Logger::ptr rootLogger()
         {
            return _root_Logger;
         }
         const std::string GetLoggerName()
         {
            return _root_Logger->loggerName();
         }
         using ptr=std::shared_ptr<LoggerManager>;
     private:
         LoggerManager()
         {
            std::unique_ptr<LocalLoggerBuilder> builder=std::unique_ptr<LocalLoggerBuilder>(new LocalLoggerBuilder());
            builder->buildLoggerName("root");
            _root_Logger=builder->build();
            _loggers.insert(std::make_pair("root",_root_Logger));
         }
     private:
         std::mutex _mutex;
         Logger::ptr _root_Logger; //默认日志器
         std::unordered_map<std::string,Logger::ptr> _loggers;
    };
    
    //全局日志建造者类，用于建造全局日志器;
    class GlobalLoggerBuilder : public LoggerBuilder
    {
      public:
      Logger::ptr build() override
      {
         if(_logger_name.empty())
         {
            throw std::runtime_error("logger name is empty");
         }
         if(_formatter==nullptr)
         {
             _formatter=std::make_shared<Formatter>();
         }
         if(_sinks.empty())
         {
            buildSink<StdoutSink>(); //默认输出到标准输出;  
         }
         Logger::ptr logger=nullptr;
         //异步日志器;
         if(_logger_type==LoggerType::LOGGER_ASYNC)
         {
            logger= std::make_shared<AsyncLogger>(_logger_name,_level,_formatter,_sinks,_looper_type);
         }
         //同步日志器;
         else 
         {
            logger= std::make_shared<SyncLogger>(_logger_name,_level,_formatter,_sinks);
         }
         LoggerManager::getInstance().addLogger(logger);
         return logger;
      }
    };

}   

#endif






































// #ifndef __M_LOG_H__
// #define __M_LOG_H__

// #include "util.hpp"
// #include "level.hpp"
// #include "message.hpp"
// #include "formatter.hpp"
// #include "sink.hpp"
// #include "looper.hpp"

// #include <vector>
// #include <list>
// #include <atomic>
// #include <unordered_map>
// #include <cstdarg>
// #include <type_traits>

// namespace mylog {

// class SyncLogger;
// class AsyncLogger;

// // ============================= Logger 抽象基类 =============================
// // 提供统一的：名字 / 级别 / 格式化 / 多个 sink 输出 / debug/info/... 接口
// // 具体“怎么写到 sink”（同步/异步），交给子类实现 logIt()
// class Logger {
// public:
//     // 日志器类型：同步 or 异步
//     enum class Type {
//         LOGGER_SYNC = 0,
//         LOGGER_ASYNC
//     };

//     using ptr = std::shared_ptr<Logger>; 

//     // 构造函数：
//     //  name      : 日志器名字
//     //  formatter : 日志格式器
//     //  sinks     : 输出端（可以多个）
//     //  level     : 日志级别（低于该级别的日志直接丢弃）
//     Logger(const std::string &name, 
//            Formatter::ptr formatter,
//            std::vector<LogSink::ptr> &sinks, 
//            LogLevel::value level = LogLevel::value::DEBUG)
//         : _name(name)
//         , _level(level)
//         , _formatter(formatter)
//         , _sinks(sinks.begin(), sinks.end())  // 拷贝一份 sink 列表
//     {}

//     // 获取日志器名称
//     std::string loggerName() { return _name; }

//     // 获取当前日志级别
//     LogLevel::value loggerLevel() { return _level; }

//     // 以下是五个“便捷日志接口”，都会：
//     // 1. 判断级别是否需要输出（shouldLog）
//     // 2. 用 va_list 收集可变参数
//     // 3. 调用统一的 log() 做格式化 + 输出

//     void debug(const char *file, size_t line, const char *fmt, ...) {
//         if (shouldLog(LogLevel::value::DEBUG) == false) {
//             return;
//         }
//         va_list al;
//         va_start(al, fmt);
//         log(LogLevel::value::DEBUG, file, line, fmt, al);
//         va_end(al);
//     }

//     void info(const char *file, size_t line, const char *fmt, ...) {
//         if (shouldLog(LogLevel::value::INFO) == false) return;
//         va_list al;
//         va_start(al, fmt);
//         log(LogLevel::value::INFO, file, line, fmt, al);
//         va_end(al);
//     }

//     void warn(const char *file, size_t line, const char *fmt, ...) {
//         if (shouldLog(LogLevel::value::WARN) == false) return;
//         va_list al;
//         va_start(al, fmt);
//         log(LogLevel::value::WARN, file, line, fmt, al);
//         va_end(al);
//     }

//     void error(const char *file, size_t line, const char *fmt, ...) {
//         if (shouldLog(LogLevel::value::ERROR) == false) return;
//         va_list al;
//         va_start(al, fmt);
//         log(LogLevel::value::ERROR, file, line, fmt, al);
//         va_end(al);
//     }

//     void fatal(const char *file, size_t line, const char *fmt, ...) {
//         if (shouldLog(LogLevel::value::FATAL) == false) return;
//         va_list al;
//         va_start(al, fmt);
//         log(LogLevel::value::FATAL, file, line, fmt, al);
//         va_end(al);
//     }

// public:
//     // ============================= Logger 构建器基类 =============================
//     // Builder 用来一步一步配置 logger（名字/级别/类型/格式器/sink）
//     // 最后调用 build() 得到具体 Logger 对象（Sync 或 Async）
//     class Builder {
//     public:
//         using ptr = std::shared_ptr<Builder>;

//         Builder()
//             : _level(LogLevel::value::DEBUG)
//             , _logger_type(Logger::Type::LOGGER_SYNC) 
//         {}

//         // 设置 logger 名字
//         void buildLoggerName(const std::string &name) { _logger_name = name; }

//         // 设置 logger 初始级别
//         void buildLoggerLevel(LogLevel::value level) { _level = level; }

//         // 设置 logger 类型（同步/异步）
//         void buildLoggerType(Logger::Type type) { _logger_type = type; }

//         // 支持直接用 pattern 创建一个 Formatter
//         void buildFormatter(const std::string pattern) {
//             _formatter = std::make_shared<Formatter>(pattern);
//         }

//         // 支持直接传入一个已有的 Formatter 智能指针
//         void buildFormatter(const Formatter::ptr &formatter) { 
//             _formatter = formatter; 
//         }

//         // 配置 sink（任意类型的 LogSink 子类）
//         //   SinkType：StdoutSink / FileSink / RollSink ...
//         //   Args... : 透传给 SinkType 的构造函数
//         template<typename SinkType, typename ...Args>
//         void buildSink(Args &&...args) { 
//             auto sink = SinkFactory::create<SinkType>(std::forward<Args>(args)...);
//             _sinks.push_back(sink); 
//         }

//         // 具体的 build 逻辑由子类实现（因为有 Local / Global 两种）
//         virtual Logger::ptr build() = 0;

//     protected:
//         Logger::Type _logger_type;        // 同步 or 异步
//         std::string  _logger_name;        // logger 名
//         LogLevel::value _level;           // 日志级别
//         Formatter::ptr _formatter;        // 格式器
//         std::vector<LogSink::ptr> _sinks; // 输出端列表
//     };

// protected:
//     // 判断某个 level 的日志是否应该输出
//     bool shouldLog(LogLevel::value level) { return level >= _level; }

//     // 核心日志逻辑：
//     // 1. 用 vasprintf 把 fmt + va_list 变成 std::string
//     // 2. 填充 LogMsg
//     // 3. 用 Formatter 做格式化（写到 stringstream）
//     // 4. 调用虚函数 logIt() 把格式化后的字符串丢给子类处理
//     void log(LogLevel::value level, 
//              const char *file, 
//              size_t line, 
//              const char *fmt, 
//              va_list al) 
//     {
//         char *buf;
//         std::string msg;

//         // 把可变参数根据 fmt 格式化到 buf（堆上分配）
//         int len = vasprintf(&buf, fmt, al);
//         if (len < 0) {
//             msg = "format log message failed!";
//         } else {
//             msg.assign(buf, len); // 拷贝到 std::string
//             free(buf);           // 释放堆内存
//         }

//         // 组装 LogMsg（包含 logger 名、文件名、行号、消息、级别等）
//         LogMsg lm(_name, file, line, std::move(msg), level);

//         // 用 formatter 把 LogMsg 格式化为最终的输出字符串
//         std::stringstream ss;
//         _formatter->format(ss, lm);

//         // 交给子类（同步/异步）具体去写到 sink 里
//         logIt(std::move(ss.str()));
//     }

//     // 纯虚函数：由 SyncLogger/AsyncLogger 实现
//     virtual void logIt(const std::string &msg) = 0;

// protected:
//     std::mutex _mutex;                // 同步 Logger 输出时使用的互斥锁
//     std::string _name;                // logger 名字
//     Formatter::ptr _formatter;        // 格式器
//     std::atomic<LogLevel::value> _level; // 当前日志级别（可原子修改）
//     std::vector<LogSink::ptr> _sinks; // 输出端列表（控制台、文件等）
// };

// // ============================= 同步 Logger =============================
// // 每次 log 时直接在当前线程里写到所有 sink（加锁保护）
// class SyncLogger : public Logger {
// public:
//     using ptr = std::shared_ptr<SyncLogger>;

//     SyncLogger(const std::string &name, 
//                Formatter::ptr formatter,
//                std::vector<LogSink::ptr> &sinks, 
//                LogLevel::value level = LogLevel::value::DEBUG)
//         : Logger(name, formatter, sinks, level)
//     {
//         std::cout << LogLevel::toString(level)
//                   << " sync logger " << name << " created...\n";
//     }

// private:
//     // 同步模式下：直接锁住 _mutex，遍历 _sinks 输出
//     virtual void logIt(const std::string &msg) override {
//         std::unique_lock<std::mutex> lock(_mutex);
//         if (_sinks.empty()) { return; }
//         for (auto &it : _sinks) {
//             it->log(msg.c_str(), msg.size());
//         }
//     }
// };

// // ============================= 异步 Logger =============================
// // 前端线程只负责 push 日志到 AsyncLooper 的队列，
// // 后端线程从 Buffer 中批量取出数据写入 sink。
// class AsyncLogger : public Logger {
// public:
//     using ptr = std::shared_ptr<AsyncLogger>;

//     AsyncLogger(const std::string &name, 
//                 Formatter::ptr formatter, 
//                 std::vector<LogSink::ptr> &sinks, 
//                 LogLevel::value level = LogLevel::value::DEBUG)
//         : Logger(name, formatter, sinks, level)
//         // 创建 AsyncLooper，并绑定后台回调函数 backendLogIt
//         , _looper(std::make_shared<AsyncLooper>(
//             std::bind(&AsyncLogger::backendLogIt, this, std::placeholders::_1)))
//     {
//         std::cout << LogLevel::toString(level)
//                   << " async logger " << name << " created...\n";
//     }

// protected:
//     // 前端 logIt：只需把格式化后的整串日志 msg 塞给 AsyncLooper
//     virtual void logIt(const std::string &msg) override {
//         _looper->push(msg);
//     }

//     // 后端回调：AsyncLooper 调用这个函数，把一批 Buffer 数据交给你处理
//     void backendLogIt(Buffer &msg) {
//         if (_sinks.empty()) { return; }

//         // 这里一次性把 Buffer 中的内容写入每个 sink
//         // 注意：Buffer 里可能是多个日志拼在一起
//         for (auto &it : _sinks) {
//             it->log(msg.begin(), msg.readAbleSize());
//         }
//     }

// protected:
//     AsyncLooper::ptr _looper; // 异步循环器（内部有后台线程）
// };

// // ============================= 本地 Logger 构建器 =============================
// // LocalLoggerBuilder：主要用在 loggerManager 初始化 root logger 时
// class LocalLoggerBuilder: public Logger::Builder {
// public:
//     virtual Logger::ptr build() override {
//         if (_logger_name.empty()) {
//             std::cout << "logger name cannot be empty!\n";
//             abort();
//         }

//         // 默认 Formatter
//         if (_formatter.get() == nullptr) {
//             std::cout << "logger: " << _logger_name
//                       << " formatter not set, use default [ %d{%H:%M:%S}%T%t%T[%p]%T[%c]%T%f:%l%T%m%n ]\n";
//             _formatter = std::make_shared<Formatter>();
//         }

//         // 默认 sink：stdout
//         if (_sinks.empty()) {
//             std::cout << "logger: " << _logger_name
//                       << " has no sinks, default to stdout\n";
//             _sinks.push_back(std::make_shared<StdoutSink>());
//         }

//         Logger::ptr lp;
//         if (_logger_type == Logger::Type::LOGGER_ASYNC) {
//             lp = std::make_shared<AsyncLogger>(_logger_name, _formatter, _sinks, _level);
//         } else {
//             lp = std::make_shared<SyncLogger>(_logger_name, _formatter, _sinks, _level);
//         }
//         return lp;
//     }
// };

// // ============================= loggerManager 单例 =============================
// // 管理所有 logger 的全局容器：
// //  - 内部有一个 root_logger
// //  - 支持按 name 存取 / 判断是否存在
// class loggerManager {
// private:
//     std::mutex _mutex;
//     Logger::ptr _root_logger;
//     std::unordered_map<std::string, Logger::ptr> _loggers;

// private:
//     // 构造函数：创建 root logger
//     loggerManager(){ 
//         std::unique_ptr<LocalLoggerBuilder> slb(new LocalLoggerBuilder());
//         slb->buildLoggerName("root");
//         slb->buildLoggerType(Logger::Type::LOGGER_SYNC);
//         _root_logger = slb->build();
//         _loggers.insert(std::make_pair("root", _root_logger));
//     }

//     // 禁止拷贝与赋值（典型单例写法）
//     loggerManager(const loggerManager&) = delete;
//     loggerManager &operator=(const loggerManager&) = delete;

// public:
//     // 获取单例实例
//     static loggerManager& getInstance() {
//         static loggerManager lm;
//         return lm;
//     }

//     // 判断某个名称的 logger 是否存在
//     bool hasLogger(const std::string &name)  {
//         std::unique_lock<std::mutex> lock(_mutex);
//         auto it = _loggers.find(name);
//         if (it == _loggers.end()) {
//             return false;
//         }
//         return true;
//     }

//     // 添加一个 logger
//     void addLogger(const std::string &name, const Logger::ptr logger) {
//         std::unique_lock<std::mutex> lock(_mutex);
//         _loggers.insert(std::make_pair(name, logger));
//     }

//     // 按名称获取一个 logger（可能返回空指针）
//     Logger::ptr getLogger(const std::string &name) {
//         std::unique_lock<std::mutex> lock(_mutex);
//         auto it = _loggers.find(name);
//         if (it == _loggers.end()) {
//             return Logger::ptr();
//         }
//         return it->second;
//     }

//     // 获取 root logger
//     Logger::ptr rootLogger() {
//         std::unique_lock<std::mutex> lock(_mutex);
//         return _root_logger;
//     }
// };

// // ============================= GlobalLoggerBuilder =============================
// // 用于“全局注册”的 logger 创建：
// // build() 会：
// //   1. 创建 logger（Sync/Async）
// //   2. 把它注册到 loggerManager 中
// class GlobalLoggerBuilder: public Logger::Builder {
// public:
//     virtual Logger::ptr build() override {
//         if (_logger_name.empty()) {
//             std::cout << "logger name cannot be empty!\n";
//             abort();
//         }

//         // 不允许重复名字
//         assert(loggerManager::getInstance().hasLogger(_logger_name) == false);

//         // 默认 Formatter
//         if (_formatter.get() == nullptr) {
//             std::cout << "logger: " << _logger_name
//                       << " formatter not set, use default [ %d{%H:%M:%S}%T%t%T[%p]%T[%c]%T%f:%l%T%m%n ]\n";
//             _formatter = std::make_shared<Formatter>();
//         }

//         // 默认 sink
//         if (_sinks.empty()) {
//             std::cout << "logger: " << _logger_name
//                       << " has no sinks, default to stdout\n";
//             _sinks.push_back(std::make_shared<StdoutSink>());
//         }

//         Logger::ptr lp;
//         if (_logger_type == Logger::Type::LOGGER_ASYNC) {
//             lp = std::make_shared<AsyncLogger>(_logger_name, _formatter, _sinks, _level);
//         } else {
//             lp = std::make_shared<SyncLogger>(_logger_name, _formatter, _sinks, _level);
//         }

//         // 注册到全局 manager
//         loggerManager::getInstance().addLogger(_logger_name, lp);
//         return lp;
//     }
// };

// }

// #endif 