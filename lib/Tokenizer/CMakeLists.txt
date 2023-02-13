cmake_minimum_required(VERSION 3.23)
project(tokenizer_lib)
set(CMAKE_CXX_STANDARD 20)

add_subdirectory(tests)

add_library(TokenizerLib STATIC Tokenizer.cpp)
