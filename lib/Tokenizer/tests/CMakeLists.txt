cmake_minimum_required(VERSION 3.23)
project(tokenizer_tests)

add_executable(tokenizer_test unit_test.cpp)

include_directories(..)

target_link_libraries(tokenizer_test TokenizerLib)
