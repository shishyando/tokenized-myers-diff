project(ls_communicator_lib)
set(CMAKE_CXX_STANDARD 20)

add_subdirectory(tests)

find_package(Qt5 REQUIRED COMPONENTS Core)
add_library(LSCommunicatorLib STATIC LSCommunicator.cpp)
target_link_libraries(LSCommunicatorLib Qt5::Core)
