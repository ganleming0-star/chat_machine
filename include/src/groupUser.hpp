//
// Created by 15455 on 2026/3/21.
//

#ifndef MYSQL_TEST_GROUPUSER_HPP
#define MYSQL_TEST_GROUPUSER_HPP
#include "user.hpp"

class GroupUser:public User {
public:
    void setRole(string role){this->role = role;}
    string getRole() {
        return role;
    }


private:
    string role;
};

#endif //MYSQL_TEST_GROUPUSER_HPP