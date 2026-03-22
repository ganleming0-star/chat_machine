//
// Created by 15455 on 2026/3/21.
//
#include "offlineMessageModel.hpp"
#include "db.hpp"
//插入
void OfflineMsgModel::insert(int userid, std::string msg) {
    char sql[1024] = {0};
    sprintf(sql, "insert into `OfflineMessage` values('%d','%s')",
        userid,msg.c_str());
    MySQL mysql;

    if (mysql.connect()) {
        mysql.update(sql);
    }


}
// 删除
void OfflineMsgModel::remove(int userid) {
    char sql[1024] = {0};
    sprintf(sql, "delete from `OfflineMessage` where userid=%d ",
        userid);
    MySQL mysql;

    if (mysql.connect()) {
        mysql.update(sql);
    }
}
// 查询
std::vector<std::string> OfflineMsgModel::query(int userid) {
    char sql[1024] = {0};
    sprintf(sql, "select message from `OfflineMessage` where userid=%d ",
        userid);
    MySQL mysql;
    vector<string> vec;

    if (mysql.connect()) {
        MYSQL_RES *res = mysql.query(sql);
        if (res != nullptr) {
            MYSQL_ROW row;
            while ((row = mysql_fetch_row(res))!=nullptr) {
                vec.emplace_back(row[0]);
            }
            mysql_free_result(res);
            return vec;
        }
    }
    return vec;
}
