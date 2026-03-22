//
// Created by 15455 on 2026/3/21.
//

#ifndef MYSQL_TEST_GROUP_HPP
#define MYSQL_TEST_GROUP_HPP
#include "groupUser.hpp"
#include <vector>
class Group {
public:
    Group(int id = -1,string name = "",string desc = "") {
        this->id = id;
        this->name = std::move(name);
        this->desc = std::move(desc);
    }

    void setId(int id) {
        this->id = id;
    }
    int getId() {
        return this->id;
    }
    void setName(string name) {
        this->name = std::move(name);
    }
    string getName() {
        return this->name;
    }
    void setDesc(string desc) {
        this->desc = std::move(desc);
    }
    string getDesc() {
        return this->desc;
    }
    vector<GroupUser> &getUsers() {
        return this->users;
    }
private:
    int id;
    string name;
    string desc;
    std::vector<GroupUser> users;
};



#endif //MYSQL_TEST_GROUP_HPP
