cmake_minimum_required(VERSION 3.0)

project(compiler)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++14 -Wall -g")

include_directories(include)

aux_source_directory(lib/ COMPILER_LIB)

aux_source_directory(example/ COMPILER_EXAMPLE)

add_executable(compiler ${COMPILER_LIB} ${COMPILER_EXAMPLE})
