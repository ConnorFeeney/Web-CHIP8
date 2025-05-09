cmake_minimum_required(VERSION 3.10)
project(CHIP8 VERSION 1.0 LANGUAGES C)

set(CMAKE_C_STANDARD 11)
set(CMAKE_C_STANDARD_REQUIRED ON)

add_subdirectory(src)
add_subdirectory(${CMAKE_SOURCE_DIR}/external/RayLib)

add_custom_command(
    OUTPUT ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/roms
    COMMAND ${CMAKE_COMMAND} -E copy_directory
            ${CMAKE_SOURCE_DIR}/roms
            ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/roms
    COMMENT "Copying assets folder"
)

add_custom_target(copy_assets ALL
    DEPENDS ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/roms
)