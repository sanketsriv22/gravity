cmake_minimum_required(VERSION 3.16)

# Project name and language (added c for glad.c)
project(Gravity LANGUAGES CXX C)

# Require C++20
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Collect all sources (recursively)
file(GLOB_RECURSE SOURCES
    CMAKE_CONFIGURE_DEPENDS
    ${CMAKE_SOURCE_DIR}/src/*.cpp
    ${CMAKE_SOURCE_DIR}/utils/*.cpp
    ${CMAKE_SOURCE_DIR}/external/glad/src/*.c
)

message(STATUS "Found sources: ${SOURCES}")

# Add executable
add_executable(gravity ${SOURCES})

# Include directories
target_include_directories(gravity PRIVATE
    ${CMAKE_SOURCE_DIR}
    ${CMAKE_SOURCE_DIR}/external/glad/include
    ${CMAKE_SOURCE_DIR}/utils
)

find_package(glfw3 REQUIRED)
target_link_libraries(gravity PRIVATE glfw)

