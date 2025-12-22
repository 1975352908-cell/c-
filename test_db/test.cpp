#include <cinttypes>
#include <iostream>
#include <mysql/field_types.h>
#include <mysql/mysql.h>
#include<string.h>
MYSQL* mysql_connect()
{
    MYSQL*conn=mysql_init(nullptr);
    if(!conn)
    {
        perror("coon faild");
        exit(-1);
    }
    if(!mysql_real_connect(conn, "localhost", "root","", "db_logs", 3306, nullptr, 0))
    {
        perror("connet failed");
        exit(-1);
    }
    mysql_set_character_set(conn, "utf8mb4");
    std::cout<<"连接数据库成功"<<std::endl;
    return conn;
}
//判断sql语句是否执行成功;;
bool check_sql(MYSQL*conn,const char*sql)
{
    if(mysql_query(conn,sql)!=0)
    {
        std::cout<<"mysql_query failed,code:"<<mysql_errno(conn)<<" "<<mysql_error(conn)<<std::endl;
        return false;
    }
    return true;
}
//打印数据;
void print(MYSQL_RES*res)
{
    //打印列：
    int ncols=mysql_num_fields(res);
    MYSQL_FIELD*fields=mysql_fetch_fields(res);
    for(int i=0;i<ncols;i++)
    {
        std::cout<<fields[i].name;
        if (i + 1 < ncols) 
        {
            std::cout<<"\t";
        }
    }
    std::cout<<std::endl;
    // // 打印分隔线
    // for (unsigned int i = 0; i < ncols; ++i) {
    //     std::cout << "--------";
    //     if (i + 1 < ncols) std::cout << "\t";
    // }
    std::cout << "\n";
    //打印数据;
    //行数据可能很大，所以设计成了迭代器风格;
    MYSQL_ROW row;
    while((row=mysql_fetch_row(res))!=nullptr)
    {
        unsigned long*lengths=mysql_fetch_lengths(res);
        for(int i=0;i<ncols;i++)
        {
            if(!row[i])
            {
                std::cout << "NULL";
            }
            else 
            {
                std::cout.write(row[i], lengths[i]);
            }
            if (i + 1 < ncols) std::cout << "\t";
        }
        std::cout << "\n";
    }
}


//练习创建表;
void test_create(MYSQL*conn)
{
    const char*sql=R"SQL(CREATE TABLE IF NOT EXISTS test_stmt(
        name varchar(10),
        age int
    );
    )SQL";
    if(mysql_query(conn,sql)!=0)
    {
        std::cout<<"mysql_query failed,code:"<<mysql_errno(conn)<<" "<<mysql_error(conn)<<std::endl;
        exit(-1);
    }
    std::cout<<"create table success"<<std::endl;
}
//练习删除表;
void test_drop(MYSQL*conn)
{
    const char*sql=R"SQL(DROP TABLE IF EXISTS LOGS;)SQL";
    if(mysql_query(conn,sql)!=0)
    {
        std::cout<<"mysql_query failed,code:"<<mysql_errno(conn)<<" "<<mysql_error(conn)<<std::endl;
        exit(-1);
    }
    std::cout<<"drop table success"<<std::endl;
}
//练习插入数据;;
void test_insert(MYSQL*conn)
{
    const char*sql=R"SQL(insert into logs (level,message) values("DEBUG","test message");)SQL";
    if(mysql_query(conn,sql)!=0)
    {
        std::cout<<"mysql_query failed,code:"<<mysql_errno(conn)<<" "<<mysql_error(conn)<<std::endl;
        exit(-1);
    }
    std::cout<<"insert data success"<<std::endl;
}
//练习查询数据;;
void test_select(MYSQL*conn)
{
    const char*sql=R"SQL(select *from logs where id=1;)SQL";
    if(!check_sql(conn,sql))
    {
        return;
    }
    //获取结果集;
    MYSQL_RES*res=mysql_store_result(conn);
    if(res==nullptr)
    {
        std::cout<<"mysql_store_result failed,code:"<<mysql_errno(conn)<<" "<<mysql_error(conn)<<std::endl;
        return;
    }
    // //获取行数;
    // int num_rows=mysql_num_rows(res);
    // std::cout<<"num_rows:"<<num_rows<<std::endl;
    // //获取列数;
    // int num_fields=mysql_num_fields(res);
    // std::cout<<"num_fields:"<<num_fields<<std::endl;
    // //获取列名;
    // MYSQL_FIELD*fields=mysql_fetch_fields(res);
    // for(int i=0;i<num_fields;i++)
    // {
    //     std::cout<<fields[i].name<<" ";
    // }
    // std::cout<<std::endl;
    // //获取数据;
    // MYSQL_ROW row;
    // while((row=mysql_fetch_row(res))!=nullptr)
    // {
    //     for(int i=0;i<num_fields;i++)
    //     {
    //         std::cout<<row[i]<<" ";
    //     }
    //     std::cout<<std::endl;
    // }
    print(res);
    //释放结果集;
    mysql_free_result(res);
    std::cout<<"select data success"<<std::endl;
}
//练习更新数据;;
void test_update(MYSQL*conn)
{
    //更新表数据
    const char*sql=R"SQL(update logs set message="update message" where id=1;)SQL";
}
//练习删除数据;
void test_delete(MYSQL*conn)
{
    const char*sql=R"SQL(delete from logs where id=1;)SQL";
    if(!check_sql(conn,sql))
    {
        return;
    }
    std::cout<<"delete data success"<<std::endl;
}

