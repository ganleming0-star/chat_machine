//
// Created by 15455 on 2026/3/21.
//

#ifndef MYSQL_TEST_OFFLINEMESSAGEMODEL_HPP
#define MYSQL_TEST_OFFLINEMESSAGEMODEL_HPP
#include <string>
#include <vector>


class OfflineMsgModel {
public:
    //存储用户离线消息
    void insert(int userid,std::string msg);
    //删除用户消息
    void remove(int userid);
    //查询用户消息
    std::vector<std::string> query(int userid);
    
};

#endif //MYSQL_TEST_OFFLINEMESSAGEMODEL_HPP