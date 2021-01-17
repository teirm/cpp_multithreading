# Currently running cmake 3.19.2 -- so picked this version 
cmake_minimum_required(VERSION 3.19.0)

# Project name 
project(CatCakes)

# Add custom build variants -- asan, ubsan, tsan
# check if a multi configuration generator is in use
get_property(isMultiConfig GLOBAL
    PROPERTY GENERATOR_IS_MULTI_CONFIG
)

if(isMultiConfig)
    if(NOT "ASAN" IN_LIST CMAKE_CONFIGURATION_TYPES)
        list(APPEND CMAKE_CONFIGURATION_TYPES Asan)
    endif()
    if(NOT "UBSAN" IN_LIST CMAKE_CONFIGURATION_TYPES)
        list(APPEND CMAKE_CONFIGURATION_TYPES Ubsan)
    endif()
    if(NOT "TSAN" IN_LIST CMAKE_CONFIGURATION_TYPES)
        list(APPEND CMAKE_CONFIGURATION_TYPES Tsan)
    endif()
else()
    set(allowedBuildTypes Debug Asan Release Ubsan Tsan)
    set_property(CACHE CMAKE_BUILD_TYPE PROPERTY
        STRINGS "${allowedBuildTypes}"
    )
    if(NOT CMAKE_BUILD_TYPE)
        set(CMAKE_BUILD_TYPE Debug CACHE STRING "" FORCE)
    elseif(NOT CMAKE_BUILD_TYPE IN_LIST allowedBuildTypes)
        message(FATAL_ERROR "Unknown build type: ${CMAKE_BUILD_TYPE}")
    endif()
endif()

# use clang++ for compilation rather than g++
set(CMAKE_CXX_COMPILER /usr/bin/clang++)

# set C++ standard to c++17
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED True)
set(CMAKE_CXX_FLAGS "-g -Wall -Wextra -Werror")

# add asan flags
set(CMAKE_C_FLAGS_ASAN              "-fsanitize=address -fno-omit-frame-pointer -fsanitize-address-use-after-scope" CACHE STRING "" FORCE)
set(CMAKE_CXX_FLAGS_ASAN            "-fsanitize=address -fno-omit-frame-pointer -fsanitize-address-use-after-scope" CACHE STRING "" FORCE)
set(CMAKE_EXE_LINKER_FLAGS_ASAN     "-fsanitize=address -fno-omit-frame-pointer -fsanitize-address-use-after-scope" CACHE STRING "" FORCE)
set(CMAKE_SHARED_LINKER_FLAGS_ASAN  "-fsanitize=address -fno-omit-frame-pointer -fsanitize-address-use-after-scope" CACHE STRING "" FORCE)
set(CMAKE_STATIC_LINKER_FLAGS_ASAN  "-fsanitize=address -fno-omit-frame-pointer -fsanitize-address-use-after-scope" CACHE STRING "" FORCE)
set(CMAKE_MODULE_LINKER_FLAGS_ASAN  "-fsanitize=address -fno-omit-frame-pointer -fsanitize-address-use-after-scope" CACHE STRING "" FORCE)

# add ubsan flags
set(CMAKE_C_FLAGS_UBSAN              "-g -fsanitize=undefined" CACHE STRING "" FORCE)
set(CMAKE_CXX_FLAGS_UBSAN            "-g -fsanitize=undefined" CACHE STRING "" FORCE)
set(CMAKE_EXE_LINKER_FLAGS_UBSAN     "-g -fsanitize=undefined" CACHE STRING "" FORCE)
set(CMAKE_SHARED_LINKER_FLAGS_UBSAN  "-g -fsanitize=undefined" CACHE STRING "" FORCE)
set(CMAKE_STATIC_LINKER_FLAGS_UBSAN  "-g -fsanitize=undefined" CACHE STRING "" FORCE)
set(CMAKE_MODULE_LINKER_FLAGS_UBSAN  "-g -fsanitize=undefined" CACHE STRING "" FORCE)

# add tsan flags
set(CMAKE_C_FLAGS_TSAN              "-g -fsanitize=thread" CACHE STRING "" FORCE)
set(CMAKE_CXX_FLAGS_TSAN            "-g -fsanitize=thread" CACHE STRING "" FORCE)
set(CMAKE_EXE_LINKER_FLAGS_TSAN     "-g -fsanitize=thread" CACHE STRING "" FORCE)
set(CMAKE_SHARED_LINKER_FLAGS_TSAN  "-g -fsanitize=thread" CACHE STRING "" FORCE)
set(CMAKE_STATIC_LINKER_FLAGS_TSAN  "-g -fsanitize=thread" CACHE STRING "" FORCE)
set(CMAKE_MODULE_LINKER_FLAGS_TSAN  "-g -fsanitize=thread" CACHE STRING "" FORCE)


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
