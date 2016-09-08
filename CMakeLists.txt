project (leapDriver)

cmake_minimum_required(VERSION 2.8)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11 -g -Wall")

set(LEAP_SDK_ROOT ../LeapSDK)

include_directories(include/)

include_directories(${LEAP_SDK_ROOT}/include/)
find_library(LEAP_LIB Leap HINTS ${LEAP_SDK_ROOT}/lib/x64/)

set(SRC
    src/Driver.cpp
    src/InputInterface.cpp
    src/main.cpp
    )

add_executable(leapdriver ${SRC})
target_link_libraries(leapdriver ${LEAP_LIB})
