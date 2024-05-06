# CMakeList.txt : CMake project for sensormonitor, include source and define
# project specific logic here.
#
cmake_minimum_required (VERSION 3.0)
project(NAVFUSION VERSION 0.1.0 LANGUAGES C CXX)

# Add the cmake folder so the FindSphinx module is found
set(CMAKE_MODULE_PATH "${PROJECT_SOURCE_DIR}/cmake" ${CMAKE_MODULE_PATH})

add_subdirectory ("src")
