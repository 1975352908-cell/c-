/*日志落地模块实现
   1.抽象落地基类
   2.派生子类(根据不同的落地方向进行派生)
   3.使用工厂模式进行创建与表示的芬利;
*/
#include"util.hpp"
#include<memory>
#include<cstido>
#include<iostream>
#include<string>
#include<sstream>
#include<cassert>
namespace mylog
{
    class LogSink
    {
    public:
        using ptr=std::make_shared<LogSink>;
        virtual void log(const char*data,size_t len)=0;
        virtual ~LogSink() {};
    }

    //1.将日志写入保准输出;
    class StdoutSink : public LogSink
    {
    public:
        virtual void log(const char*data,size_t len)
        {
            std::cout.write(data,len);
        }
    }

    //2.将日志写入指定文件;
    class FileSink : public LogSink
    {
    public:
        FileSink(std::string pathname)
        :_pathname(pathname)
        {
            //创建目录;
            util::File::createDirectory(util::File::path(_pathname));
            //打开文件;
            _ofs.open(_pathname,std::ios::binary | std::ios::app);
            assert(_ofs_open());
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
    }

    //3.滚动文件(以大小进行滚动)
    class RollSink : public LogSink
    {
    public:
        virtual void log(const char*data,size_t len);
    private:
        //进行大小判断，超过指定大小则创建新的文件
        void createNewFile()
        {
            
        }
        size_t max_size;
        size_t cur_size;
        std::string _pathname;
        std::ofstream _ofs;
    }
}