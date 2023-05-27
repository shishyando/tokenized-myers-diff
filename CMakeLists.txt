cmake_minimum_required(VERSION 3.16)
project(tokenized_myers_diff)

set(CMAKE_CXX_STANDARD 20)

add_compile_options(-O3)

include_directories(.)


add_library(MyersDiff STATIC lib/MyersDiff/MyersDiff.cpp)
add_library(DiffPrint STATIC lib/DiffPrint/DiffPrint.cpp)
add_library(Timer STATIC lib/Timer/Timer.cpp)
add_library(DiffRunner STATIC lib/DiffRunner/DiffRunner.cpp)

add_subdirectory(tests)

add_executable(diff bin/main.cpp)

add_subdirectory(lib/LSCommunicator)
add_subdirectory(lib/Tokenizer)

target_link_libraries(diff MyersDiff DiffPrint Timer TokenizerLib DiffRunner)
