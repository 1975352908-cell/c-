/*
    数据库落地类;
    1.继承自LogSink基类;
    2.符合开闭原则  
 */

 #include "sink.hpp"
 #include <mysql/mysql.h>
 #include <iostream>
namespace mylog
{
    class MysqlSink : public mylog::LogSink
{
public:
    using ptr=std::shared_ptr<MysqlSink>;
    MysqlSink(const std::string&host,const std::string&user,const std::string&password,const std::string&database,int port)
    :_host(host),_user(user),_password(password),_database(database),_port(port)
    {
        _conn = mysql_init(nullptr);
        connect_mysql();
    }
    ~MysqlSink()
    {
        if(_conn)
        {
            ::mysql_close(_conn);
        }
    }
    //将data数据写入到logs表;
    virtual void log(const char*data,size_t len)
    {
        std::string sql=std::string(data,len);
        if(mysql_query(_conn,sql.c_str())!=0)
        {
            throw std::runtime_error("mysql_query failed");
        }
    }
private:
    void connect_mysql()
    {
        if(!mysql_real_connect(_conn,_host.c_str(),_user.c_str(),_password.c_str(),_database.c_str(),_port,nullptr,_clientflag))
        {
            throw std::runtime_error("mysql_real_connect failed");
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
    std::string _host;
    std::string _user;
    std::string _password;
    std::string _database;
    int _port;
    std::string _unix_socket="";
    unsigned long _clientflag=0;
};
}