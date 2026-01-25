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
    src/system/raycast.cpp
    src/system/render.cpp
    src/system/collision.cpp
    src/system/entity.cpp
    src/component/manager.cpp
    src/entity/manager.cpp
    src/world/manager.cpp
)

# 使用 vcpkg toolchain 后，这一行能找到 bgfx
# find_package(bgfx CONFIG REQUIRED)
# 但我觉得配置vcpkg toolchain太麻烦，所以干脆把路径写死了！

# bgfx
set(BGFX_ROOT "/Users/karl/vcpkg/packages/bgfx_arm64-osx")
include_directories("${BGFX_ROOT}/include")
link_directories("${BGFX_ROOT}/lib")

# bx 和 bimg
include_directories("${BGFX_ROOT}/include")  # bx 和 bimg 都在 bgfx include 下
link_directories("${BGFX_ROOT}/lib")

target_include_directories(MyWorld PRIVATE
    ${SDL2_INCLUDE_DIRS}
)

# 添加编译宏
# 这样 bx 的 config.h 就能通过编译
add_compile_definitions(
    BX_CONFIG_DEBUG=1    # 如果你在 Debug 模式
)

if (APPLE)
    find_library(COCOA_FRAMEWORK Cocoa)
    find_library(METAL_FRAMEWORK Metal)
    find_library(FOUNDATION_FRAMEWORK Foundation)
    find_library(COREFOUNDATION_FRAMEWORK CoreFoundation)
    find_library(IOKIT_FRAMEWORK IOKit)
    find_library(QUARTZCORE_FRAMEWORK QuartzCore)
    target_link_libraries(MyWorld
        PRIVATE
            ${COCOA_FRAMEWORK}
            ${METAL_FRAMEWORK}
            ${FOUNDATION_FRAMEWORK}
            ${COREFOUNDATION_FRAMEWORK}
            ${IOKIT_FRAMEWORK}
            ${QUARTZCORE_FRAMEWORK}
            "${BGFX_ROOT}/lib/libbgfx.a"
            "${BGFX_ROOT}/lib/libbx.a"
            "${BGFX_ROOT}/lib/libbimg.a"
            SDL2::SDL2
            SDL2::SDL2main
    )
else()
    target_link_libraries(MyWorld
        PRIVATE
            "${BGFX_ROOT}/lib/libbgfx.a"
            "${BGFX_ROOT}/lib/libbx.a"
            "${BGFX_ROOT}/lib/libbimg.a"
            SDL2::SDL2
            SDL2::SDL2main
    )
endif()
# 不指定编译器（macOS 自动用 clang）
# Ninja 会被 CMake Tools 自动选
# CMake 有时还在用旧的 configure 缓存，则ctrl+shift+p：CMake: Delete Cache and Reconfigure 清除缓存
