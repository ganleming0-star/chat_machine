#include <chatservice.hpp>
#include "public.hpp"
#include <muduo/base/Logging.h>



ChatService * ChatService::instance() {
    static ChatService service;
    return &service;
}

void ChatService::login(const TcpConnectionPtr &conn, json &js, Timestamp time) {
    int id = js["id"].get<int>();
    string pwd = js["password"];

    User user = _userModel.query(id);
    if (user.getId() == id&&user.getPwd() == pwd) {
        if (user.getState() == "online") {
            //登录重复
            json response;
            response["msgid"] = LOGIN_MSG_ACK;
            response["errno"] = 2;
            response["errmsg"] = "该账户已经登录，请勿重复登录";
            conn->send(response.dump());
        }else {
            //登录成功
            user.setState("online");
            if (!_userModel.updateState(user)) {
                json response;
                response["msgid"] = LOGIN_MSG_ACK;
                response["errno"] = 3;
                response["errmsg"] = "用户状态更新失败";
                conn->send(response.dump());
                return;
            }
            {
                lock_guard<mutex> lock(_connMutex);
                _userConnMap[id] = conn;
            }
            if (_isRedisConnected && !_redis.subscribe(id)) {
                {
                    lock_guard<mutex> lock(_connMutex);
                    _userConnMap.erase(id);
                }
                user.setState("offline");
                _userModel.updateState(user);
                json response;
                response["msgid"] = LOGIN_MSG_ACK;
                response["errno"] = 4;
                response["errmsg"] = "redis订阅失败，请稍后重试";
                conn->send(response.dump());
                return;
            }
            json response;
            response["msgid"] = LOGIN_MSG_ACK;
            response["errno"] = 0;
            response["id"] = user.getId();
            response["name"] = user.getName();
            //查询用户是否有离线消息
            vector<string> vec = _offlineMsgModel.query(id);
            if (!vec.empty()) {
                response["offlinemsg"] = vec;
                _offlineMsgModel.remove(id);
            }
            //查询好友信息
            vector<User> vecFriend = _friendModel.query(id);
            if (!vecFriend.empty()) {
                vector<string> vec2;
                for (auto &user : vecFriend) {
                    json jss;
                    jss["id"] = user.getId();
                    jss["name"] = user.getName();
                    jss["state"] = user.getState();
                    vec2.push_back(jss.dump());
                }
                response["friends"] = vec2;
            }
            vector<Group> vecGroup = _groupModel.queryGroups(id);
            if (!vecGroup.empty()) {
                vector<string> vecGroupString;
                for (Group &group : vecGroup) {
                    json groupJson;
                    groupJson["id"] = group.getId();
                    groupJson["groupname"] = group.getName();
                    groupJson["groupdesc"] = group.getDesc();
                    vector<string> groupUserString;
                    for (GroupUser &user : group.getUsers()) {
                        json userJson;
                        userJson["id"] = user.getId();
                        userJson["name"] = user.getName();
                        userJson["state"] = user.getState();
                        userJson["role"] = user.getRole();
                        groupUserString.push_back(userJson.dump());
                    }
                    groupJson["users"] = groupUserString;
                    vecGroupString.push_back(groupJson.dump());
                }
                response["groups"] = vecGroupString;
            }

            conn->send(response.dump());
        }
    }else {
        json response;
        response["msgid"] = LOGIN_MSG_ACK;
        response["errno"] = 1;
        conn->send(response.dump());
    }
}

void ChatService::reg(const TcpConnectionPtr &conn, json &js, Timestamp time) {
    string name = js["name"];
    string pwd = js["password"];

    User user;
    user.setName(name);
    user.setPwd(pwd);
    bool state = _userModel.insert(user);
    if (state) {
        json response;
        response["msgid"] = REG_MSG_ACK;
        response["errno"] = 0;
        response["id"] = user.getId();
        conn->send(response.dump());
    }else {
        json response;
        response["msgid"] = REG_MSG_ACK;
        response["errno"] = 1;
        conn->send(response.dump());
    }
}
//注册消息以及对应的Hanlder回调操作
ChatService::ChatService() {
    _isRedisConnected = false;
    _msgHandlerMap.insert({LOGIN_MSG,std::bind(&ChatService::login,this,_1,_2,_3)});
    _msgHandlerMap.insert({REG_MSG,std::bind(&ChatService::reg,this,_1,_2,_3)});
    _msgHandlerMap.insert({LOGINOUT_MSG,std::bind(&ChatService::loginout,this,_1,_2,_3)});
    _msgHandlerMap.insert({ONE_CHAT_MSG,std::bind(&ChatService::oneChat,this,_1,_2,_3)});
    _msgHandlerMap.insert({ADD_FRIEND_MSG,std::bind(&ChatService::addFriend,this,_1,_2,_3)});
    _msgHandlerMap.insert({CREATE_GROUP_MSG,std::bind(&ChatService::createGroup,this,_1,_2,_3)});
    _msgHandlerMap.insert({ADD_GROUP_MSG,std::bind(&ChatService::addGroup,this,_1,_2,_3)});
    _msgHandlerMap.insert({GROUP_CHAT_MSG,std::bind(&ChatService::groupChat,this,_1,_2,_3)});

 

    if(_redis.connect()){
        _isRedisConnected = true;
        _redis.init_notify_handler(std::bind(&ChatService::handleRedisSubscribeMessage,this,_1,_2));
    } else {
        LOG_ERROR << "redis connect failed";
    }
}

