cmake_minimum_required(VERSION 3.5 FATAL_ERROR )

project (getrestandpubmq)

enable_testing()

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/build)

add_subdirectory(src)
add_subdirectory(test)

add_custom_target(astyle
  COMMAND astyle --options=astyle.options src/*.cpp src/*.hpp test/*.cpp test/stub/mqtt/*.h
)
