cmake_minimum_required(VERSION 3.24)
project(tokenized_myers_diff_tests)

add_executable(digits_gen Digits/main.cpp)
target_link_libraries(digits_gen ArgParserLib)
