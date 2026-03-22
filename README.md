# Muduo Chat Project

基于 Muduo 网络库实现的 C++ 集群聊天系统，包含服务端与命令行客户端，支持用户注册登录、一对一聊天、离线消息、好友管理和群聊。

## 功能特性

- 用户注册、登录、注销
- 一对一聊天（在线转发 + 离线存储）
- 好友添加与好友列表拉取
- 群组创建、加群、群聊
- Redis 发布订阅转发跨进程消息
- 服务端异常退出后重置用户在线状态

## 技术栈

- C++20
- Muduo（`muduo_net`、`muduo_base`）
- MySQL（`mysqlclient`）
- Redis（`hiredis`）
- nlohmann/json（项目内头文件）
- CMake

## 目录结构

```text
.
├── CMakeLists.txt
├── include
│   ├── db                  # MySQL 封装与配置
│   ├── redis               # Redis 封装
│   └── src                 # 业务头文件（服务层、模型层、实体）
└── src
    ├── client              # 命令行客户端
    ├── redis               # Redis 实现
    └── server              # 服务端网络层 + 业务层 + 数据层
```

## 环境依赖

请先安装并启动以下组件：

- Muduo
- Boost
- MySQL Server + MySQL 开发库
- Redis Server + hiredis 开发库
- CMake 3.22+
- 支持 C++20 的编译器（g++/clang++）

项目默认头文件和库路径在 `CMakeLists.txt` 中硬编码为：

- `/usr/include/muduo`
- `/usr/include/boost`
- `/usr/include/mysql`
- `/usr/include/hiredis`

如果你的环境路径不同，请先修改 `CMakeLists.txt`。

## 数据库初始化

项目代码依赖数据库 `chat_db`，并使用以下数据表：

- `User`
- `Friend`
- `OfflineMessage`
- `AllGroup`
- `GroupUser`

可使用如下 SQL 初始化（按当前代码行为整理）：

```sql
CREATE DATABASE IF NOT EXISTS chat_db;
USE chat_db;

CREATE TABLE IF NOT EXISTS `User` (
  `id` INT PRIMARY KEY AUTO_INCREMENT,
  `name` VARCHAR(50) NOT NULL UNIQUE,
  `password` VARCHAR(50) NOT NULL,
  `state` VARCHAR(20) NOT NULL DEFAULT 'offline'
);

CREATE TABLE IF NOT EXISTS `Friend` (
  `userid` INT NOT NULL,
  `friendid` INT NOT NULL,
  PRIMARY KEY (`userid`, `friendid`)
);

CREATE TABLE IF NOT EXISTS `OfflineMessage` (
  `userid` INT NOT NULL,
  `message` TEXT NOT NULL
);

CREATE TABLE IF NOT EXISTS `AllGroup` (
  `id` INT PRIMARY KEY AUTO_INCREMENT,
  `groupname` VARCHAR(50) NOT NULL,
  `groupdesc` VARCHAR(200) DEFAULT ''
);

CREATE TABLE IF NOT EXISTS `GroupUser` (
  `groupid` INT NOT NULL,
  `userid` INT NOT NULL,
  `grouprole` VARCHAR(20) NOT NULL DEFAULT 'normal',
  PRIMARY KEY (`groupid`, `userid`)
);
```

## 配置项

### MySQL 配置

在 `include/db/db.hpp` 中修改数据库连接参数：

- `server`
- `user`
- `password`
- `dbname`

### Redis 配置

`src/redis/redis.cpp` 默认连接：

- `127.0.0.1:6379`

如有需要请修改 `redisConnect` 的地址和端口。

### 服务端监听地址

`src/server/main.cpp` 中默认监听：

- `192.168.6.129:6000`

如果在本机测试，建议改为：

- `127.0.0.1:6000`

## 编译

在项目根目录执行：

```bash
cmake -S . -B build
cmake --build build -j
```

编译完成后，二进制输出到 `bin/`：

- `bin/server`
- `bin/client`

## 运行

1. 启动 MySQL 与 Redis
2. 启动服务端
3. 启动客户端并连接服务端

```bash
./bin/server
./bin/client 127.0.0.1 6000
```

如果服务端监听地址不是 `127.0.0.1`，请按实际 IP 调整客户端参数。

## 客户端命令

登录成功后支持如下命令：

- `help`：显示命令
- `chat:friendid:message`：单聊
- `addfriend:friendid`：添加好友
- `creategroup:groupname:groupdesc`：创建群组
- `addgroup:groupid`：加入群组
- `groupchat:groupid:message`：群聊
- `loginout`：注销

## 消息类型（msgid）

定义位置：`include/src/public.hpp`

- `1` `LOGIN_MSG`
- `2` `LOGIN_MSG_ACK`
- `3` `LOGINOUT_MSG`
- `4` `REG_MSG`
- `5` `REG_MSG_ACK`
- `6` `ONE_CHAT_MSG`
- `7` `ADD_FRIEND_MSG`
- `8` `CREATE_GROUP_MSG`
- `9` `ADD_GROUP_MSG`
- `10` `GROUP_CHAT_MSG`

## 常见问题

- 客户端连接失败：先检查服务端监听 IP 与端口是否一致
- 登录后收不到跨进程消息：检查 Redis 是否启动、服务端是否成功订阅
- 数据库操作失败：检查 MySQL 连接参数、`chat_db` 与表结构是否初始化
