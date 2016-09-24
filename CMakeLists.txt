project (leapDriver)

cmake_minimum_required(VERSION 2.8)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++14 -g -Wall")



find_package (Threads)

set(LEAP_SDK_ROOT "external/LeapSDK")
set(JSON "external/json")
set(NITRO "external/nitro")

include_directories(include/)

include_directories(${LEAP_SDK_ROOT}/include/)

find_library(LEAP_LIB Leap HINTS ${LEAP_SDK_ROOT}/lib/x64/)

message(${LEAP_LIB})

if(NOT LEAP_LIB)
message(FATAL_ERROR "Could't find LeapMotion SDK. Please download the SDK from https://developer.leapmotion.com/v2 . Install the debian pacakge and place the LeapSDK folder into \"external/\"")
endif(NOT LEAP_LIB)

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

install(TARGETS leapdriver
        RUNTIME DESTINATION bin)
        
install(FILES ${LEAP_LIB}
        DESTINATION /usr/local/lib/)
        
        
install(FILES leap_driver_config.json DESTINATION /etc/)
install(FILES system/leap_driver.service system/leapd.service DESTINATION /lib/systemd/system/)

install(CODE "
    MESSAGE(\"Running ldconfig.\")
    execute_process(COMMAND ldconfig)
    MESSAGE(\"Register with systemd.\")
    execute_process(COMMAND systemctl enable leapd.service)
    execute_process(COMMAND systemctl enable leap_driver.service)
    execute_process(COMMAND systemctl start leap_driver.service)")
