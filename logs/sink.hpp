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
#include<map>
#include<cstring>
#include <fstream> 
#include <string>   
#include "level.hpp"
#include<sys/socket.h>
#include<netinet/in.h>
#include<arpa/inet.h>
#include<sys/uio.h>
#include<unistd.h>
#include<sys/types.h>
#include<sys/socket.h>
#include<cerrno>
namespace mylog
{
    
    namespace color
    {
        // static std::map<std::string_view,LogLevel::value> color_map=
        // {
        //     {"\033[31m",LogLevel::value::FATAL},//红色
        //     {"\033[32m",LogLevel::value::ERROR},//绿色
        //     {"\033[33m",LogLevel::value::WORN},//黄色
        //     {"\033[34m",LogLevel::value::INFO},//蓝色
        //     {"\033[35m",LogLevel::value::DEBUG},//洋红色
        // case LogLevel::value::FATAL: return "\033[31m";     // 红
        // case LogLevel::value::ERROR: return "\033[31m";     // 红
        // case LogLevel::value::WORN:  return "\033[33m";     // 黄
        // case LogLevel::value::INFO:  return "\033[34m";     // 蓝
        // case LogLevel::value::DEBUG: return "\033[35m";     // 紫
        // };
        static const char* getColor(std::string_view view)
        {
            size_t sp = view.find(' '); // header 和 message 通常用空格分隔
            std::string_view header = (sp == std::string_view::npos) ? view : view.substr(0, sp);
            if(header.find("[FATAL]")!=std::string_view::npos)
            {
                return "\033[31m";
            }
            if(header.find("[ERROR]")!=std::string_view::npos)
            {
                return "\033[31m";
            }
            if(header.find("[WORN]")!=std::string_view::npos)
            {
                return "\033[33m";
            }
            if(header.find("[INFO]")!=std::string_view::npos)
            {
                return "\033[34m";
            }
            if(header.find("[DEBUG]")!=std::string_view::npos)
            {
                return "\033[32m";
            }
            return nullptr;
        }

        static const char* color_end="\033[0m";//颜色复位;

    }
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
        StdoutSink(bool enable_color=true)
        :_enable_color(enable_color)
        {}
        virtual void log(const char*data,size_t len)
        {   
            if(_enable_color==false)
            {
                std::cout.write(data,len);
                return;
            }
            //日志要求输出颜色;
            std::string view(data,len);
            const char*pcolor=color::getColor(view);
            if(pcolor!=nullptr)
            {
                std::cout<<pcolor;
                std::cout.write(data,len);
                std::cout<<color::color_end;
            }
            else
            {
                std::cout.write(data,len);
            }
        }
    private:
        bool _enable_color;
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
    class UdpSink : public LogSink
    {
    private:
        int _fd={-1};//相当于文件描述符;
        struct sockaddr_in _addr;
        int _port;
        std::string _host;
    public:
        using ptr=std::shared_ptr<UdpSink>;
        UdpSink(int port,std::string host)
        :_port(port),_host(host)
        {
            _fd=socket(AF_INET,SOCK_DGRAM,0);
            if(_fd<0)
            {
                std::cerr << "❌ UDP socket 创建失败: " << strerror(errno) << std::endl;
                return;
            }
            memset(&_addr,0,sizeof(_addr));
            _addr.sin_family=AF_INET;
            _addr.sin_port=htons(_port);
            _addr.sin_addr.s_addr=inet_addr(_host.c_str());
                // 改进：检查 IP 地址转换是否成功
            if (inet_pton(AF_INET, _host.c_str(), &_addr.sin_addr) <= 0) 
            {
                std::cerr << "❌ 无效的 IP 地址: " << _host << std::endl;
                close(_fd);
                _fd = -1;
                return;
            }
            std::cout << "✅ UDP 客户端创建成功 → " << _host << ":" << _port << std::endl;
        }
        ~UdpSink()
        {
            if(_fd>=0)
            {
                close(_fd);
            }
        }
        virtual void log(const char*data,size_t len)
        {
            if(_fd<0)
            {
                throw std::runtime_error("socket error");
            }
            //判断是否要换行;
            bool need_newline;
            if(len==0 || data[len-1]!='\n')
            {
                need_newline=true;
            }
            else
            {
                need_newline=false;
            }


            // 分散读写;struct iovec 类似与c++17的string_view
            struct iovec iov[2];
            //数据;
            iov[0].iov_base=const_cast<char*>(data);                   //                  struct iovec
            iov[0].iov_len=len;                                        //                   {     void *iov_base;	/* Pointer to data.  */          size_t iov_len;	/* Length of data.  */ }
            //换行;                                                     //  
            char newline='\n';                                          //
            iov[1].iov_base=&newline;                                   //
            iov[1].iov_len=(need_newline?1:0);                          //
            /*
            struct msghdr
            {
                void *msg_name;    //目标地址;
                socklen_t msg_namelen; //目标地址长度;
                struct iovec *msg_iov; //分散读写;
                size_t msg_iovlen; //分散读写长度;
                void *msg_control; //控制信息;
                size_t msg_controllen; //控制信息长度;
            };
            */
            struct msghdr msg;
            msg.msg_name=&_addr;
            msg.msg_namelen=sizeof(_addr);
            msg.msg_iov=iov;
            msg.msg_iovlen = need_newline ? 2 : 1;//分散读写长度
            msg.msg_control=nullptr;
             /*               bug修复，未完全初始化                 */
            msg.msg_controllen = 0;
            ssize_t n = sendmsg(_fd,&msg,0);
            if(n < 0)
            {
                std::cerr << "sendmsg error: " << strerror(errno) << std::endl;
            }
        }
    };

