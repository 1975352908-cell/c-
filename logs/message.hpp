/*
   用于封装一条日志对应的所有完整的信息:
   1.时间
   2.日志等级
   3.logger name
   4.文件名和行号
   5.线程id
   6.具体日志内容
*/
#ifndef __M_MESG_H__
#define __M_MESG_H__
#include"level.hpp"
#include"util.hpp"
#include<stdio.h>
#include<string>
#include<thread>
namespace mylog
{
    struct LogMsg
    {
        time_t _ctime; //时间
        LogLevel::value _level;//日志等级
        size_t _line; //日志产生的行号;
        std::thread::id _tid;//对应线程id
        std::string _file;//对应文件名;
        std::string _logger;//对应日志器名称;
        std::string _payload;//有效数据;

        LogMsg(LogLevel::value level,size_t line,std::string file,std::string logger,std::string payload)
        :_ctime(util::Date::now()),
         _level(level),
         _line(line),
         _file(file),
         _logger(logger),
         _payload(payload),
         _tid(std::this_thread::get_id())
         {};
    };
}

#endif