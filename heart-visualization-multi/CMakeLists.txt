cmake_minimum_required(VERSION 2.8)	 

project(heart-visualization-multi)

find_package(MPI REQUIRED)
#find_package(MPI)
include_directories(${MPI_INCLUDE_PATH})
#include_directories(SYSTEM ${MPI_INCLUDE_PATH})

set(VTK_DIR "VTK-build")
find_package(VTK REQUIRED)
include(${VTK_USE_FILE})

set(CMAKE_CXX_FLAGS "--std=c++11 ${CMAKE_CXX_FLAGS}")

set(SOURCE_EXE main.cpp)

set(SOURCE_LIB Cell MyHeart Solver DataProcessor mesh)

add_library(staticlib STATIC ${SOURCE_LIB})

add_executable(heart-visualization ${SOURCE_EXE})
#set(CMAKE_CXX_COMPILE_FLAGS ${CMAKE_CXX_COMPILE_FLAGS} ${MPI_COMPILE_FLAGS})
#set(CMAKE_CXX_LINK_FLAGS ${CMAKE_CXX_LINK_FLAGS} ${MPI_LINK_FLAGS})
target_link_libraries(heart-visualization staticlib ${MPI_LIBRARIES})

if(VTK_LIBRARIES)
  target_link_libraries(heart-visualization ${VTK_LIBRARIES})
else()
  target_link_libraries(heart-visualization vtkHybrid vtkWidgets)
endif()




