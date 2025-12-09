#include<iostream>
#include<mysql.h>
int main()
{
    MYSQL* coon=mysql_init(nullptr);
    if(coon==nullptr)
    {
        std::cout<<"mysql_init failed"<<std::endl;
        return -1;
    }
    char*host="localhost";
    char*user="root";
    char*password="111111";
    char*db="test";
    unsigned int port=3306;
    if(mysql_real_connect(coon,host,user,password,db,port,nullptr,0))
    {
        std::cout<<"mysql_real_connect failed"<<std::endl;
        return -1;
    }
    std::cout<<"mysql_real_connect success"<<std::endl;

    mysql_close(coon);
    return 0;
}
// #include<iostream>
// #include<mysql.h>
// int main()
// {
//     MYSQL* coon=mysql_init(nullptr);
//     if(coon==nullptr)
//     {
//         std::cout<<"mysql_init failed"<<std::endl;
//         return -1;
//     }
//     char*host="localhost";
//     char*user="root";
//     char*password="111111";
//     char*db="test";
//     unsigned int port=3306;
//     if(mysql_real_connect(coon,host,user,password,db,port,nullptr,0))
//     {
//         std::cout<<"mysql_real_connect failed"<<std::endl;
//         return -1;
//     }
//     std::cout<<"mysql_real_connect success"<<std::endl;

//     mysql_close(coon);
//     return 0;
// }

#include<iostream>
#include<mysql.h>
int main()
{
    MYSQL*coon=mysql_init(nullptr);
    if(coon==nullptr)
    {
        std::cout<<"mysql_init failed"<<std::endl;
        return -1;
    }
    //连接数据库;
    if(!mysql_real_connect(coon,"localhost","root","111111","test",3306,nullptr,0))
    {
        std::cout << "mysql_real_connect failed: "
        << mysql_error(coon) << std::endl;
        return -1;
    }
    // 执行sql语句;
    const char*sql="select *from users where id=4";
    int flag=mysql_query(coon,sql);
    if(flag!=0)
    {
        std::cout<<"mysql_query failed"<<std::endl;
        return -1;
    }
    //从服务器到客户端，获取结果集;
    MYSQL_RES*res=mysql_store_result(coon);
    if(res==nullptr)
    {
        std::cout<<"mysql_store_result failed"<<mysql_error(coon)<<std::endl;
        return -1;
    }
    //获取结果
    MYSQL_ROW row=mysql_fetch_row(res);
    if(res==nullptr)
    {
        std::cout<<"mysql_fetch_row failed"<<mysql_error(coon)<<std::endl;
        return -1;
    }
    return 0;
<<<<<<< HEAD
}
=======
}
>>>>>>> 86e72353f7898e3b0d0b72984686710854dc186b
