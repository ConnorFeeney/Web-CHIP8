cmake_minimum_required(VERSION 3.10)
project(CHIP8 VERSION 1.0 LANGUAGES C)

set(CMAKE_C_STANDARD 11)
set(CMAKE_C_STANDARD_REQUIRED ON)

add_subdirectory(src)
add_subdirectory(${CMAKE_SOURCE_DIR}/external/RayLib)