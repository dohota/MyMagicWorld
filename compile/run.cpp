#include <iostream>
#include <filesystem>
#include <vector>
#include <string>
#include <cstdlib> // std::system
// 该文件可以编译为跨平台的可执行文件（一个简单的控制台程序：决定软件怎么编译/库怎么链接/最后运行，可以参考cmake，scons）
// 用于编译，运行程序。这样理论上就不用cmake，ninja，ccache了
namespace fs = std::filesystem;
int main() {
    const std::string PROJECT_NAME = "MyWorld";
    fs::path SRC = "src/main.cpp";
    fs::path BUILD_DIR = "build";
    fs::path OUTPUT = BUILD_DIR / PROJECT_NAME;

    std::string CXX = "clang++";
    std::string CXX_STANDARD = "-std=c++20";

    std::vector<std::string> POSSIBLE_SDL_PATHS = {
        "/Users/karl/vcpkg/installed/arm64-osx",
        "/opt/homebrew/include/SDL2",
        "/usr/local",
        "/Users/karl/libs/SDL2",
        "/Users/karl/libs"
    };

    fs::path SDL2_INCLUDE;
    fs::path SDL2_LIB;
    bool found = false;

    for (const auto& base : POSSIBLE_SDL_PATHS) {
        fs::path inc = base + "/include";
        fs::path lib = base + "/lib";
        if (fs::exists(inc) && fs::exists(lib)) {
            SDL2_INCLUDE = inc;
            SDL2_LIB = lib;
            found = true;
            break;
        }
    }

    if (!found) {
        std::cerr << "Error: SDL2 not found\n";
        return 1;
    }

    // 创建 build/ 目录
    if (!fs::exists(BUILD_DIR)) {
        fs::create_directory(BUILD_DIR);
    }

    // 构建 clang++ 命令
    std::string cmd = CXX + " " + CXX_STANDARD + " -g -I" + SDL2_INCLUDE.string() + " "
                    + SRC.string() + " -o " + OUTPUT.string()
                    + " -L" + SDL2_LIB.string() + " -lSDL2 -lSDL2main -framework OpenGL";

    std::cout << "Build command: " << cmd << std::endl;

    // 执行编译
    int ret = std::system(cmd.c_str());
    if (ret != 0) {
        std::cerr << "Build failed!" << std::endl;
        return 1;
    }

    std::cout << "Build succeeded: " << OUTPUT << std::endl;
    std::cout << "Running program..." << std::endl;

    // 运行生成的程序
    ret = std::system(OUTPUT.string().c_str());

    return ret;
}

