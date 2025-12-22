/*
    服务器模块实现
    1.TCP服务器实现
    2.UDP服务器实现
    3.服务器工厂实现
*/
#ifndef __M_SERVER_H__
#define __M_SERVER_H__
#include <cstddef>
#include <cstdio>
#include<iostream>
#include<string>
#include<thread>
#include<mutex>
#include<condition_variable>
#include<queue>
#include<vector>
#include<memory>
#include<functional>
#include<atomic>
#include<deque>
#include<unordered_map>
#include<unistd.h>
#include<sys/types.h>
#include<sys/socket.h>
#include<netinet/in.h>
#include<arpa/inet.h>
#include<cstring>
#include<memory.h>
#include"util.hpp"
#include<fstream>
#include"level.hpp"
namespace mylog
{
    class LogServer
    {
    public:
        LogServer(uint16_t tcp_port = 9010, uint16_t udp_port = 9010, const std::string &store_path = "./server_logs/data.log", size_t keep_lines = 5000)
        :_tcp_port(tcp_port),_udp_port(udp_port),_store_path(store_path),_keep_lines(keep_lines)
        {
            mylog::util::File::createDirectory(mylog::util::File::path(_store_path));
            _ofs.open(_store_path, std::ios::app | std::ios::binary);//打开日志文件
            if (_ofs.is_open() == false)
            {
                throw std::runtime_error("open log file failed");
            }
            initSockets();
        }
        ~LogServer()
        {
            if (_tcp_fd >= 0)
                ::close(_tcp_fd);
            if (_udp_fd >= 0)
                ::close(_udp_fd);
            if (_ofs.is_open())
                _ofs.close();
        }
        void start()
        {
           //tcp
           _tcp_worker=std::thread(std::bind(&LogServer::TcpLopper,this));
           //udp
           _udp_worker=std::thread(std::bind(&LogServer::UdpLopper,this));
           //主线程等待子线程结束;
           _tcp_worker.join();
           _udp_worker.join();
        }
    private:
        void TcpLopper()
        {
            while(1)
            {
                int client_fd=::accept(_tcp_fd,NULL,NULL);
                if(client_fd>=0)
                {
                    //创建子线程处理;
                    std::thread(&LogServer::handleTcpClient, this, client_fd).detach();
                }
            }
        }
        void handleTcpClient(int client_fd)
        {
            std::vector<char> buffer(1024);//缓冲区;
            std::string message;
            //tcp面向字节流，要处理粘包问题;
            while(true)
            {
                ssize_t n=::recv(client_fd, buffer.data(), buffer.size(), 0);
                if(n<=0)
                {
                    ::close(client_fd);
                    return;
                }
                message.append(buffer.data(),n);
                //游客能有多个换行符，所以要有一个循环来处理;
                size_t search_pos = 0;
                while(true)
                {
                    //看看是否读到了换行符;
                    size_t newline_pos=message.find("\n",search_pos);//从pos开始查找换行符;
                    if(newline_pos==std::string::npos)
                    {
                        //保留未完整的一行
                        message.erase(0, search_pos);
                        break;
                    }
                    //写入落地目录并且更新下标;
                    std::string line=message.substr(search_pos,newline_pos-search_pos+1);
                    appendToFile(line);
                    search_pos=newline_pos+1;
                    //如果pos已经大于message.size()，则清空message;
                    if(search_pos>=message.size())
                    {
                        message.clear();
                        break;
                    }
                }
            }
        }
        void UdpLopper()
        {
            std::vector<char> buffer(1024);
            while(true)
            {
                ssize_t n=::recvfrom(_udp_fd,buffer.data(),buffer.size(),0,nullptr,nullptr);
                if(n>=0)
                {
                    //写入对应落地目录;
                    appendToFile(std::string(buffer.data(),n));
                }
            }
        }
        void appendToFile(const std::string& line)
        {   
            std::unique_lock<std::mutex> lock(_mutex);
            _recent.push_back(line);
            //写入落地目录
            _ofs.write(line.data(),line.size());
            _ofs.flush();
            //如果超过keep_lines，则删除最早的日志;
            if(_recent.size()>_keep_lines)
            {
                _recent.pop_front();
            }
            //获取日志等级,插入_level_count中;
            LogLevel::value level=LogLevel::value::UNKNOW;
            if(line.find("DEBUG")!=std::string::npos)
            {
                level=LogLevel::value::DEBUG;
            }
            else if(line.find("INFO")!=std::string::npos)
            {
                level=LogLevel::value::INFO;
            }
            else if(line.find("WORN")!=std::string::npos)
            {
                level=LogLevel::value::WORN;
            }
            else if(line.find("ERROR")!=std::string::npos)
            {
                level=LogLevel::value::ERROR;
            }
            else if(line.find("FATAL")!=std::string::npos)
            {
                level=LogLevel::value::FATAL;
            }
            else
            {
                level=LogLevel::value::UNKNOW;
            }
            _level_count[LogLevel::ToString(level)]++;
        }
        void initSockets()
        {
            //tcp
            _tcp_fd=socket(AF_INET,SOCK_STREAM,0);
            if(_tcp_fd<0)
            {
                throw std::runtime_error("create tcp socket failed");
            }
            int opt = 1;
            ::setsockopt(_tcp_fd,SOL_SOCKET,SO_REUSEADDR,(const void*)& opt ,sizeof(opt));
            struct sockaddr_in tcpaddr;
            socklen_t tcpaddr_len=sizeof(tcpaddr);
            memset(&tcpaddr,0,sizeof(tcpaddr));
            tcpaddr.sin_family=AF_INET;
            tcpaddr.sin_port=htons(_tcp_port);
            tcpaddr.sin_addr.s_addr=INADDR_ANY;
            int n=::bind(_tcp_fd,(struct sockaddr*)&tcpaddr,sizeof(tcpaddr));
            if(n<0)
            {
                throw std::runtime_error("bind tcp socket failed");
            }
            ::listen(_tcp_fd,64);
            std::cout<<"tcp server is running on port "<<_tcp_port<<std::endl;
            //udp
            _udp_fd=socket(AF_INET,SOCK_DGRAM,0);
            if(_udp_fd<0)
            {
                throw std::runtime_error("create udp socket failed");
            }
            struct sockaddr_in udpaddr;
            socklen_t udpaddr_len=sizeof(udpaddr);
            memset(&udpaddr,0,sizeof(udpaddr));
            udpaddr.sin_family=AF_INET;
            udpaddr.sin_port=htons(_udp_port);
            udpaddr.sin_addr.s_addr=INADDR_ANY;
            n=::bind(_udp_fd,(struct sockaddr*)&udpaddr,sizeof(udpaddr));
            if(n<0)
            {
                throw std::runtime_error("bind udp socket failed");
            }
            std::cout<<"udp server is running on port "<<_udp_port<<std::endl;
        }
    private:
        uint16_t _tcp_port;
        uint16_t _udp_port;
        int _tcp_fd;
        int _udp_fd;
        std::string _store_path;
        size_t _keep_lines;
        std::ofstream _ofs;
        std::thread _tcp_worker;
        std::thread _udp_worker;
        std::mutex _mutex;
        std::deque<std::string> _recent;//最近keep_lines条日志;
        std::unordered_map<std::string, size_t> _level_count;//每个等级日志的条数;
    };
}


















#endif
