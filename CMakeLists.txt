cmake_minimum_required(VERSION 3.23)

project(RedisPlusPls)


set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED True)


add_executable(redis 
    app/redis_server.cpp
)

target_include_directories(redis PRIVATE app)
