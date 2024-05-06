# CMakeList.txt : CMake project for sensormonitor, include source and define
# project specific logic here.
#
cmake_minimum_required (VERSION 3.0)
project(navfusion VERSION 0.1.0 LANGUAGES C CXX)
set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(NAVFUSION_SRC_ROOT "./")
include_directories("${NAVFUSION_SRC_ROOT}")


# SET ARMADILLO PATH NAME HERE AS: $ENV{YOUR_ARMADILLO_ROOT_NAME}
#include_directories("$ENV{ARMADILLO_ROOT}/include")
#link_libraries("$ENV{ARMADILLO_ROOT}/examples/lib_win64/libopenblas.lib")


# COMMENT
include_directories("../libs/include")
# COMMENT
get_filename_component(LIB_DIR_ABSOLUTE "${CMAKE_CURRENT_SOURCE_DIR}/../libs" ABSOLUTE)
# COMMENT
link_directories(${LIB_DIR_ABSOLUTE})


set(NAVFUSION_SRC
#${NAVFUSION_SRC_ROOT}/general/general.cpp
${NAVFUSION_SRC_ROOT}/interface/io/files/io_files.cpp
${NAVFUSION_SRC_ROOT}/interface/io/in/io_in.cpp
${NAVFUSION_SRC_ROOT}/interface/io/out/io_out.cpp
${NAVFUSION_SRC_ROOT}/interface/navdata/interface_navdata.cpp
${NAVFUSION_SRC_ROOT}/interface/ui/ui.cpp
${NAVFUSION_SRC_ROOT}/monitor/monitor.cpp
${NAVFUSION_SRC_ROOT}/processing/frames/frames.cpp
${NAVFUSION_SRC_ROOT}/processing/kf/proc_kf.cpp
${NAVFUSION_SRC_ROOT}/processing/system/proc_system.cpp
${NAVFUSION_SRC_ROOT}/processing/system/proc_system_helper.cpp
${NAVFUSION_SRC_ROOT}/processing/system/fusion/proc_system_fusion.cpp
${NAVFUSION_SRC_ROOT}/processing/system/ins/proc_system_ins.cpp
${NAVFUSION_SRC_ROOT}/processing/system/gnss/proc_system_gnss.cpp
${NAVFUSION_SRC_ROOT}/main.cpp
)

# Add source to this project's executable.
add_executable (navfusion ${NAVFUSION_SRC})

# COMMENT
target_link_libraries(navfusion PRIVATE libopenblas)

target_include_directories(navfusion PUBLIC .)
