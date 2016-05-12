cmake_minimum_required(VERSION 2.8)

project(heart-visualization-serial)

set(VTK_DIR "VTK-build")
find_package(VTK REQUIRED)
include(${VTK_USE_FILE})

set(SOURCE_EXE main.cpp)

set(SOURCE_LIB Cell MyHeart Solver mesh)

add_library(staticlib STATIC ${SOURCE_LIB})

add_executable(main ${SOURCE_EXE})

target_link_libraries(main staticlib)

if(VTK_LIBRARIES)
  target_link_libraries(main ${VTK_LIBRARIES})
else()
  target_link_libraries(main vtkHybrid vtkWidgets)
endif()

include(CheckCXXCompilerFlag)
CHECK_CXX_COMPILER_FLAG("-std=c++11" COMPILER_SUPPORTS_CXX11)
CHECK_CXX_COMPILER_FLAG("-std=c++0x" COMPILER_SUPPORTS_CXX0X)
if (COMPILER_SUPPORTS_CXX11)
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11")
elseif (COMPILER_SUPPORTS_CXX0X)
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++0x")
else()
        message(STATUS "The compiler ${CMAKE_CXX_COMPILER} has no c++11 support. Please use a different c++ compiler.")
endif()
