//
// Created by 15455 on 2026/3/19.
//

#ifndef MYSQL_TEST_DB_HPP
#define MYSQL_TEST_DB_HPP

#include <mysql/mysql.h>
#include <string>
#include "muduo/base/Logging.h"
using namespace std;
// 数据库配置信息
static string server = "192.168.6.129";
static string user = "gan";
static string password = "123456";
static string dbname = "chat_db";
// 数据库操作类
class MySQL
{
public:
    // 初始化数据库连接
    MySQL()
    {
        _conn = mysql_init(nullptr);
    }
    // 释放数据库连接资源
    //这里用UserModel示例，通过UserModel如何对业务层封装底层数据库的操作。代码示例如下：
    ~MySQL()
    {
        if (_conn != nullptr)
            mysql_close(_conn);
    }
    // 连接数据库
    bool connect()
    {
        MYSQL *p = mysql_real_connect(_conn, server.c_str(), user.c_str(),
        password.c_str(), dbname.c_str(), 3306, nullptr, 0);
        if (p != nullptr)
        {
            mysql_query(_conn, "set names gbk");
        }
        return p;
    }
    // 更新操作
    bool update(const string& sql)
    {
        if (mysql_query(_conn, sql.c_str()))
        {
            LOG_INFO << __FILE__ << ":" << __LINE__ << ":"
            << sql << "更新失败! error:" << mysql_error(_conn);
            return false;
        }
        return true;
    }
    // 查询操作
    MYSQL_RES* query(const string& sql)
    {
        if (mysql_query(_conn, sql.c_str()))
        {
            LOG_INFO << __FILE__ << ":" << __LINE__ << ":"
            << sql << "查询失败! error:" << mysql_error(_conn);
            return nullptr;
        }
        return mysql_use_result(_conn);
    }
    MYSQL* getConn() {
        return _conn;
    }
private:
    MYSQL *_conn;
};

#endif //MYSQL_TEST_DB_HPP
