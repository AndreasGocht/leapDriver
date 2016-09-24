project (leapDriver)

cmake_minimum_required(VERSION 2.8)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++14 -g -Wall")

set(LEAP_SDK_ROOT ../LeapSDK)

find_package (Threads)

SET(JSON "external/json")
SET(NITRO "external/nitro")


include_directories(include/)

include_directories(${LEAP_SDK_ROOT}/include/)
find_library(LEAP_LIB Leap HINTS ${LEAP_SDK_ROOT}/lib/x64/)

message(STATUS "Looking for json")
execute_process(COMMAND git submodule update --init "${CMAKE_SOURCE_DIR}/${JSON}")
include_directories(${JSON}/src)

message(STATUS "Looking for nitro")
execute_process(COMMAND git submodule update --init "${CMAKE_SOURCE_DIR}/${NITRO}")
include_directories(${NITRO}/include)


set(SRC
    src/Driver.cpp
    src/InputInterface.cpp
    src/main.cpp
    )

add_executable(leapdriver ${SRC})
target_link_libraries(leapdriver ${LEAP_LIB} ${CMAKE_THREAD_LIBS_INIT})
