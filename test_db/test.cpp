#include <iostream>
#include <mysql/mysql.h>

int main()
{
    // 初始化连接句柄
    MYSQL* conn = mysql_init(nullptr);
    if (conn == nullptr)
    {
        std::cout << "mysql_init failed" << std::endl;
        return -1;
    }

    // 连接数据库（根据你的实际账号信息修改）
    if (!mysql_real_connect(conn,
                            "localhost",
                            "root",
                            "111111",
                            "test",
                            3306,
                            nullptr,
                            0))
    {
        std::cout << "mysql_real_connect failed: "
                  << mysql_error(conn) << std::endl;
        mysql_close(conn);
        return -1;
    }

    // 执行 SQL 语句
    const char* sql = "select * from users where id = 4";
    int rc = mysql_query(conn, sql);
    if (rc != 0)
    {
        std::cout << "mysql_query failed: "
                  << mysql_error(conn) << std::endl;
        mysql_close(conn);
        return -1;
    }

    // 获取结果集
    MYSQL_RES* res = mysql_store_result(conn);
    if (res == nullptr)
    {
        std::cout << "mysql_store_result failed: "
                  << mysql_error(conn) << std::endl;
        mysql_close(conn);
        return -1;
    }

    // 取一行结果（仅示例）
    MYSQL_ROW row = mysql_fetch_row(res);
    if (row == nullptr)
    {
        std::cout << "mysql_fetch_row failed or no data: "
                  << mysql_error(conn) << std::endl;
        mysql_free_result(res);
        mysql_close(conn);
        return -1;
    }

    // 简单打印第一列（如果有）
    if (row[0] != nullptr)
    {
        std::cout << "first column: " << row[0] << std::endl;
    }

    // 释放资源并关闭连接
    mysql_free_result(res);
    mysql_close(conn);

    return 0;
}


