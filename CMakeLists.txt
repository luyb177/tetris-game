cmake_minimum_required(VERSION 3.16)
project(NeonTetrisWeb LANGUAGES CXX)

option(TETRIS_BUILD_SERVER "Build the C++ HTTP web backend" ON)

if(TETRIS_BUILD_SERVER)
    add_subdirectory(server)
endif()
