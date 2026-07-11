cmake_minimum_required(VERSION 3.16)
project(NeonTetris LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)

include(FetchContent)
FetchContent_Declare(sfml
    GIT_REPOSITORY https://github.com/SFML/SFML.git
    GIT_TAG 2.6.1
    GIT_SHALLOW TRUE)
set(SFML_BUILD_AUDIO OFF CACHE BOOL "" FORCE)
set(SFML_BUILD_NETWORK OFF CACHE BOOL "" FORCE)
set(SFML_BUILD_EXAMPLES OFF CACHE BOOL "" FORCE)
set(SFML_BUILD_DOC OFF CACHE BOOL "" FORCE)
FetchContent_MakeAvailable(sfml)

add_executable(neon_tetris
    src/main.cpp
    src/Game.cpp
    src/Board.cpp
    src/Renderer.cpp
)
target_include_directories(neon_tetris PRIVATE include)
target_link_libraries(neon_tetris PRIVATE sfml-graphics sfml-window sfml-system)

if(APPLE)
    target_compile_options(neon_tetris PRIVATE -Wall -Wextra -Wpedantic)
elseif(MSVC)
    target_compile_options(neon_tetris PRIVATE /W4)
else()
    target_compile_options(neon_tetris PRIVATE -Wall -Wextra -Wpedantic)
endif()

