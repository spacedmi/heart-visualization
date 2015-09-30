cmake_minimum_required(VERSION 2.8)	 

project(heart-visualization-multi)

find_package(MPI REQUIRED)
include_directories(${MPI_INCLUDE_PATH})

set(SOURCE_EXE main.cpp)

set(SOURCE_LIB Cell MyHeart Solver)

add_library(staticlib STATIC ${SOURCE_LIB})

add_executable(heart-visualization ${SOURCE_EXE})

target_link_libraries(heart-visualization staticlib ${MPI_LIBRARIES})