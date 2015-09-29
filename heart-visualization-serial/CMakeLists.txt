cmake_minimum_required(VERSION 2.8)	 

project(heart-visualization-serial)

set(SOURCE_EXE main.cpp)

set(SOURCE_LIB Cell MyHeart Solver)

add_library(staticlib STATIC ${SOURCE_LIB})

add_executable(main ${SOURCE_EXE})

target_link_libraries(main staticlib)
