#include "../../include/src/chatserver.hpp"

#include "chatservice.hpp"
#include "../../include/src/json.hpp"
#include <muduo/base/Logging.h>
using json = nlohmann::json;

void ChatServer::start() {
    _server.start();
}
//
void ChatServer::onConnection(const TcpConnectionPtr &conn) {
    if (!conn->connected()) {
        ChatService::instance()->clientCloseException(conn);
        conn->shutdown();
    }
}
//上报读写事件相关信息的回调函数
void ChatServer::onMessage(const TcpConnectionPtr &conn, Buffer *buff, Timestamp time) {
    string buf = buff->retrieveAllAsString();
    if (buf.empty()) {
        return;
    }

    json js = json::parse(buf, nullptr, false);
    if (js.is_discarded()) {
        LOG_ERROR << "invalid json message from " << conn->peerAddress().toIpPort();
        return;
    }

    if (!js.contains("msgid") || !js["msgid"].is_number_integer()) {
        LOG_ERROR << "invalid msgid from " << conn->peerAddress().toIpPort();
        return;
    }

    auto msgHandler = ChatService::instance()->getHandler(js["msgid"].get<int>());
    msgHandler(conn,js,time);
}
