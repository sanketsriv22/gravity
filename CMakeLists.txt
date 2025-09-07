cmake_minimum_required(VERSION 3.16)

# Project name and language
project(Gravity LANGUAGES CXX)

# Require C++20
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Collect all sources (recursively)
file(GLOB_RECURSE SOURCES
    CMAKE_CONFIGURE_DEPENDS
    ${CMAKE_SOURCE_DIR}/src/*.cpp
    ${CMAKE_SOURCE_DIR}/utils/*.cpp
)

message(STATUS "Found sources: ${SOURCES}")

# Add executable
add_executable(gravity src/main.cpp)

# Include directories
target_include_directories(gravity PRIVATE
    ${CMAKE_SOURCE_DIR}
    ${CMAKE_SOURCE_DIR}/utils
)

find_package(glfw3 REQUIRED)
target_link_libraries(gravity PRIVATE glfw)


# Link Raylib (assuming installed via Homebrew)
# find_package(raylib 5.0 REQUIRED)
# target_link_libraries(pong PRIVATE raylib)



