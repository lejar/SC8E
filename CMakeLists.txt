cmake_minimum_required (VERSION 2.8)
project (chip8)

if(CMAKE_BUILD_TYPE STREQUAL "")
  set(CMAKE_BUILD_TYPE Debug)
endif()

list( APPEND CMAKE_CXX_FLAGS "-std=c++0x ${CMAKE_CXX_FLAGS} -g")

include_directories("${PROJECT_BINARY_DIR}")

set(EXECUTABLE_NAME "Chip8Test")


find_path(GTEST_INCLUDE_DIR gtest/gtest.h
          PATH_SUFFIXES include
          PATHS
          include/)
find_path(GMOCK_INCLUDE_DIR gmock/gmock.h
          PATH_SUFFIXES include
          PATHS
          include/)

include_directories("include/")

find_library(GTEST gtest PATHS lib/)
find_library(GTESTMAIN gtest_main PATHS lib/)
find_library(GMOCK gmock PATHS lib/)

add_executable(${EXECUTABLE_NAME}
              src/chip8.h
              src/chip8.cpp
              src/chip8Test.cpp)

include_directories("${GTEST_INCLUDE_DIR}}")
include_directories("${GMOCK_INCLUDE_DIR}}")
set(CMAKE_LIBRARY_PATH ${CMAKE_LIBRARY_PATH} lib/)
target_link_libraries(${EXECUTABLE_NAME} ${GTEST} ${GTESTMAIN} ${GMOCK} pthread)

add_custom_command(TARGET Chip8Test POST_BUILD COMMAND Chip8Test)

# add the executable
set(EXECUTABLE_NAME "Chip8Emulator")
add_executable(${EXECUTABLE_NAME}
                src/chip8.h
                src/chip8.cpp
                src/emulator.cpp)

#Libraries
set(CMAKE_MODULE_PATH "${CMAKE_SOURCE_DIR}/cmake_modules" ${CMAKE_MODULE_PATH})
find_package(SFML 2 REQUIRED system window graphics network audio)
#if(SFML_FOUND)
  include_directories(${SFML_INCLUDE_DIR})
  target_link_libraries(${EXECUTABLE_NAME} ${SFML_LIBRARIES})
#endif()

add_custom_command(TARGET Chip8Emulator POST_BUILD COMMAND Chip8Test)