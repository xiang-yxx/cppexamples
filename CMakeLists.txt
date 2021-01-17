cmake_minimum_required(VERSION 3.10.0)

project(cppexamples)

include_directories(
    ${CMAKE_SOURCE_DIR}/commons/
)

add_executable(
    helloword
    helloword.cpp
)

add_executable(
    deadlock
    deadlock.cpp
)
target_link_libraries(
    deadlock
    pthread
)
