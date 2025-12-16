/*
   日志格式化类，把日志消息格式化;
   1. %d ⽇期
   2. %T 缩进
   3. %t 线程id
   4. %p ⽇志级别
   5. %c ⽇志器名称
   6. %f ⽂件名
   7. %l ⾏号
   8. %m ⽇志消息
   9. %n 换⾏
  10. %o 其他消息
*/

#ifndef __M_FORMATR_h__
#define __M_FORMATR_h__

#include"util.hpp"
#include"message.hpp"
#include<memory>
#include<ctime>
#include<string>
#include<vector>
#include<sstream>
#include<iostream>
#include<cassert>
namespace mylog
{
    class FormatItem
    {
     public:
        using ptr=std::shared_ptr<FormatItem>;
        virtual ~FormatItem(){}
        virtual void format(std::ostream &out,const LogMsg& mes)=0;//设置成虚函数;
    };
    class TimeFormatItem : public FormatItem
    {
     public:

        TimeFormatItem(const std::string& format="%H:%M:%S")//改变格式的话修改_format的格式即可;
        :_format(format)
        {    
        }
        void format (std::ostream& os,const LogMsg& msg)
        {
            struct tm lt;
            time_t t=msg._ctime;
            localtime_r(&t,&lt);//将时间按照规定的时间格式保存到lt结构体中;
            char tmp[128];
            strftime(tmp,127,_format.c_str(),&lt);
            os<<tmp;
        }
      private:
        std::string _format;
    };


    class TabFormatItem : public FormatItem
    {
     public:
        void format(std::ostream& os,const LogMsg& msg)
        {
            os<<"\t";
        }
    };

    class ThreadFormatItem : public FormatItem
    {
     public:
        void format(std::ostream& os,const LogMsg& msg)
        {
            os<<msg._tid;
        }
    };

    class LevelFormatItem : public FormatItem
    {
     public:
        LevelFormatItem(const std::string &str = ""){(void)str; }
        void format(std::ostream& os,const LogMsg& msg)
        {
            os<<LogLevel::ToString(msg._level);
        }
    };

    //⽇志器名称
    class LoggerFormatItem : public FormatItem
    {
     public:
        void format(std::ostream& os,const LogMsg& msg)
        {
            os<<msg._logger;
        }
    };

    //⽂件名
    class FileFormatItem : public FormatItem
    {
     public:
        void format(std::ostream& os,const LogMsg& msg)
        {
            os<<msg._file;
        }
    };

    //行号
    class LineFormatItem : public FormatItem
    {
     public:
        void format(std::ostream& os,const LogMsg& msg)
        {
            os<<msg._line;
        }
    };

    //日志消息;
    class MsgFormatItem : public FormatItem
    {
     public:
        void format(std::ostream& os,const LogMsg& msg)
        {
            os<<msg._payload;
        }
    };

    //换行;
    class NLineFormatItem : public FormatItem
    {
     public:
        void format(std::ostream& os,const LogMsg& msg)
        {
            os<<"\n";
        }
    };

    //其他;
    class OtherFormatItem : public FormatItem
    {
     public:
        OtherFormatItem(const std::string& str)
        :_str(str)
        {
        }
        void format(std::ostream& os,const LogMsg& msg)
        {
            os<<_str;
        }
      private:
        std::string _str;
    };
    class Formatter
    {
      public:
        using ptr=std::shared_ptr<Formatter>;
        Formatter(const std::string &pattern = "[%d{%H:%M:%S}][%t][%p][%c][%f:%l] %m%n")
        :_pattern(pattern)
        {
            assert(parsePattern());
        }
        const std::string pattern()
        {
            return _pattern;
        }
        std::string format(const LogMsg &msg) 
        {
            std::stringstream ss;
            for (auto &it : _items) 
            {
                it->format(ss, msg);
            }
            return ss.str();
        }
        std::ostream& format(std::ostream &os, const LogMsg &msg) 
        {
            for (auto &it : _items) 
            {
                it->format(os, msg);
            }
            return os;
        }
        bool parsePattern()
        {
           // [%d{%H:%M:%S}][%t][%p][%c][%f:%l] %m%n
            std::vector<std::pair<std::string, std::string>> fmt_order;//存储格式化项,第一个是占位符,第二个是子格式;
            std::string literal;  // 普通字符串
            size_t pos = 0;

            while (pos < _pattern.size())
            {
                // 普通字符
                if (_pattern[pos] != '%')
                {
                    literal.push_back(_pattern[pos]);
                    ++pos;
                    continue;   
                }

                // 处理 "%%" -> 输出一个 '%'
                if (pos + 1 < _pattern.size() && _pattern[pos + 1] == '%')
                {
                    literal.push_back('%');
                    pos += 2;
                    continue;   
                }

                // 遇到真正的占位符，先把前面的普通字符串存起来
                if (!literal.empty())
                {
                    fmt_order.emplace_back("", literal);
                    literal.clear();
                }

                // 跳过 '%'
                ++pos;
                if (pos >= _pattern.size())
                {
                    std::cout << "%后面没有格式化字符串!" << std::endl;
                    return false;
                }

                // 取占位符 key（比如 d / t / p / m 等）
                std::string key;//占位符;
                key.push_back(_pattern[pos]);
                ++pos;

                // 看看有没有子格式 {xxx}
                std::string val;
                if (pos < _pattern.size() && _pattern[pos] == '{')
                {
                    ++pos; // 跳过 '{'
                    while (pos < _pattern.size() && _pattern[pos] != '}')
                    {
                        val.push_back(_pattern[pos]);
                        ++pos;
                    }
                    if (pos == _pattern.size())
                    {
                        std::cout << "子规则{}匹配出错！！" << std::endl;
                        return false;
                    }
                    ++pos; // 跳过 '}'
                }

                fmt_order.emplace_back(key, val);
            }

            // 最后可能还残留一段普通字符串
            if (!literal.empty())
            {
                fmt_order.emplace_back("", literal);
            }

            // 根据解析结果初始化 _items
            for (auto &it : fmt_order)
            {
                _items.push_back(createItem(it.first, it.second));
            }

            return true;
        }

    private:
        FormatItem::ptr createItem(const std::string& key,const std::string&val)
        {
            if (key == "m") return std::make_shared<MsgFormatItem>();
            if (key == "p") return std::make_shared<LevelFormatItem>();
            if (key == "c") return std::make_shared<LoggerFormatItem>();
            if (key == "t") return std::make_shared<ThreadFormatItem>();
            if (key == "n") return std::make_shared<NLineFormatItem>();
            if (key == "d") return std::make_shared<TimeFormatItem>();
            if (key == "f") return std::make_shared<FileFormatItem>();
            if (key == "l") return std::make_shared<LineFormatItem>();
            if (key == "T") return std::make_shared<TabFormatItem>();
            if(key=="") return std::make_shared<OtherFormatItem>(val);
            return std::make_shared<OtherFormatItem>("%" + key);
        }
        const std::string _pattern;
        std::vector<FormatItem::ptr> _items;
    };
}
#endif