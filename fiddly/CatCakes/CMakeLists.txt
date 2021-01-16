cmake_minimum_required(VERSION 3.19.0)
project(CatCakes)
add_executable(CatCakes cat_cakes.cpp)
target_link_libraries(CatCakes PRIVATE pthread)
