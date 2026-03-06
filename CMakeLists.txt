cmake_minimum_required(VERSION 3.16)
project(aimf)

set(CMAKE_CXX_STANDARD 17)

include_directories(include)

add_executable(aimf
    cli/main.cpp
    container/header.cpp
    container/stream_table.cpp
    container/chunk.cpp
)
