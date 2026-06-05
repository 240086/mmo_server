# /home/xj/projects/mmo_server/CMakeLists.txt
cmake_minimum_required(VERSION 3.20)
project(mmo_server_cluster VERSION 0.1.0 LANGUAGES CXX)

# --- 全局配置 ---
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
set(CMAKE_THREAD_PREFER_PTHREAD TRUE)

find_package(Threads REQUIRED)

# --- 编译器警告防御 ---
if(MSVC)
    add_compile_options(/W4 /Zc:__cplusplus)
else()
    add_compile_options(-Wall -Wextra -Wpedantic -Wshadow -Wconversion -Wno-unused-parameter)
endif()

# --- 1. 开启全局测试 ---
enable_testing() 

# --- 2. 第三方库：Googletest (挪到测试开启后，且增加条件判断防御) ---
if(NOT TARGET gtest)
    set(INSTALL_GTEST OFF CACHE BOOL "" FORCE)
    set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)
    add_subdirectory(third_party/googletest)
endif()

# --- 3. 核心引擎层 ---
add_subdirectory(server_core)