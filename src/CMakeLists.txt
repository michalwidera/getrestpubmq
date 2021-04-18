cmake_minimum_required(VERSION 3.5 FATAL_ERROR )

project(rtoy-src)

set(Boost_USE_STATIC_LIBS ON )
set(Boost_USE_MULTITHREADED ON)

FIND_PACKAGE( Boost COMPONENTS
    system
    thread
    REQUIRED )

include_directories( ${Boost_INCLUDE_DIR} ${mqtt_SOURCE_DIR}/include )

link_libraries(
    ${Boost_LIBRARIES}
)

ADD_DEFINITIONS(-DBOOST_BIND_GLOBAL_PLACEHOLDERS)
ADD_DEFINITIONS(-DHAS_BOOST)

set(SOURCES_RTOY
    main.cpp
    restclient.cpp
    mqttclient.cpp
    mgrapikey.cpp
    mgrpayload.cpp
   )

add_executable(rtoy ${SOURCES_RTOY})

target_link_libraries( rtoy ${Boost_LIBRARIES}  ${CMAKE_THREAD_LIBS_INIT} -lrt -ldl -lpaho-mqttpp3 -lpaho-mqtt3as )
