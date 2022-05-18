cmake_minimum_required(VERSION 3.7)

add_definitions(-std=c++17)

set(CXX_FLAGS "-Wall")
set(CMAKE_CXX_FLAGS, "${CXX_FLAGS}")

project(VMTranslator VERSION 1.0)
include_directories(
  ${CMAKE_SOURCE_DIR}/src
  ${CMAKE_SOURCE_DIR}/include
)

add_executable(
  JackCompiler

  src/main.cc
  src/keyword.cc
  src/exceptions.cc
  src/tokenizer.cc
  src/symbol_table.cc
  src/scope_list.cc
  src/compilation_engine.cc
)
