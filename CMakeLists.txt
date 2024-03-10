cmake_minimum_required(VERSION 3.10)
project(lab5-7)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

add_executable(control control.cpp)
add_executable(client client.cpp)

target_link_libraries(con zmq)
target_link_libraries(cl zmq)