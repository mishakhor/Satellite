cmake_minimum_required(VERSION 3.21)
project(Satellite)

set(CMAKE_CXX_STANDARD 14)

add_executable(Satellite main.cpp includes.h vec_operations.h vec_operations.cpp satelite.cpp integrator.h integrator.cpp)
