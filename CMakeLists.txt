cmake_minimum_required(VERSION 3.24)
project(tokenized_myers_diff)

set(CMAKE_CXX_STANDARD 14)

include_directories(.)

add_library(ArgParserLib STATIC lib/ArgParser.cpp)

add_subdirectory(tests)

add_executable(diff bin/main.cpp)

target_link_libraries(diff ArgParserLib)

