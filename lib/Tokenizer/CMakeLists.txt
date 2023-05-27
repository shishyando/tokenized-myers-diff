project(tokenizer_lib)
set(CMAKE_CXX_STANDARD 20)

add_subdirectory(tests)

add_library(TokenizerLib STATIC Tokenizer.cpp)
target_link_libraries(TokenizerLib LSCommunicatorLib)
