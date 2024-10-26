cmake_minimum_required(VERSION 3.28 FATAL_ERROR)
project(fncxx)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

if(CMAKE_CXX_COMPILER_ID MATCHES "GNU|CLANG")
  add_compile_options(-fvisibility=hidden)
endif()

find_package(fmt CONFIG REQUIRED)
find_package(
  SFML
  COMPONENTS system window graphics
  REQUIRED)

find_path(TERMCOLOR_INCLUDE_DIRS "termcolor/termcolor.hpp")

add_executable(
  fncxx Grapher/Graphing.hpp functionParser/Types.hpp functionParser/Logger.hpp
        functionParser/Tokenizer.hpp functionParser/Tokenizer.cpp src/main.cc)

add_compile_options(-O3)
target_link_libraries(fncxx PRIVATE fmt::fmt sfml-system sfml-window
                                    sfml-graphics)
