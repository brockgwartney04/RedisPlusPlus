cmake_minimum_required(VERSION 3.23)

project(RedisPlusPlus)


set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED True)


add_executable(redispp)

target_sources(redispp
    PRIVATE
        main.cpp
)