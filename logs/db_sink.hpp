/*
    数据库落地类;
    1.继承自LogSink基类;
    2.符合开闭原则  
 */
#ifndef __M_DB_SINK_H__
#define __M_DB_SINK_H__
 #include "sink.hpp"
 #include <mysql/mysql.h>
 #include <iostream>
namespace mylog
{  
    class MysqlSink : public mylog::LogSink
{
public:
    struct LogParts
    {
        std::string level;
        std::string file;
        std::string logger;
        std::string payload;
        int line;
    };
    using ptr=std::shared_ptr<MysqlSink>;
    MysqlSink(const std::string&host,const std::string&user,const std::string&password,const std::string&database,int port)
    :_host(host),_user(user),_password(password),_database(database),_port(port)
    {
        _conn = mysql_init(nullptr);
        connect_mysql();
        prepare_stmt(); // 初始化预处理语句
    }
    ~MysqlSink()
    {
        if(_stmt)
        {
            mysql_stmt_close(_stmt);
        }
        if(_conn)
        {
            ::mysql_close(_conn);
        }
    }
    //将data数据写入到logs表;
    virtual void log(const char*data,size_t len)
    {
        //写入logs表; 
        try {
            parse_log_line(data, len);
            
            MYSQL_BIND bind[5];
            memset(bind, 0, sizeof(bind));

            // 1. Level
            unsigned long level_len = _parts.level.length();
            bind[0].buffer_type = MYSQL_TYPE_STRING;
            bind[0].buffer = (char*)_parts.level.data();
            bind[0].buffer_length = level_len;
            bind[0].length = &level_len;

            // 2. Line
            bind[1].buffer_type = MYSQL_TYPE_LONG;
            bind[1].buffer = (char*)&_parts.line;

            // 3. File
            unsigned long file_len = _parts.file.length();
            bind[2].buffer_type = MYSQL_TYPE_STRING;
            bind[2].buffer = (char*)_parts.file.c_str();
            bind[2].buffer_length = file_len;
            bind[2].length = &file_len;

            // 4. Logger
            unsigned long logger_len = _parts.logger.length();
            bind[3].buffer_type = MYSQL_TYPE_STRING;
            bind[3].buffer = (char*)_parts.logger.c_str();
            bind[3].buffer_length = logger_len;
            bind[3].length = &logger_len;

            // 5. Payload
            unsigned long payload_len = _parts.payload.length();
            bind[4].buffer_type = MYSQL_TYPE_BLOB;
            bind[4].buffer = (char*)_parts.payload.c_str();
            bind[4].buffer_length = payload_len;
            bind[4].length = &payload_len;

            if (mysql_stmt_bind_param(_stmt, bind) != 0) {
                std::cerr << "mysql_stmt_bind_param failed: " << mysql_stmt_error(_stmt) << std::endl;
                return;
            }

            if (mysql_stmt_execute(_stmt) != 0) {
                std::cerr << "mysql_stmt_execute failed: " << mysql_stmt_error(_stmt) << std::endl;
            }
        } catch (const std::exception& e) {
            std::cerr << "Log processing error: " << e.what() << std::endl;
        }
    }
private:
    //解析日志行中的括号内容;
    static std::string_view next_bracket_token(std::string_view s, size_t& pos)
    {
        size_t l = s.find('[', pos);
        if (l == std::string_view::npos) throw std::runtime_error("bad log: no '['");
        size_t r = s.find(']', l + 1);
        if (r == std::string_view::npos) throw std::runtime_error("bad log: no ']'");
        pos = r + 1;
        return s.substr(l + 1, r - (l + 1));
    }
    //解析日志行;
    void  parse_log_line(const char* data, size_t len) 
    {
        std::string_view s(data, len);
        size_t pos = 0;
    
        (void)next_bracket_token(s, pos); // time
        (void)next_bracket_token(s, pos); // tid
        auto t_level  = next_bracket_token(s, pos);
        auto t_logger = next_bracket_token(s, pos);
        auto t_fileln = next_bracket_token(s, pos);
    
        auto colon = t_fileln.rfind(':');
        if (colon == std::string_view::npos) throw std::runtime_error("bad log: file:line");
        std::string file = (colon == std::string_view::npos) ? "unknown" : std::string(t_fileln.substr(0, colon));
        int line = (colon == std::string_view::npos) ? 0 : std::stoi(std::string(t_fileln.substr(colon + 1)));
    
        std::string payload;
        size_t first = payload.find_first_not_of(" \t\r\n");
        if (first != std::string::npos) payload = payload.substr(first);
        _parts = {
            std::string(t_level),
            std::move(file),
            std::string(t_logger),
            std::move(payload),
            line
        };
    }
    //准备预处理语句;
    void prepare_stmt()
    {
        const char* sql = "INSERT INTO logs(time, level, line, file, logger, payload) VALUES (NOW(), ?, ?, ?, ?, ?)";
        _stmt = mysql_stmt_init(_conn);
        if (!_stmt) throw std::runtime_error("mysql_stmt_init failed");
        if (mysql_stmt_prepare(_stmt, sql, strlen(sql)) != 0) {
            throw std::runtime_error("mysql_stmt_prepare failed: " + std::string(mysql_stmt_error(_stmt)));
        }
    }
    //连接数据库;
    void connect_mysql()
    {
        if(!mysql_real_connect(_conn,_host.c_str(),_user.c_str(),_password.c_str(),_database.c_str(),_port,nullptr,_clientflag))
        {
            std::cout<<"mysql_real_connect failed,code:"<<mysql_errno(_conn)<<" "<<mysql_error(_conn)<<std::endl;
            throw std::runtime_error("mysql_real_connect failed: " + std::string(mysql_error(_conn)));
        }
        mysql_set_character_set(_conn,"utf8mb4");//设置字符集;

        //从LogMsg中跟据日志信息创建对应表；   time,level,line,tid,file,logger,payload
        const char*sql=R"SQL(create table if not exists logs(
            id int primary key auto_increment,
            time datetime,
            level varchar(10),
            line int,
            file varchar(255),
            logger varchar(255),
            payload text
        )ENGINE=InnoDB DEFAULT CHARSET=utf8mb4;
        )SQL";
        if(mysql_query(_conn,sql)!=0)
        {
            throw std::runtime_error("mysql_query failed");
        }
    }
private:
    MYSQL*_conn=nullptr;
    std::string _host;//数据库主机;
    std::string _user;//数据库用户名;
    std::string _password;//数据库密码;
    std::string _database;//数据库名称;
    int _port;//数据库端口;
    std::string _unix_socket="";//数据库unix套接字;
    unsigned long _clientflag=0;//数据库客户端标志;
    struct LogParts _parts;//日志部分;
    MYSQL_STMT*_stmt=nullptr;//数据库语句;
};
}
#endif