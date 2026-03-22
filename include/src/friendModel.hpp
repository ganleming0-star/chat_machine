//
// Created by 15455 on 2026/3/21.
//

#ifndef MYSQL_TEST_FRIENDMODEL_HPP
#define MYSQL_TEST_FRIENDMODEL_HPP
#include <user.hpp>
#include <vector>

class FriendModel {
public:
    //添加好友消息
    void insert(int id,int friendId);
    //查询
    std::vector<User> query(int userid);
};

#endif //MYSQL_TEST_FRIENDMODEL_HPP