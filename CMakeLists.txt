cmake_minimum_required(VERSION 3.28 FATAL_ERROR)
project(fncxx)

# Enable export of compile commands for better tooling support
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
set(CMAKE_TOOLCHAIN_FILE "/home/ayman/vcpkg/scripts/buildsystems/vcpkg.cmake")
# Set C++ standard
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_COMPILER /usr/bin/g++)
set(VCPKG_CMAKE_SYSTEM_NAME "Linux")
# Find required packages
find_package(fmt CONFIG REQUIRED)
find_package(
  SFML
  COMPONENTS system window graphics
  REQUIRED)

# Locate the termcolor header
find_path(TERMCOLOR_INCLUDE_DIRS "termcolor/termcolor.hpp")

# Create shared library for the function parser
add_library(fnparser STATIC 
                            functionParser/Types.hpp functionParser/Logger.hpp functionParser/Tokenizer.hpp functionParser/Tokenizer.cpp
                            )
                      
add_library(fnGrapher STATIC  Grapher/Graphing.hpp)
target_link_libraries(fnGrapher PUBLIC fmt::fmt sfml-system sfml-window sfml-graphics fnparser)

# Specify properties for the fnparser target
set_target_properties(fnparser PROPERTIES LINKER_LANGUAGE CXX)

# Include directories for fnparser
target_include_directories(fnparser PRIVATE ${TERMCOLOR_INCLUDE_DIRS})
target_include_directories(fnparser
                           PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/functionParser)

target_link_directories(fnparser PRIVATE ${TERMCOLOR_INCLUDE_DIRS})
target_link_libraries(fnparser PRIVATE fmt::fmt)
# Create the main executable
add_executable(fncxx src/main.cc
        src/main.cc
        src/main.cc)

# Link libraries to the fncxx executable
target_link_libraries(fncxx PUBLIC fmt::fmt sfml-system sfml-window
                                    sfml-graphics fnparser fnGrapher)
