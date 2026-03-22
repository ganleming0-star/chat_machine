cmake_minimum_required(VERSION 3.22.1)
project(mysql_test LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${PROJECT_SOURCE_DIR}/bin)

add_executable(server
        src/server/main.cpp
        src/server/chatserver.cpp
        src/server/chatservice.cpp
        src/server/usermodel.cpp
        src/server/offlineMessageModel.cpp
        src/server/friendModel.cpp
        src/server/groupModel.cpp
        src/redis/redis.cpp
)

target_include_directories(server PRIVATE
        ${PROJECT_SOURCE_DIR}/include
        ${PROJECT_SOURCE_DIR}/include/db
        ${PROJECT_SOURCE_DIR}/include/src
        ${PROJECT_SOURCE_DIR}/include/redis
        /usr/include/muduo
        /usr/include/boost
        /usr/include/mysql
        /usr/include/hiredis
)

target_link_libraries(server PRIVATE
        muduo_net
        muduo_base
        mysqlclient
        hiredis
        pthread
)

add_executable(client
        src/client/main.cpp
        src/redis/redis.cpp
)

target_include_directories(client PRIVATE
        ${PROJECT_SOURCE_DIR}/include
        ${PROJECT_SOURCE_DIR}/include/db
        ${PROJECT_SOURCE_DIR}/include/src
        ${PROJECT_SOURCE_DIR}/include/redis
        /usr/include/muduo
        /usr/include/boost
        /usr/include/mysql
        /usr/include/hiredis
)

target_link_libraries(client PRIVATE
        muduo_net
        muduo_base
        mysqlclient
        hiredis
        pthread
)
