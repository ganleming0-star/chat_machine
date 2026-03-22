#include <iostream>
#include "json.hpp"
#include <muduo/net/TcpServer.h>
#include <muduo/net/EventLoop.h>
#include "chatserver.hpp"
#include "chatservice.hpp"
#include <signal.h>
#include <cstdlib>
#include <hiredis/hiredis.h>
using json = nlohmann::json;
using namespace muduo;
using namespace muduo::net;
void resetStateOnExit() {
     ChatService::instance()->reset();
}
void resetHandler(int sig) {
     std::exit(sig);
}
int main() {
     std::atexit(resetStateOnExit);
     signal(SIGINT,resetHandler);
     signal(SIGTERM,resetHandler);
     signal(SIGQUIT,resetHandler);

     EventLoop loop; //epoll 事件循环
     InetAddress listenAddr("192.168.6.129",6000);
     ChatServer server(&loop, listenAddr, "ChatServer");
     server.start();
     loop.loop(); //epoll_wait以阻塞方式等待连接，已连接用户的读写时间==事件等
     return 0;
}
