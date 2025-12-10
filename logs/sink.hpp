/*日志落地模块实现
   1.抽象落地基类
   2.派生子类(根据不同的落地方向进行派生)
   3.使用工厂模式进行创建与表示的芬利;
*/
#ifndef __M_SINK_H__
#define __M_SINK_H__
#include"util.hpp"
#include"message.hpp"
#include"format.hpp"
#include<iostream>
#include<memory>
#include<mutex>
#include <fstream> 
#include <string>   
namespace mylog
{
    class LogSink
    {
    public:
        using ptr=std::shared_ptr<LogSink>;
        virtual void log(const char*data,size_t len)=0;
        virtual ~ LogSink() {};
    };

    //1.将日志写入标准输出;
    class StdoutSink : public LogSink
    {
    public:
        using ptr=std::shared_ptr<StdoutSink>;
        StdoutSink()=default;
        virtual void log(const char*data,size_t len)
        {   
            std::cout.write(data,len);
        }
    };

    //2.将日志写入指定文件;
    class FileSink : public LogSink
    {
    public:
        using ptr=std::shared_ptr<FileSink>;
        FileSink(const std::string& pathname)
        :_pathname(pathname)
        {
            //创建目录;
            util::File::createDirectory(util::File::path(_pathname));
            //打开文件;
            _ofs.open(_pathname,std::ios::binary | std::ios::app);
            assert(_ofs.is_open());
        }
        const std::string&file()
        {
            return _pathname;
        }
        virtual void log(const char*data,size_t len)
        {
            _ofs.write(data,len);
            if(_ofs.good()==false)
            {
                std::cout<<"文件输出失败"<<std::endl;
            }
        }
    private:
        std::string _pathname;
        std::ofstream  _ofs;
    };

    //3.滚动文件(以大小进行滚动)
    class RollSink : public LogSink
    {
    public:
        using ptr=std::shared_ptr<RollSink>;
        RollSink(const std::string basename,size_t max_size)
        :_basename(basename),
         _max_size(max_size),
         _cur_size(0)
         {
            util::File::createDirectory(util::File::path(_basename));
         }
        virtual void log(const char*data,size_t len)
        {
             initLogFile();
             //打开成功,开始写入;
             _ofs.write(data,len);
             if(_ofs.good()==false)
             {
                std::cout<<"文件写入失败"<<std::endl;
             }
             _cur_size+=len;
        }
    private:
        void initLogFile()
        {
            if(_ofs.is_open()==false || _cur_size>=_max_size)
            {
                _ofs.close();
                //创建新的文件;
                std::string name=createNewFile();
                _ofs.open(name, std::ios::binary | std::ios::app);
                assert(_ofs.is_open());
                _cur_size=0;
            }
        }
        //进行大小判断，超过指定大小则创建新的文件
        std::string createNewFile()
        {
            time_t t=time(nullptr);
            struct tm lt;
            localtime_r(&t,&lt);
            std::stringstream ss;
            ss << _basename;
            ss << lt.tm_year + 1900;
            ss << lt.tm_mon + 1;
            ss << lt.tm_mday;
            ss << lt.tm_hour;
            ss << lt.tm_min;
            ss << lt.tm_sec;
            ss << ".log";
            return ss.str();
        }
private:
        size_t _max_size;
        size_t _cur_size;
        std::string _basename;
        std::ofstream _ofs;
    };
    class SinkFactory
    {
    public:
       template<class SinkType,class ...Args>
       static LogSink::ptr create(Args&&...args)
       {
          return std::make_shared<SinkType>(std::forward<Args>(args)...);
       }
    };
}
#endif