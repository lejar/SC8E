cmake_minimum_required (VERSION 2.8)

include_directories(${gtest_SOURCE_DIR}/include)
include_directories(${CMAKE_SOURCE_DIR}/src)

enable_testing()

add_custom_target(test COMMAND ${CMAKE_CTEST_COMMAND} WORKING_DIRECTORY ${CURRENT_BINARY_DIR})
add_custom_target(build_tests WORKING_DIRECTORY ${CURRENT_BINARY_DIR})
add_dependencies(test build_tests)

add_executable(Chip8Test
  opcodes.cpp
  ${CMAKE_SOURCE_DIR}/src/instructions.cpp
  ${CMAKE_SOURCE_DIR}/src/chip8.cpp
)
target_link_libraries(Chip8Test gtest_main)
add_test(Chip8Test Chip8Test)
add_dependencies(build_tests Chip8Test)

if (test)
  add_custom_target(tests ALL ctest DEPENDS Chip8Test)
else()
  add_custom_target(tests ctest DEPENDS Chip8Test)
endif()
