cmake_minimum_required(VERSION 3.5 FATAL_ERROR )

project(xtoy-src)

set(Boost_USE_STATIC_LIBS ON )
set(Boost_USE_MULTITHREADED ON)

FIND_PACKAGE( Boost COMPONENTS
    thread
    filesystem
    serialization
    regex
    program_options
    chrono
    date_time
    system
    REQUIRED )

MESSAGE(STATUS "Boost information:")
MESSAGE(STATUS "Boost_INCLUDE_DIRS: ${Boost_INCLUDE_DIRS}")
MESSAGE(STATUS "Boost_LIBRARY_DIRS: ${Boost_LIBRARY_DIRS}")
MESSAGE(STATUS "Boost_LIBRARIES: ${Boost_LIBRARIES}")

include_directories( ${Boost_INCLUDE_DIR} "share" )

link_libraries(
    ${Boost_LIBRARIES}
)

set(SOURCES_RTOY
    main.cpp
   )

add_executable( rtoy ${SOURCES_RTOY} )

INCLUDE_DIRECTORIES(${Boost_INCLUDE_DIR})
ADD_DEFINITIONS( "-DHAS_BOOST" )