//练习mysql的预编译语句;
void test_stmt(MYSQL*conn)
{
    MYSQL_STMT*stmt=mysql_stmt_init(conn);
    if(stmt==nullptr)
    {
        std::cout<<mysql_errno(conn)<<" "<<mysql_error(conn);
        exit(1);
    }
    //初始化成功;
    const char*sql=R"SQL(insert into test (id,message) values(?,?);)SQL";

    if(mysql_stmt_prepare(stmt, sql, strlen(sql))!=0)
    {
        std::cout<<mysql_errno(conn)<<" "<<mysql_error(conn);
        exit(1);
    }
    //开始进行参数绑定：
    MYSQL_BIND bind[2]={};
    int id=1;
    bind[0].buffer_type=MYSQL_TYPE_LONG;
    bind[0].buffer_length=sizeof(id);
    bind[0].buffer=&id;
    //message
    std::string message="测试消息";
    bind[1].buffer_type=MYSQL_TYPE_VAR_STRING;
    bind[1].buffer_length=message.size();
    bind[1].buffer=(void*)message.c_str();
    //绑定参数;
    if(mysql_stmt_bind_param(stmt, bind))
    {
        std::cout<<mysql_errno(conn)<<" "<<mysql_error(conn);
        exit(1);
    }

}
void test_stmt_10()
{
    MYSQL*conn=mysql_connect();
    MYSQL_STMT*stmt=mysql_stmt_init(conn);
    if(stmt==nullptr)
    {
        std::cout<<mysql_errno(conn)<<" "<<mysql_error(conn);
        exit(1);
    }
    const char*sql=R"SQL(insert into test (id,message) values(?,?);)SQL";
    if(mysql_stmt_prepare(stmt,sql,strlen(sql))!=0)
    {
        std::cout<<mysql_errno(conn)<<" "<<mysql_error(conn);
        exit(1);
    }
    //开始进行参数绑定：
    MYSQL_BIND bind[2]={};
    int id=2;
    bind[0].buffer_type=MYSQL_TYPE_LONG;
    bind[0].buffer_length=sizeof(id);
    bind[0].buffer=&id;
    //message
    std::string message="测试消息";
    bind[1].buffer_type=MYSQL_TYPE_VAR_STRING;
    bind[1].buffer_length=message.size();
    bind[1].buffer=(void*)message.c_str();
    //只绑定一次，修改所指向的值即可;
    mysql_stmt_bind_param(stmt,bind);
    //插入十条数据;
    for(int i=0;i<10;i++)
    {
        id+=1;
        message="测试消息"+std::to_string(i+1);
        bind[1].buffer_length=message.size();
        bind[1].buffer=(void*)message.c_str();
        mysql_stmt_bind_param(stmt,bind);
        //执行预编译语句;
        if(mysql_stmt_execute(stmt)!=0)
        {
            std::cout<<mysql_errno(conn)<<" "<<mysql_error(conn);
            exit(1);
        }
    }
    //释放结果集;
    mysql_stmt_close(stmt);
    mysql_close(conn);
    std::cout<<"insert data success"<<std::endl;
}
int main()
{
//     //初始化连接句柄;
//     MYSQL*conn=mysql_connect();
//     //test_create(conn);
//     //test_drop(conn);
//     //test_insert(conn);
//     //test_select(conn);
//     //test_update(conn);
//     //test_delete(conn);
//     test_create(conn);
//     test_stmt(conn);
    test_stmt_10();
    return 0;
}



























// int main()
// {
//     // 初始化连接句柄
//     MYSQL* conn = mysql_init(nullptr);
//     if (conn == nullptr)
//     {
//         std::cout << "mysql_init failed" << std::endl;
//         return -1;
//     }

//     // 连接数据库（根据你的实际账号信息修改）
//     if (!mysql_real_connect(conn,
//                             "localhost",
//                             "root",
//                             "111111",
//                             "test",
//                             3306,
//                             nullptr,
//                             0))
//     {
//         std::cout << "mysql_real_connect failed: "
//                   << mysql_error(conn) << std::endl;
//         mysql_close(conn);
//         return -1;
//     }

//     // 执行 SQL 语句
//     const char* sql = "select * from users where id = 4";
//     int rc = mysql_query(conn, sql);
//     if (rc != 0)
//     {
//         std::cout << "mysql_query failed: "
//                   << mysql_error(conn) << std::endl;
//         mysql_close(conn);
//         return -1;
//     }

//     // 获取结果集
//     MYSQL_RES* res = mysql_store_result(conn);
//     if (res == nullptr)
//     {
//         std::cout << "mysql_store_result failed: "
//                   << mysql_error(conn) << std::endl;
//         mysql_close(conn);
//         return -1;
//     }

//     // 取一行结果（仅示例）
//     MYSQL_ROW row = mysql_fetch_row(res);
//     if (row == nullptr)
//     {
//         std::cout << "mysql_fetch_row failed or no data: "
//                   << mysql_error(conn) << std::endl;
//         mysql_free_result(res);
//         mysql_close(conn);
//         return -1;
//     }

//     // 简单打印第一列（如果有）
//     if (row[0] != nullptr)
//     {
//         std::cout << "first column: " << row[0] << std::endl;
//     }

//     // 释放资源并关闭连接
//     mysql_free_result(res);
//     mysql_close(conn);

//     return 0;
// }

