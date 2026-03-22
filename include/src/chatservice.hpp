//
// Created by 15455 on 2026/3/19.
//

#ifndef MYSQL_TEST_CHATSERVICE_HPP
#define MYSQL_TEST_CHATSERVICE_HPP

#include <muduo/net/TcpConnection.h>
#include <unordered_map>
#include <functional>
#include "json.hpp"
#include "usermodel.hpp"
#include <memory>
#include <mutex>
#include "offlineMessageModel.hpp"
#include "friendModel.hpp"
#include "groupModel.hpp"
#include "redis.hpp"
using namespace muduo;
using namespace  muduo::net;
using json = nlohmann::json;

using MsgHandler = std::function<void(const TcpConnectionPtr &conn,json &js,Timestamp time)>;
//聊天服务器业务类
class ChatService {
public:
    //单例对象接口函数
    static ChatService* instance();
    void login(const TcpConnectionPtr &conn,json &js,Timestamp time);
    //注册业务
    void reg(const TcpConnectionPtr &conn,json &js,Timestamp time);
    //一对一聊天
    void oneChat(const TcpConnectionPtr &conn,json &js,Timestamp time);
    //获取消息对应的处理器
    MsgHandler getHandler(int msgid);
    //删除用户连接
    void clientCloseException(const TcpConnectionPtr &conn);
    //添加好友业务
    void addFriend(const TcpConnectionPtr &conn,json &js,Timestamp time);
    void loginout(const TcpConnectionPtr &conn, json &js, Timestamp time);

    void createGroup(const TcpConnectionPtr &conn, json &js, Timestamp time);

    void addGroup(const TcpConnectionPtr &conn, json &js, Timestamp time);

    void groupChat(const TcpConnectionPtr &conn, json &js, Timestamp time);

    void reset();

    void handleRedisSubscribeMessage(int userid, string msg);

private:
    ChatService();
    bool _isRedisConnected;
    std::unordered_map<int,MsgHandler> _msgHandlerMap;
    //存储用户的连接
    unordered_map<int,TcpConnectionPtr> _userConnMap;
    //数据操作类对象
    UserModel _userModel;
    OfflineMsgModel _offlineMsgModel;
    FriendModel _friendModel;
    GroupModel _groupModel;

    // redis操作对象
    Redis _redis;


    mutex _connMutex;

};

#endif //MYSQL_TEST_CHATSERVICE_HPP
