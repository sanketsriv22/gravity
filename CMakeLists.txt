cmake_minimum_required(VERSION 3.16)

# Project name and language
project(Gravity LANGUAGES CXX)

# Require C++20
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Collect all sources (recursively)
file(GLOB_RECURSE SOURCES
    CMAKE_CONFIGURE_DEPENDS
    ${CMAKE_SOURCE_DIR}/main.cpp
    ${CMAKE_SOURCE_DIR}/utils/*.cpp
)

message(STATUS "Found sources: ${SOURCES}")

# Add executable
add_executable(gravity ${SOURCES})

# Include directories
target_include_directories(gravity PRIVATE
    ${CMAKE_SOURCE_DIR}
    ${CMAKE_SOURCE_DIR}/utils
)

# Link Raylib (assuming installed via Homebrew)
# find_package(raylib 5.0 REQUIRED)
# target_link_libraries(pong PRIVATE raylib)

