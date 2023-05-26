cmake_minimum_required(VERSION 3.5)
PROJECT(jps)

set(CMAKE_CXX_STANDARD 11)
add_compile_options(-std=c++11 -g)
# get version
string(TIMESTAMP UVERSION "8.%m.%d")
add_definitions(-DUVERSION="${UVERSION}")
message(STATUS "version = ${UVERSION}")


add_executable(jps jps.cpp ${Incs})

