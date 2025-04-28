add_executable(CHIP8 main.c)
target_include_directories(CHIP8 PRIVATE {CMAKE_SOURCE_DIR}/src/include)


target_link_libraries(CHIP8 PRIVATE raylib)
