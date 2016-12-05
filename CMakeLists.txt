cmake_minimum_required(VERSION 3.0)

project(compiler)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++0x -Wall -Werror -O3")

include_directories(include)

set(COMPILER_HEAD include/)

set(COMPILER_SRC
    src/lexer.cpp
)

add_executable(compiler ${COMPILER_SRC})
