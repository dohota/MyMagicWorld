cmake_minimum_required(VERSION 3.21)
project(MyWorld LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(SDL2 CONFIG REQUIRED)
add_executable(MyWorld
    src/main.cpp
    src/world/world.cpp
    src/system/input.cpp
    src/system/manager.cpp
    src/system/move.cpp
    src/system/render.cpp
)


if (APPLE)
    target_link_libraries(MyWorld
        PRIVATE
            SDL2::SDL2
            SDL2::SDL2main
            "-framework OpenGL"
    )
else()
    target_link_libraries(MyWorld
        PRIVATE
            SDL2::SDL2
            SDL2::SDL2main
            OpenGL::GL
    )
endif()
# 不指定编译器（macOS 自动用 clang）
# Ninja 会被 CMake Tools 自动选

# CMake 有时还在用旧的 configure 缓存，则ctrl+shift+p：CMake: Delete Cache and Reconfigure 清除缓存
