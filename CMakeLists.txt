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

if(LOX_ENABLE_SANITISERS)
  add_compile_options(-fsanitize=address,leak,undefined)
  add_link_options(-fsanitize=address,leak,undefined)
endif()

add_executable(
  clox
  src/lox.c
  src/token.c
  src/scanner.c)

target_include_directories(
  clox
  PUBLIC
  ${CMAKE_CURRENT_SOURCE_DIR}/include)
