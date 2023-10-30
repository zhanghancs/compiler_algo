cmake_minimum_required( VERSION 2.8 )

project(RegularParser CXX)

set( CMAKE_CXX_FLAGS "-std=c++17")
set( CMAKE_BUILD_TYPE "Debug")
set( CMAKE_CXX_FLAGS_DEBUG "-g -Wall")

include_directories(
    ${PROJECT_SOURCE_DIR}/include
)

set(src
    ${PROJECT_SOURCE_DIR}/src/main.cpp
)

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${PROJECT_SOURCE_DIR}/bin)

add_executable(${PROJECT_NAME} ${src})