cmake_minimum_required(VERSION 3.16)
project(aimf)

set(CMAKE_CXX_STANDARD 17)

include_directories(include)

add_executable(aimf
    cli/main.cpp
    container/header.cpp
    container/stream_table.cpp
    container/chunk.cpp
    codecs/dummy_codec.cpp
)

add_executable(aimf_inspect
    tools/inspect.cpp
)

add_executable(codec_test
    tools/test_codec.cpp
)
