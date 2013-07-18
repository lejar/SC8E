cmake_minimum_required (VERSION 2.8)

include_directories(SYSTEM ${gtest_SOURCE_DIR}/include)
include_directories(${CMAKE_SOURCE_DIR}/src)

enable_testing()

if(auto_test)
  set(AUTO_TEST "ALL")
endif()

add_custom_target(test ${AUTO_TEST} COMMAND ${CMAKE_CTEST_COMMAND} WORKING_DIRECTORY ${CURRENT_BINARY_DIR})
add_custom_target(build_tests WORKING_DIRECTORY ${CURRENT_BINARY_DIR})
add_dependencies(test build_tests)

add_executable(Chip8Test EXCLUDE_FROM_ALL
  opcodes.cpp
  ${CMAKE_SOURCE_DIR}/src/instructions.cpp
  ${CMAKE_SOURCE_DIR}/src/chip8.cpp
)
target_link_libraries(Chip8Test gtest_main)
# disable warning clang generates for gtest
set_target_properties(gtest gtest_main PROPERTIES
  COMPILE_FLAGS "-Wno-error=missing-field-initializers"
)

add_test(Chip8Test Chip8Test)
add_dependencies(build_tests Chip8Test)
