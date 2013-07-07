cmake_minimum_required (VERSION 2.8)
project (chip8 CXX)

option(test "Automatically run and build the tests when running make" OFF)

if(CMAKE_BUILD_TYPE STREQUAL "")
  set(CMAKE_BUILD_TYPE Debug)
endif()

list(APPEND CMAKE_CXX_FLAGS "-std=c++0x -g")

include_directories("${PROJECT_BINARY_DIR}")

add_subdirectory(include/gmock EXCLUDE_FROM_ALL)
include_directories(${gtest_SOURCE_DIR}/include)
include_directories(${gmock_SOURCE_DIR}/include)

enable_testing()
add_executable(Chip8Test EXCLUDE_FROM_ALL
              src/chip8.h
              src/chip8.cpp
              src/chip8Test.cpp)
target_link_libraries(Chip8Test gmock_main)
add_test(Chip8Test Chip8Test)

if (test)
	add_custom_target(tests ALL ctest DEPENDS Chip8Test)
else()
	add_custom_target(tests ctest DEPENDS Chip8Test)
endif()

set(EXECUTABLE_NAME Chip8Emulator)
add_executable(${EXECUTABLE_NAME}
                src/chip8.h
                src/chip8.cpp
                src/emulator.cpp)

#Libraries
set(CMAKE_MODULE_PATH "${CMAKE_SOURCE_DIR}/cmake_modules" ${CMAKE_MODULE_PATH})
find_package(SFML 2 REQUIRED system window graphics network audio)

include_directories(${SFML_INCLUDE_DIR})
target_link_libraries(${EXECUTABLE_NAME} ${SFML_LIBRARIES})

#Copy resources
add_custom_command(TARGET Chip8Emulator PRE_BUILD
                   COMMAND ${CMAKE_COMMAND} -E copy_directory
                       ${CMAKE_SOURCE_DIR}/resources $<TARGET_FILE_DIR:Chip8Emulator>/resources)