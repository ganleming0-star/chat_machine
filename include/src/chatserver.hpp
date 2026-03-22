//
// Created by 15455 on 2026/3/19.
//

#ifndef MYSQL_TEST_CHATSERVER_HPP
#define MYSQL_TEST_CHATSERVER_HPP

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
        //设置muduo库的线程数量
        _server.setThreadNum(4);
    }

    void start() ;

private:

    //处理用户的连接和断开
    void onConnection(const TcpConnectionPtr& conn);
    



    //处理用户的读写事件
    void onMessage(const TcpConnectionPtr& conn,   //连接
                Buffer* buff,//缓冲区
                Timestamp time);/* 接受到数据时间*/

    TcpServer _server;
    
    EventLoop *_loop;//epoll 事件循环s
};

#endif //MYSQL_TEST_CHATSERVER_HPP