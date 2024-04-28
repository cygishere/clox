cmake_minimum_required(VERSION 3.15)

project(
  clox
  VERSION 0.1
  LANGUAGES C)

set(CMAKE_C_STANDARD 99)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

option(LOX_ENABLE_SANITISERS "Enable sanitisers" OFF)

add_compile_options(-Wall)
add_compile_options(-Wextra)
add_compile_options(-pedantic)
add_compile_options(-g)

if(LOX_ENABLE_SANITISERS)
  add_compile_options(-fsanitize=address,leak,undefined)
  add_link_options(-fsanitize=address,leak,undefined)
endif()

add_executable(
  clox
  src/lox.c
  src/token.c
  src/scanner.c
  src/parser.c
  src/ast_print.c)

target_include_directories(
  clox
  PUBLIC
  ${CMAKE_CURRENT_SOURCE_DIR}/include)


# Tools
add_executable(
  hashgen
  EXCLUDE_FROM_ALL
  tool/hashgen/hashgen.c)

target_include_directories(
  hashgen
  PUBLIC
  ${CMAKE_CURRENT_SOURCE_DIR}/include)

# Tests
add_executable(
  test_ast_print
  EXCLUDE_FROM_ALL
  test/ast_print/test.c
  src/ast_print.c
  src/token.c)

target_include_directories(
  test_ast_print
  PUBLIC
  ${CMAKE_CURRENT_SOURCE_DIR}/include)
