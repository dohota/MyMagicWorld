import subprocess # 用来 启动外部程序，在这里用来启动 clang++
import sys
from pathlib import Path # 现代化路径处理
PROJECT_NAME = "MyWorld"
SRC = Path("src/main.cpp") # 源文件路径
BUILD_DIR = Path("build") # 输出目录
OUTPUT = BUILD_DIR / PROJECT_NAME # 最终生成的可执行文件路径
CXX = "clang++" # 编译器配置
CXX_STANDARD = "-std=c++20"

SDL2_INCLUDE = None
SDL2_LIB = None
POSSIBLE_SDL_PATHS = [
    "/Users/karl/vcpkg/installed/arm64-osx",
    "/opt/homebrew/include/SDL2",
    "/usr/local",
    "/Users/karl/libs/SDL2",
    "/Users/karl/libs"
    ] # SDL2 可能被安装在这些地方之一
for base in POSSIBLE_SDL_PATHS: # 依次尝试可能的目录
    inc = Path(base) / "include"
    lib = Path(base) / "lib" # 拼出 include ，lib 路径
    if inc.exists() and lib.exists(): # 判断路径是否真的存在
        SDL2_INCLUDE = str(inc)
        SDL2_LIB = str(lib) # 找到就使用，然后退出循环
        break
if SDL2_INCLUDE is None:
    print("Error: SDL2 not found")
    sys.exit(1)
FRAMEWORKS = ["-framework", "OpenGL"] # macOS 特有写法 （macOS OpenGL）

BUILD_DIR.mkdir(exist_ok=True) # 如果 build/ 不存在 → 创建。    如果存在 → 不报错
cmd = [
        CXX,
        CXX_STANDARD,
        "-g",
        "-I", SDL2_INCLUDE,
        str(SRC),
        "-o", str(OUTPUT),
        "-L", SDL2_LIB,
        "-lSDL2",
        "-lSDL2main",
        *FRAMEWORKS,
    ] # 构建 clang++ 命令
print("Build command:".join(cmd)) # 打印命令（调试神器）

result = subprocess.run(cmd) # 执行命令（阻塞执行，等 clang++ 完成）
if result.returncode != 0: 
    print("Build failed！")
    sys.exit(1) # 构建失败

print("Build succeeded:", OUTPUT)
print("Running program...")
subprocess.run([str(OUTPUT)]) 
