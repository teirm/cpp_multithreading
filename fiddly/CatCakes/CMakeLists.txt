# Currently running cmake 3.19.2 -- so picked this version 
cmake_minimum_required(VERSION 3.19.0)

# Project name 
project(CatCakes)

# use clang++ for compilation rather than g++
set(CMAKE_CXX_COMPILER /usr/bin/clang++)

# set C++ standard to c++17
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED True)

# Source Directory is flag with all files at same level
add_executable(CatCakes 
               Bakery.cpp
               Cat.cpp
               Logger.cpp
               cat_cakes.cpp)

# Don't want gnu extensions to C++ standard
# Cmake defaults to this on
set_target_properties(CatCakes
                      PROPERTIES CXX_EXTENSIONS OFF)


# std::thread depends on pthread on linux and freebsd
# don't plan on compiling with Windows anytime soon
target_link_libraries(CatCakes PRIVATE pthread)
