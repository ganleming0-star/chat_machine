#include <muduo/net/TcpServer.h>
#include <muduo/net/TcpConnection.h>
#include <iostream>
#include <functional>
using namespace muduo;
using namespace muduo::net;
using namespace std;

class ChatServer {
public:
    ChatServer(EventLoop *loop,
        const InetAddress &listenAddr, //IP+port
        const string &nameArg //服务器 名
        ):_server(loop,listenAddr,nameArg),_loop(loop)
    {
        //给服务器注册用户连接的创建和断开回调
        _server.setConnectionCallback(bind(&ChatServer::onConnection,this,_1));
        //给服务器注册用户读写事件回调
        _server.setMessageCallback(bind(&ChatServer::onMessage,this,_1,_2,_3));

        _server.setThreadNum(4);
    }

    void start() {
        _server.start();
    }

private:

    //处理用户的连接和断开
    void onConnection(const TcpConnectionPtr& conn) {
        if (conn->connected()) {
            cout<<conn->peerAddress().toIpPort()<<" -> "<<conn->localAddress().toIpPort()<<"status:yes"<<endl;
        }else {
            cout<<conn->peerAddress().toIpPort()<<" -> "<<conn->localAddress().toIpPort()<<"status:no"<<endl;
            conn->shutdown();
        }
    }

    //处理用户的读写事件
    void onMessage(const TcpConnectionPtr& conn,   //连接
                Buffer* buff,//缓冲区
                Timestamp time)/* 接受到数据时间*/
    {
        string buf = buff->retrieveAllAsString();
        cout<<"recieve "<<buf<<" time "<<time.toString()<<endl;
        conn->send(buf);
    }
    TcpServer _server;
    EventLoop *_loop;//epoll 事件循环
};