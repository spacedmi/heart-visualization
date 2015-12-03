cmake_minimum_required(VERSION 2.8)	 

project(heart-visualization-multi)

find_package(MPI REQUIRED)
include_directories(${MPI_INCLUDE_PATH})

set(VTK_DIR "~/libs/VTK-build")
find_package(VTK REQUIRED)
include(${VTK_USE_FILE})

set(SOURCE_EXE main.cpp)

set(SOURCE_LIB Cell MyHeart Solver DataProcessor)

add_library(staticlib STATIC ${SOURCE_LIB})

add_executable(heart-visualization ${SOURCE_EXE})

target_link_libraries(heart-visualization staticlib ${MPI_LIBRARIES})

if(VTK_LIBRARIES)
  target_link_libraries(heart-visualization ${VTK_LIBRARIES})
else()
  target_link_libraries(heart-visualization vtkHybrid vtkWidgets)
endif()
