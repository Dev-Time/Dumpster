cmake_minimum_required(VERSION 3.9)
project(Dumpster)

set(CMAKE_CXX_STANDARD 17)

add_executable(rm src/rm.cpp)
add_executable(dv src/dv.cpp)
add_executable(dump src/dump.cpp)