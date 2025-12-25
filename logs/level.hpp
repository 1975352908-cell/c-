/*
   1.定义枚举类，枚举出日志等级;
   2.提供转换接口，将枚举类型转化为对应的字符串;
*/
#ifndef __M_LEVLE_H__
#define __M_LEVLE_H__
namespace mylog
{
    class LogLevel
    {
     public:
        enum class value
        {
            UNKNOW=0,
            DEBUG,
            INFO,
            WARN,
            ERROR,
            FATAL,
            OFF
        };
        static const char*ToString(LogLevel::value val)
        {
            switch(val)
            {
                case LogLevel::value::DEBUG:  return "DEBUG";
                case LogLevel::value::INFO:   return "INFO";
                case LogLevel::value::WARN:   return "WARN";
                case LogLevel::value::ERROR:  return "ERROR";
                case LogLevel::value::FATAL:  return "FATAL";
                case LogLevel::value::OFF:    return "OFF";
            }
            return "UNKNOW";
        }
    };
}
#endif