# /home/xj/projects/mmo_server/server_core/runtime/RuntimeContext/CMakeLists.txt
cmake_minimum_required(VERSION 3.22)

# =====================================================
# 1. 定义路径前缀（严防硬编码污染，对齐现代化多层目录结构）
# =====================================================
set(RUNTIME_CTX_INC_BASE "include/mmo/runtime/RuntimeContext")
set(RUNTIME_CTX_SRC_BASE "src")

# =====================================================
# 2. 显式列出所有头文件（全量暴露，维护极佳的现代编译契约与IDE感知）
# =====================================================
set(RUNTIME_CTX_HEADERS
    "${RUNTIME_CTX_INC_BASE}/RuntimeContext.hpp"
    "${RUNTIME_CTX_INC_BASE}/RuntimeMode.hpp"
    "${RUNTIME_CTX_INC_BASE}/RuntimeTypes.hpp"
)

# =====================================================
# 3. 显式列出所有源文件（不使用 GLOB，确保增量编译追踪的确定性）
# =====================================================
set(RUNTIME_CTX_SOURCES
)

# =====================================================
# 4. 创建静态库目标
# =====================================================
add_library(mmo_runtime_context STATIC
    ${RUNTIME_CTX_HEADERS}
    ${RUNTIME_CTX_SOURCES}
)

# =====================================================
# 5. 配置包含路径（实现现代 CMake 的 Interface 严格隔离）
# =====================================================
target_include_directories(mmo_runtime_context
    PUBLIC

    # 暴露给外部模块（如 game_server）的根路径，使其能以 <mmo/runtime/RuntimeContext/xxx.hpp> 引用
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>

    PRIVATE

    # 内部头文件子目录与 src 内部引用隔离
    ${CMAKE_CURRENT_SOURCE_DIR}/${RUNTIME_CTX_INC_BASE}
    ${CMAKE_CURRENT_SOURCE_DIR}/src
)

# =====================================================
# 6. 核心运行时：硬核微架构硬件榨干优化与严苛编译选项
# =====================================================
target_compile_options(mmo_runtime_context PRIVATE
    -O3
    -Wall
    -Wextra
    -Wpedantic
    -Werror
    -Wshadow # 🛡️ 拦截局部变量遮蔽
    -Wconversion # 🛡️ 拦截潜在的隐式类型转换截断
    -march=native # 解锁当前宿主机 CPU 的全部现代化原生硬件指令集，加速上下文调度
)

# =====================================================
# 7. 契约级链条：横向/纵向依赖静态库强绑定
# =====================================================
target_link_libraries(mmo_runtime_context
    PUBLIC

    # 🔗 纵向依赖链：RuntimeContext 作为生命周期宿主，直接组合 L1 基础设施生态
    mmo_infrastructure

    PRIVATE

    # 内部实现可能依赖的特定外部线程库
    Threads::Threads
)

# 给运行时 Target 设置规范别名（完全符合现代 C++ 命名空间设计哲学）
add_library(mmo::runtime_context ALIAS mmo_runtime_context)