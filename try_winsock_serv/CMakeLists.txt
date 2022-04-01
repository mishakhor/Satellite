cmake_minimum_required(VERSION 3.20.2)
project(try_winsock_serv)

set(CMAKE_CXX_STANDARD 14)


add_executable(try_winsock_serv main.cpp)

if(WIN32)
    target_link_libraries(try_winsock_serv wsock32 ws2_32)
endif()