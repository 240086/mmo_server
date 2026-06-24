# /home/xj/projects/mmo_server/server_core/runtime/RuntimePipeline/CMakeLists.txt
cmake_minimum_required(VERSION 3.22)

# =====================================================
# 1. 定义路径前缀（严防硬编码污染，对齐现代化多层目录结构）
# =====================================================
set(RUNTIME_PIPELINE_INC_BASE "include/mmo/runtime/RuntimePipeline")
set(RUNTIME_PIPELINE_SRC_BASE "src")

# =====================================================
# 2. 显式列出所有头文件（全量暴露，维护极佳的现代编译契约与IDE感知）
# =====================================================
set(RUNTIME_PIPELINE_HEADERS
    "${RUNTIME_PIPELINE_INC_BASE}/RuntimePipeline.hpp"
)

# =====================================================
# 3. 显式列出所有源文件（流水线核心状态机逻辑通常涉及拓扑排序与解耦，不宜全内联）
# =====================================================
set(RUNTIME_PIPELINE_SOURCES
)

# =====================================================
# 4. 创建静态库目标
# =====================================================
add_library(mmo_runtime_pipeline STATIC
    ${RUNTIME_PIPELINE_HEADERS}
    ${RUNTIME_PIPELINE_SOURCES}
)

# =====================================================
# 5. 配置包含路径（实现现代 CMake 的 Interface 严格隔离）
# =====================================================
target_include_directories(mmo_runtime_pipeline
    PUBLIC

    # 暴露给外部模块（如 game_server）的根路径，使其能以 <mmo/runtime/RuntimePipeline/xxx.hpp> 引用
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>

    PRIVATE

    # 内部头文件子目录与 src 内部引用隔离
    ${CMAKE_CURRENT_SOURCE_DIR}/${RUNTIME_PIPELINE_INC_BASE}
    ${CMAKE_CURRENT_SOURCE_DIR}/src
)

# =====================================================
# 6. 核心运行时：硬核微架构硬件榨干优化与严苛编译选项
# =====================================================
target_compile_options(mmo_runtime_pipeline PRIVATE
    -O3
    -Wall
    -Wextra
    -Wpedantic
    -Werror
    -Wshadow        # 🛡️ 拦截局部变量遮蔽
    -Wconversion    # 🛡️ 拦截潜在的隐式类型转换截断
    -march=native   # 解锁当前宿主机 CPU 的全部现代化原生硬件指令集，加速流水线驱动与分支预测优化
)

# =====================================================
# 7. 契约级链条：横向/纵向依赖静态库强绑定（流水线聚合核心上下文与阶段）
# =====================================================
target_link_libraries(mmo_runtime_pipeline
    PUBLIC

    # 🔗 纵向依赖链：深度绑定流水线所需的上下文与阶段状态机
    mmo_runtime_context
    mmo_runtime_phase

    PRIVATE

    # 内部多线程调度可能依赖的特定系统库
    Threads::Threads
)

# 给运行时 Target 设置规范别名（完全符合现代 C++ 命名空间设计哲学）
add_library(mmo::runtime_pipeline ALIAS mmo_runtime_pipeline)