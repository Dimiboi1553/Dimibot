cmake_minimum_required(VERSION 3.27)
project(Dimibot)

set(CMAKE_CXX_STANDARD 17)

add_executable(Dimibot main.cpp
        Eval.h
        Eval.cpp
        Search.h
        Search.cpp
)
