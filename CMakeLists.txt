cmake_minimum_required(VERSION 3.20)
project(DdDToVbi)

set(CMAKE_CXX_STANDARD 17)

add_executable(DdDToVbi main.cpp Worker.cpp Worker.h)

# needed to get the mpolib headers
find_package(mpo 2.0 REQUIRED)
# don't need to do "target_link_libraries" call for mpo because mpojson and mpozip both import it.

find_package(mpojson 2.0 REQUIRED)
target_link_libraries(DdDToVbi PUBLIC mpojson)

find_package(jansson REQUIRED)

# to help me figure out what these vars are set to (handy!)
message("CMAKE_SOURCE_DIR: ${CMAKE_SOURCE_DIR}")
message("CMAKE_CURRENT_SOURCE_DIR: ${CMAKE_CURRENT_SOURCE_DIR}")
message("CMAKE_BINARY_DIR: ${CMAKE_BINARY_DIR}")
message("CMAKE_CURRENT_BINARY_DIR: ${CMAKE_CURRENT_BINARY_DIR}")

if (MSVC)
	# see https://devblogs.microsoft.com/cppblog/msvc-now-correctly-reports-__cplusplus/
	target_compile_options(DdDToVbi PUBLIC "/Zc:__cplusplus")
endif()
