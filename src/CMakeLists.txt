cmake_minimum_required(VERSION 3.5 FATAL_ERROR )

project(rtoy-src)

set(Boost_USE_STATIC_LIBS ON )
set(Boost_USE_MULTITHREADED ON)

include(FetchContent)

FetchContent_Declare(cpr GIT_REPOSITORY https://github.com/whoshuu/cpr.git GIT_TAG c8d33915dbd88ad6c92b258869b03aba06587ff9) # the commit hash for 1.5.0
set(cpr_BUILD_TESTS OFF)
FetchContent_MakeAvailable(cpr)

#FetchContent_Populate(mqtt GIT_REPOSITORY https://github.com/redboltz/mqtt_cpp.git GIT_TAG HEAD SOURCE_DIR mqtt) # the commit hash for head 16.04.2021
#set(mqtt_BUILD_TESTS OFF)

FIND_PACKAGE( Boost COMPONENTS
    filesystem
    program_options
    chrono
    date_time
    system
    thread
    REQUIRED )

include_directories( ${Boost_INCLUDE_DIR} ${mqtt_SOURCE_DIR}/include )

link_libraries(
    ${Boost_LIBRARIES}
    cpr::cpr
)

set(SOURCES_RTOY
    main.cpp
   )

add_executable(rtoy ${SOURCES_RTOY})

target_link_libraries( rtoy ${Boost_LIBRARIES}  ${CMAKE_THREAD_LIBS_INIT} -lrt -ldl -lpaho-mqttpp3 -lpaho-mqtt3as )

ADD_DEFINITIONS( "-DHAS_BOOST" )
