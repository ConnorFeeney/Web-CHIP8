file(GLOB_RECURSE source *.c)

add_executable(CHIP8 ${source})
target_include_directories(CHIP8 PRIVATE ../include)
target_link_libraries(CHIP8 PRIVATE raylib)
