project(ls_communicator_tests)
set(CMAKE_CXX_STANDARD 20)

add_executable(LSCommunicatorTest test.cpp)

include_directories(..)

find_package(Qt5 REQUIRED COMPONENTS Core)
target_link_libraries(LSCommunicatorTest Qt5::Core)
target_link_libraries(LSCommunicatorTest LSCommunicatorLib)