    //tcp客户端;
    class TcpSink : public LogSink
    {
    private:
        int _fd{-1};
        int _port;
        std::string _host;
    private:
        bool connectServer()
        {
            //关闭旧连接;
            if(_fd>=0)
            {
                close(_fd);
            }
            //创建新连接;
            _fd=socket(AF_INET,SOCK_STREAM,0);

            if(_fd<0)
            {
                return false;
            }
            struct sockaddr_in addr;
            std::memset(&addr,0,sizeof(addr));
            addr.sin_family=AF_INET;
            addr.sin_port=htons(_port);
            addr.sin_addr.s_addr=inet_addr(_host.c_str());
            if (::inet_pton(AF_INET, _host.c_str(), &addr.sin_addr) <= 0)//将主机地址转换为网络地址
            {
                ::close(_fd);
                _fd = -1;//关闭套接字
                return false;
            }
            //连接服务端;
            if(::connect(_fd,(struct sockaddr*)&addr,sizeof(addr))<0)
            {
                close(_fd); 
                _fd=-1;
                return false;
            }
            return true;
        }
    public:
        using ptr=std::shared_ptr<TcpSink>;
        TcpSink(int port,std::string host)
        :_port(port),_host(host)
        {
            connectServer();
        }
        ~TcpSink()
        {
            if(_fd>=0)
            {
                close(_fd);
            }
        }
        virtual void log(const char*data,size_t len)
        {
            if(_fd<0 && connectServer()==false)
            {
                return;
            }
            //连接成功,开始写入;
            bool need_newline;
            if(len==0 || data[len-1]!='\n')
            {
                need_newline=true;
            }
            else
            {
                need_newline=false;         
            }
            std::string payload(data,len);
            if(need_newline)
            {
                payload+='\n';
            }
            ssize_t n = ::send(_fd, payload.data(), payload.size(), MSG_NOSIGNAL);
            if (n < 0)
            {
                if (connectServer())
                {
                    (void)::send(_fd, payload.data(), payload.size(), MSG_NOSIGNAL);
                }
            }
        }
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
