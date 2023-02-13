cmake_minimum_required(VERSION 3.23)
project(tokenized_myers_diff)

set(CMAKE_CXX_STANDARD 20)

include_directories(.)

add_library(ArgParserLib STATIC lib/ArgParser/ArgParser.cpp)
add_library(MyersDiff STATIC lib/MyersDiff/MyersDiff.cpp)
add_library(DiffPrint STATIC lib/DiffPrint/DiffPrint.cpp)

add_subdirectory(tests)

add_executable(diff bin/main.cpp)

add_subdirectory(lib/Tokenizer)

target_link_libraries(diff ArgParserLib TokenizerLib MyersDiff DiffPrint)

