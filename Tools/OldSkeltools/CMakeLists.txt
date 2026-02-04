cmake_minimum_required(VERSION 3.10)
project(skeltools)
message(STATUS "Found CMAKE-${CMAKE_MAJOR_VERSION}.${CMAKE_MINOR_VERSION}")
message(STATUS "----------------Running CMake---------------------------------")
cmake_host_system_information(RESULT HOSTNAME QUERY HOSTNAME)
message(STATUS "Invoking CMAKE on : ${HOSTNAME} running: ${CMAKE_HOST_SYSTEM}")

message(STATUS "Set ITK Directory to: " ${ITK_DIR})
find_package(ITK REQUIRED)
include(${ITK_USE_FILE})


set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_FLAGS "-g -Ofast -Wall -march=native")
set(CMAKE_EXE_LINKER_FLAGS "-rdynamic")
set(CMAKE_EXPORT_COMPILE_COMMANDS 1)

include_directories(include)

# Source

set(SOURCES
        main.cpp
        src/medial.cpp
        src/itkCommandLineArgumentParser.cxx
        src/topology.cpp)

add_executable(skeltool ${SOURCES})

target_link_libraries(skeltool ${ITK_LIBRARIES}  stdc++fs)
