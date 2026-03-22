//
// Created by 15455 on 2026/3/19.
//

#ifndef MYSQL_TEST_USER_HPP
#define MYSQL_TEST_USER_HPP

#include <string>
#include <utility>
using namespace std;

class User {
public:
    User(int id = -1,string name = "",string pwd = "",string state = "offline") {
        this->id = id;
        this->name = std::move(name);
        this->pwd = std::move(pwd);
        this->state = std::move(state);
    }

    void setId(int n_id){this->id = n_id;}
    int getId(){return this->id;}
    void setName(string n_name){this->name = n_name;}
    string getName(){return this->name;}
    void setPwd(string n_pwd){this->pwd = n_pwd;}
    string getPwd(){return this->pwd;}
    void setState(string n_state){this->state = n_state;}
    string getState(){return this->state;}



protected:
    int id;
    string name;
    string pwd;
    string state;
};

#endif //MYSQL_TEST_USER_HPP