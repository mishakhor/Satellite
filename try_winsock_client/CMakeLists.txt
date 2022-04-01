cmake_minimum_required(VERSION 3.20.2)
project(try_winsock_client)

set(CMAKE_CXX_STANDARD 14)

add_executable(try_winsock_client main.cpp)

if(WIN32)
    target_link_libraries(try_winsock_client wsock32 ws2_32)
endif()