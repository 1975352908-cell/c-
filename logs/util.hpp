#ifndef __M_UTIL_H__
#define __M_UTIL_H__
/*实用工具类的实现
  1.获取系统时间
  2.判断文件是否存在
  3.获取文件所在路径
  4.创建目录;
*/
#include<string>
#include<string>
#include<unistd.h>
#include<fcntl.h>
#include<sys/stat.h>
#include<time.h>
namespace mylog
{
    namespace util
    {
        class Date
        {
        public:
            //获取系统时间;
            static size_t now()
            {
                return (size_t) time(nullptr);
            }
        };
        class File
        {
        public:
            //断文件是否存在
            static bool exist(const std::string pathname)
            {
                struct stat st;
                if(stat(pathname.c_str(),&st)<0)
                {
                    return false;
                }
                return true;
            }

            //获取文件所在路径
            static std::string path(const std::string pathname)
            {
                if(pathname.empty()) return ".";
                //找到最后一个分割符;
                size_t pos=pathname.find_last_of("/\\");
                if(pos==std::string::npos)
                {
                    return ".";
                }
                return pathname.substr(0,pos+1);
            }
            
            //创建目录
            static void createDirectory(const std::string pathname)
            {
                if(pathname.empty()) return;
                size_t pos = 0;
                while(pos < pathname.size())
                {
                    size_t sep = pathname.find_first_of("/\\", pos);
                    std::string parent_dir = pathname.substr(0, (sep == std::string::npos) ? pathname.size() : sep + 1);
                    if(!parent_dir.empty() && exist(parent_dir)==false)
                    {
                        mkdir(parent_dir.c_str(),0777);
                    }
                    if(sep == std::string::npos) break;
                    pos = sep + 1;
                }
            } 
        };
    }
}
#endif
