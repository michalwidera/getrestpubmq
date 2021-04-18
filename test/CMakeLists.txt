cmake_minimum_required(VERSION 3.5 FATAL_ERROR )

project(rtoy-test)

set(Boost_USE_STATIC_LIBS ON )
set(Boost_USE_MULTITHREADED ON)

enable_testing()

FIND_PACKAGE( Boost COMPONENTS
    system
    thread
    REQUIRED )

link_libraries(
    ${Boost_LIBRARIES}
)

INCLUDE_DIRECTORIES("${Boost_INCLUDE_DIR};${CMAKE_SOURCE_DIR}/test/stub;${CMAKE_SOURCE_DIR}/src")

ADD_DEFINITIONS(-DBOOST_TEST_DYN_LINK)
ADD_DEFINITIONS(-DBOOST_BIND_GLOBAL_PLACEHOLDERS)
ADD_DEFINITIONS(-DHAS_BOOST)

set(CMAKE_CURRENT_DATA_DIR ${CMAKE_SOURCE_DIR}/test/sandbox)

set(SOURCES_MGRAPIKEY
  ${CMAKE_SOURCE_DIR}/src/mgrapikey.cpp
  test_mgrapikey.cpp
)

set(SOURCES_MGRPAYLOAD
  ${CMAKE_SOURCE_DIR}/src/mgrpayload.cpp
  test_mgrpayload.cpp
)

set(SOURCES_MQTTCLIENT
  ${CMAKE_SOURCE_DIR}/src/mqttclient.cpp
  test_mqttclient.cpp
)

set(SOURCES_RESTCLIENT
  ${CMAKE_SOURCE_DIR}/src/restclient.cpp
  test_restclient.cpp
)


add_executable( test_mgrapikey ${SOURCES_MGRAPIKEY} )
add_executable( test_mgrpayload ${SOURCES_MGRPAYLOAD} )
add_executable( test_mqttclient ${SOURCES_MQTTCLIENT} )
add_executable( test_restclient ${SOURCES_RESTCLIENT} )

target_link_libraries( test_mgrapikey ${Boost_LIBRARIES} ${CMAKE_THREAD_LIBS_INIT} -lrt -ldl -lboost_unit_test_framework )
target_link_libraries( test_mgrpayload ${Boost_LIBRARIES} ${CMAKE_THREAD_LIBS_INIT} -lrt -ldl -lboost_unit_test_framework )
target_link_libraries( test_mqttclient ${Boost_LIBRARIES} ${CMAKE_THREAD_LIBS_INIT} -lrt -ldl -lboost_unit_test_framework )
target_link_libraries( test_restclient ${Boost_LIBRARIES} ${CMAKE_THREAD_LIBS_INIT} -lrt -ldl -lboost_unit_test_framework )

add_test(
  NAME ut-test_mgrapikey
  WORKING_DIRECTORY ${CMAKE_CURRENT_DATA_DIR}
  COMMAND test_mgrapikey
)

add_test(
  NAME ut-test_mgrpayload
  WORKING_DIRECTORY ${CMAKE_CURRENT_DATA_DIR}
  COMMAND test_mgrpayload
)

add_test(
  NAME ut-test_mqttclient
  WORKING_DIRECTORY ${CMAKE_CURRENT_DATA_DIR}
  COMMAND test_mqttclient
)

add_test(
  NAME ut-test_restclient
  WORKING_DIRECTORY ${CMAKE_CURRENT_DATA_DIR}
  COMMAND test_restclient
)
