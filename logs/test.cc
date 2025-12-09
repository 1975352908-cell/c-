//util.cpp单元测试;
#include"util.hpp"
// int main()
// {
//     std::cout<<mylog::util::Date::now()<<std::endl;
//     std::string pathname="./abc/bcd/a.txt";
//     mylog::util::File::createDirectory(mylog::util::File::path(pathname));
//     return 0;
// }




#include"level.hpp"
// int main()
// {
//     std::cout<<mylog::LogLevel::ToString(mylog::LogLevel::value::DEBUG)<<std::endl;
//     std::cout<<mylog::LogLevel::ToString(mylog::LogLevel::value::INFO)<<std::endl;
//     std::cout<<mylog::LogLevel::ToString(mylog::LogLevel::value::WORN)<<std::endl;
//     std::cout<<mylog::LogLevel::ToString(mylog::LogLevel::value::ERROR)<<std::endl;
//     std::cout<<mylog::LogLevel::ToString(mylog::LogLevel::value::FATAL)<<std::endl;
//     std::cout<<mylog::LogLevel::ToString(mylog::LogLevel::value::UNKNOW)<<std::endl;

//     return 0;
// }

#include "util.hpp"
#include "level.hpp"
#include "message.hpp"
#include "format.hpp"
#include <iostream>

int main()
{
    mylog::LogMsg msg(
        mylog::LogLevel::value::DEBUG,
        30,
        "main.cc",
        "root",
        "功能测试"
    );

    mylog::Formatter fmt;  // 用默认模式 "[%d{%H:%M:%S}][%t][%p][%c][%f:%l] %m%n"

    std::string str = fmt.format(msg);
    std::cout << str;

    return 0;
}