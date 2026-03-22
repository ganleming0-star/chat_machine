//
// Created by 15455 on 2026/3/19.
//

#ifndef MYSQL_TEST_USERMODEL_HPP
#define MYSQL_TEST_USERMODEL_HPP
#include "user.hpp"
//操作user库
class UserModel {
public:
    bool insert(User& user);

    User query(int id);

    bool updateState(User user);

    void resetState();
};

#endif //MYSQL_TEST_USERMODEL_HPP