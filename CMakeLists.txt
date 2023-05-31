cmake_minimum_required(VERSION 3.23)
project(lox)

set(CMAKE_CXX_STANDARD 17)

file(GLOB SOURCE_FILES
    ${PROJECT_SOURCE_DIR}/main.cpp
    ${PROJECT_SOURCE_DIR}/lox/*.h
    ${PROJECT_SOURCE_DIR}/lox/*.cpp)

add_executable(lox ${SOURCE_FILES} )

target_include_directories(lox PUBLIC ${PROJECT_SOURCE_DIR})
