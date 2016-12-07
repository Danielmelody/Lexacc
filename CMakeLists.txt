cmake_minimum_required(VERSION 3.0)

project(compiler)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++0x -Wall -g")

include_directories(include)

aux_source_directory(src/ COMPILER_SRC)

add_executable(compiler ${COMPILER_SRC})
