cmake_minimum_required(VERSION 3.10.0)

project(cppexamples)

# find boost
# find_package( Boost 1.75.0 EXACT COMPONENTS system thread REQUIRED )
find_package( Boost 1.75.0  )

include_directories( ${Boost_INCLUDE_DIR} )

include_directories(
    ${CMAKE_SOURCE_DIR}/commons/
    ${CMAKE_SOURCE_DIR}/lightmuduo/
)

# a simple helloworld
add_executable(
    helloword
    helloword.cpp
)

# deadlock example
add_executable(
    deadlock
    deadlock.cpp
)
target_link_libraries(
    deadlock
    pthread
)

# singleton example
add_executable(
    singleton
    singleton.cpp
)
target_link_libraries(
    singleton
    pthread
)

# atomic example
add_executable(atomicexample atomicexample.cpp)
target_link_libraries(atomicexample pthread)


# block queue
add_executable(blockqueue blockqueue.cpp)
target_link_libraries(blockqueue pthread)


# lightmuduo -- echo
add_executable(echoserver 
    lightmuduo/echo.cpp
    lightmuduo/event_loop.cpp
    lightmuduo/channel.cpp
    lightmuduo/poller.cpp
)

