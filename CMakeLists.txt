cmake_minimum_required(VERSION 3.1)
project(tricks)

set(CMAKE_EXPORT_COMPILE_COMMANDS 1)

set(CMAKE_CXX_FLAGS "-g -std=c++14")

file(GLOB SOURCES src/*.cpp)

foreach(SOURCE ${SOURCES})
  get_filename_component(bin_target ${SOURCE} NAME_WE)
  message(STATUS "adding target ${bin_target}")
  add_executable(${bin_target} ${SOURCE})
  target_link_libraries(${bin_target} m pthread)
endforeach()
