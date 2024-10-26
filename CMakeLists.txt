cmake_minimum_required(VERSION 3.28 FATAL_ERROR)
project(fncxx)

# Enable export of compile commands for better tooling support
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
set(CMAKE_TOOLCHAIN_FILE "/home/ayman/vcpkg/scripts/buildsystems/vcpkg.cmake")

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(fmt CONFIG REQUIRED)
find_package(
  SFML
  COMPONENTS system window graphics
  REQUIRED)

find_path(TERMCOLOR_INCLUDE_DIRS "termcolor/termcolor.hpp")

add_executable(
  fncxx Grapher/Graphing.hpp functionParser/Types.hpp functionParser/Logger.hpp
        functionParser/Tokenizer.hpp functionParser/Tokenizer.cpp src/main.cc)

# Link libraries to the fncxx executable
target_link_libraries(fncxx PUBLIC fmt::fmt sfml-system sfml-window
                                   sfml-graphics)
