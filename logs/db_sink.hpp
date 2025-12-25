/*
    数据库落地类;
    1.继承自LogSink基类;
    2.符合开闭原则  
 */
#ifndef __M_DB_SINK_H__
#define __M_DB_SINK_H__
 #include "sink.hpp"
#include <cstring>
#include <ctime>
 #include <mysql/mysql.h>
 #include <iostream>
namespace mylog
{
    class MysqlSink : public LogSink
    {
    public:
        struct LogParts
        {
            std::string level;
            std::string logger;
            std::string file;
            std::string line;
            std::string payload;
        };
        MysqlSink(const std::string&host,const std::string&user,const std::string&password,const std::string&database,int port)
        :_host(host),_user(user),_password(password),_database(database),_port(port)
        {
            connect_mysql();
            prepare_stmt();

        }
        virtual ~MysqlSink()
        {
            if(_stmt)
            {
                mysql_stmt_close(_stmt);
            }
            if(_conn)
            {
                mysql_close(_conn);
            }
        }
        virtual void log(const char*data,size_t len)
        {
            std::string_view s(data,len);
            size_t pos=0;
            parse_log_line(s,pos);
            write_log();
        }
    private:
        
        void write_log()
        {
            const char*sql=R"SQL(insert into logs(time,level,line,file,logger,payload) values(NOW(),?,?,?,?,?);)SQL";
            _stmt=mysql_stmt_init(_conn);
            if(mysql_stmt_prepare(_stmt,sql,strlen(sql))!=0)
            {
                std::cout<<"mysql_stmt_prepare failed,code:"<<mysql_errno(_conn)<<" "<<mysql_error(_conn)<<std::endl;
                throw std::runtime_error("mysql_stmt_prepare failed");
            }
            //绑定bind;
            MYSQL_BIND bind[5];
            //初始化bind;
            memset(bind,0,sizeof(bind));

             // 1. Level
            unsigned long level_len = _parts.level.length();
            bind[0].buffer_type = MYSQL_TYPE_STRING;
            bind[0].buffer = (char*)_parts.level.data();
            bind[0].buffer_length = level_len;
            bind[0].length = &level_len;

            // 2. Line
            unsigned long line_len = _parts.line.length();
            bind[1].buffer_type = MYSQL_TYPE_STRING;
            bind[1].buffer = (char*)_parts.line.c_str();
            bind[1].buffer_length = line_len;
            bind[1].length = (unsigned long*)(&line_len);

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

            if(mysql_stmt_bind_param(_stmt,bind)!=0)
            {
                std::cout<<"mysql_stmt_bind_param failed,code:"<<mysql_errno(_conn)<<" "<<mysql_error(_conn)<<std::endl;
                throw std::runtime_error("mysql_stmt_bind_param failed");
            }
            if(mysql_stmt_execute(_stmt)!=0)
            {
                std::cout<<"mysql_stmt_execute failed,code:"<<mysql_errno(_conn)<<" "<<mysql_error(_conn)<<std::endl;
                throw std::runtime_error("mysql_stmt_execute failed");
            }
        }
        //解析日志行;
        void parse_log_line(std::string_view s,size_t&pos)
        {
            next_bracket_token(s,pos);//时间;
            next_bracket_token(s,pos);//线程id;
            auto level=next_bracket_token(s,pos);//日志等级;
            auto logger=next_bracket_token(s,pos);//日志器名称;
            //[file:line]单独处理
            if(s.find(':',pos)!=std::string_view::npos)
            {
                auto file=s.substr(pos+1,s.find(':',pos)-pos);
                auto line=s.substr(s.find(':',pos)+1,s.find(']',pos)-s.find(':',pos)-1);
                pos=s.find(']',pos)+1;
                _parts.file=file;
                _parts.line=line;
            }
            else
            {
                throw std::runtime_error("bad log: no '['");
            }
            //剩余部分为有效数据;
            auto payload=s.substr(pos);
            _parts.level=level;
            _parts.logger=logger;
            _parts.payload=payload;
        }
        //分割提取括号中的内容,并返回括号中的内容;
        static std::string_view next_bracket_token(std::string_view s,size_t&pos)
        {
            size_t l=s.find('[',pos);
            if(l==std::string_view::npos)
            {
                throw std::runtime_error("bad log: no '['");
            }
            size_t r=s.find(']',l+1);
            if(r==std::string_view::npos)
            {
                throw std::runtime_error("bad log: no ']'");
            }
            pos=r+1;
            return s.substr(l+1,r-l-1);
        }
        void connect_mysql()
        {
            _conn=mysql_init(nullptr);
            if(!_conn)
            {
                std::cout<<"mysql_init failed,code:"<<mysql_errno(_conn)<<" "<<mysql_error(_conn)<<std::endl;
                throw std::runtime_error("mysql_init failed");
            }
            if(!mysql_real_connect(_conn,_host.c_str(),_user.c_str(),_password.c_str(),_database.c_str(),_port,nullptr,_clientflag))
            {
                std::cout<<"mysql_real_connect failed,code:"<<mysql_errno(_conn)<<" "<<mysql_error(_conn)<<std::endl;
                throw std::runtime_error("mysql_real_connect failed");
            }
            //设置字符集;
            mysql_set_character_set(_conn,"utf8mb4");

        }
        void prepare_stmt()
        {
            _stmt=mysql_stmt_init(_conn);
            if(!_stmt)
            {
                std::cout<<"mysql_stmt_init failed,code:"<<mysql_errno(_conn)<<" "<<mysql_error(_conn)<<std::endl;
                throw std::runtime_error("mysql_stmt_init failed");
            }
            const char*create_sql=R"SQL(create table if not exists logs(
                id int not null primary key auto_increment,
                time datetime default null,
                level varchar(10) default null,
                line varchar(255) default null,
                file varchar(255) default null,
                logger varchar(255) default null,
                payload text
            )ENGINE=InnoDB DEFAULT CHARSET=utf8mb4;
            )SQL";
            if(mysql_query(_conn,create_sql)!=0)
            {
                std::cout<<"mysql_query failed,code:"<<mysql_errno(_conn)<<" "<<mysql_error(_conn)<<std::endl;
                throw std::runtime_error("mysql_query failed");
            }
            

        }
    private:
        MYSQL*_conn=nullptr;
        MYSQL_STMT* _stmt=nullptr;
        std::string _host="localhost";
        std::string _user="root";
        std::string _password="";
        std::string _database="db_logs";
        int _port=3306;
        std::string _unix_socket="";
        unsigned long _clientflag=0;
        LogParts _parts={};
    };

}
#endif