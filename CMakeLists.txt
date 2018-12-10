cmake_minimum_required(VERSION 3.5.1)
project(main)
SET(CMAKE_C_COMPILER g++)
add_compile_options(-std=c++11)

find_package(OpenCV REQUIRED)
message(STATUS "OpenCV library status:")
message(STATUS "    version: ${OpenCV_VERSION}")
message(STATUS "    libraries: ${OpenCV_LIBS}")
message(STATUS "    include path: ${OpenCV_INCLUDE_DIRS}")
include_directories(${OpenCV_INCLUDE_DIRS})

add_subdirectory (functions)
set (EXTRA_LIBS ${EXTRA_LIBS} functions)

add_executable(main main.cpp)
target_link_libraries(main ${OpenCV_LIBS})
target_link_libraries(main ${EXTRA_LIBS})