MsgHandler ChatService::getHandler(int msgid) {
    auto it = _msgHandlerMap.find(msgid);
    if (it == _msgHandlerMap.end()) {
        return [=](const TcpConnectionPtr&conn,json &js,Timestamp time) {
            LOG_ERROR<<"msgid: "<<msgid<<" can not find handler!";
        };

    }
    return _msgHandlerMap[msgid];
}

void ChatService::clientCloseException(const TcpConnectionPtr &conn) {
    User user;
    {
        lock_guard<mutex> lock(_connMutex);

        for (auto it = _userConnMap.begin();it != _userConnMap.end();it++) {
            if (it->second == conn) {

                user.setId(it->first);
                _userConnMap.erase(it);
                break;;
            }
        }
    }
    if (_isRedisConnected) {
        _redis.unsubscribe(user.getId());
    }
    if (user.getId() != -1) {
        user.setState("offline");
        if (!_userModel.updateState(user)) {
            LOG_ERROR << "update offline state failed, user id:" << user.getId();
        }
    }

}

void ChatService::oneChat(const TcpConnectionPtr &conn, json &js, Timestamp time) {
    int toid = js["toid"].get<int>();
    {
        lock_guard<mutex> lock(_connMutex);
        auto it = _userConnMap.find(toid);
        // 在线
        if (it != _userConnMap.end()) {
            it->second->send(js.dump());
            return;
        }
    }

    User user = _userModel.query(toid);
    if(user.getState() == "online" && _isRedisConnected){
        _redis.publish(toid,js.dump());
        return;
    }
    // 不在线 存储离线消息
    _offlineMsgModel.insert(toid,js.dump());

}

//服务器异常，重置处理
void ChatService::reset() {
    // 重置用户状态
    _userModel.resetState();
}

void ChatService::addFriend(const TcpConnectionPtr &conn, json &js, Timestamp time) {
    int userid = -1;
    if (js.contains("userid") && js["userid"].is_number_integer()) {
        userid = js["userid"].get<int>();
    } else if (js.contains("id") && js["id"].is_number_integer()) {
        userid = js["id"].get<int>();
    }
    if (userid == -1 || !js.contains("friendid") || !js["friendid"].is_number_integer()) {
        LOG_ERROR << "invalid addFriend request: " << js.dump();
        return;
    }
    int friendid = js["friendid"].get<int>();

    //存储好友关系
    _friendModel.insert(userid,friendid);
}

void ChatService::loginout(const TcpConnectionPtr &conn, json &js, Timestamp time) {
    if (!js.contains("id") || !js["id"].is_number_integer()) {
        LOG_ERROR << "invalid loginout request: " << js.dump();
        return;
    }
    int userid = js["id"].get<int>();
    {
        lock_guard<mutex> lock(_connMutex);
        auto it = _userConnMap.find(userid);
        if (it != _userConnMap.end()) {
            _userConnMap.erase(it);
        }
    }
    if (_isRedisConnected) {
        _redis.unsubscribe(userid);
    }
    User user(userid, "", "", "offline");
    if (!_userModel.updateState(user)) {
        LOG_ERROR << "logout update offline failed, user id:" << userid;
    }
}

// 创建群组业务
void ChatService::createGroup(const TcpConnectionPtr &conn, json &js, Timestamp time)
{
    int userid = js["id"].get<int>();
    string name = js["groupname"];
    string desc = js["groupdesc"];

    Group group(-1, name, desc);
    json response;
    response["msgid"] = CREATE_GROUP_MSG;
    if (_groupModel.createGroup(group))
    {
        _groupModel.addGroup(userid, group.getId(), "creator");
        response["errno"] = 0;
        response["groupid"] = group.getId();
    }
    else
    {
        response["errno"] = 1;
    }
    conn->send(response.dump());
}
// 加入群组业务
void ChatService::addGroup(const TcpConnectionPtr &conn, json &js, Timestamp time)
{
    int userid = js["id"].get<int>();
    int groupid = js["groupid"].get<int>();
    _groupModel.addGroup(userid, groupid, "normal");

}

// 群组聊天业务
void ChatService::groupChat(const TcpConnectionPtr &conn, json &js, Timestamp time)
{
    int userid = js["id"].get<int>();
    int groupid = js["groupid"].get<int>();
    vector<int> useridVec = _groupModel.queryGroupUsers(userid, groupid);
    
        lock_guard<mutex> lock(_connMutex);
        for (int id : useridVec)
        {

            auto it = _userConnMap.find(id);
            if (it != _userConnMap.end())
            {
                // 转发群消息
                it->second->send(js.dump());
            }
            else
            {
                // 存储离线群消息
                User user = _userModel.query(id);
                if(user.getState() == "online" && _isRedisConnected){
                    _redis.publish(id,js.dump());
                }else{
                    _offlineMsgModel.insert(id, js.dump()); 
                }
                
            }
        }
    

}
// 处理订阅消息
void ChatService::handleRedisSubscribeMessage(int userid, string msg)
{
    
    lock_guard<mutex> lock(_connMutex);
    auto it = _userConnMap.find(userid);
    if(it != _userConnMap.end()){
        it->second->send(msg);
        return;
    }
    _offlineMsgModel.insert(userid, msg); 

}
