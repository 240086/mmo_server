# /home/xj/projects/mmo_server/server_core/runtime/RuntimePipelineExecutor/CMakeLists.txt
cmake_minimum_required(VERSION 3.22)

# =====================================================
# 1. 定义路径前缀（严防硬编码污染，对齐现代化多层目录结构）
# =====================================================
set(PIPELINE_EXECUTOR_INC_BASE "include/mmo/runtime/RuntimePipelineExecutor")
set(PIPELINE_EXECUTOR_SRC_BASE "src")

# =====================================================
# 2. 显式列出所有头文件（全量暴露，维护极佳的现代编译契约与IDE感知）
# =====================================================
set(PIPELINE_EXECUTOR_HEADERS
    "${PIPELINE_EXECUTOR_INC_BASE}/IRuntimePipelineExecutor.hpp"
    "${PIPELINE_EXECUTOR_INC_BASE}/RuntimePipelineExecutor.hpp"
    "${PIPELINE_EXECUTOR_INC_BASE}/RuntimePipelineExecutorConfig.hpp"
    "${PIPELINE_EXECUTOR_INC_BASE}/RuntimePipelineExecutorTypes.hpp"
)

# =====================================================
# 3. 显式列出所有源文件（流水线核心状态机驱动器）
# =====================================================
set(PIPELINE_EXECUTOR_SOURCES
    "${PIPELINE_EXECUTOR_SRC_BASE}/RuntimePipelineExecutor.cpp"
)

# =====================================================
# 4. 创建静态库目标
# =====================================================
add_library(mmo_runtime_pipeline_executor STATIC
    ${PIPELINE_EXECUTOR_HEADERS}
    ${PIPELINE_EXECUTOR_SOURCES}
)

# =====================================================
# 5. 配置包含路径（实现现代 CMake 的 Interface 严格隔离）
# =====================================================
target_include_directories(mmo_runtime_pipeline_executor
    PUBLIC

    # 暴露给外部模块的根路径，使其能以 <mmo/runtime/RuntimePipelineExecutor/xxx.hpp> 引用
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>

    PRIVATE

    # 内部头文件子目录与 src 内部引用隔离
    ${CMAKE_CURRENT_SOURCE_DIR}/${PIPELINE_EXECUTOR_INC_BASE}
    ${CMAKE_CURRENT_SOURCE_DIR}/src
)

# =====================================================
# 6. 核心运行时：硬核微架构硬件榨干优化与严苛编译选项 (C++20)
# =====================================================
target_compile_features(mmo_runtime_pipeline_executor PRIVATE cxx_std_20)

target_compile_options(mmo_runtime_pipeline_executor PRIVATE
    -O3
    -Wall
    -Wextra
    -Wpedantic
    -Werror
    -Wshadow # 🛡️ 拦截局部变量遮蔽
    -Wconversion # 🛡️ 拦截潜在的隐式类型转换截断
    -march=native # 解锁当前宿主机 CPU 的全部现代化原生硬件指令集
)

# =====================================================
# 7. 契约级链条：横向/纵向依赖静态库强绑定
# =====================================================
find_package(Threads REQUIRED)

target_link_libraries(mmo_runtime_pipeline_executor
    PUBLIC
    mmo_runtime_pipeline # 🔗 核心流水线依赖
    mmo_runtime_context # 🔗 核心上下文快照
    mmo_runtime_phase # 🔗 步进驱动依赖 Phase
    mmo_runtime_job_dispatch # 🔗 驱动高并发并行作业分发
    mmo_runtime_task_graph # 🔗 任务拓扑编排依赖
    mmo_infrastructure # 🔗 L1 基础设施

    PRIVATE
    Threads::Threads
)

# 给运行时 Target 设置规范别名（完全符合现代 C++ 命名空间设计哲学）
add_library(mmo::runtime_pipeline_executor ALIAS mmo_runtime_pipeline_executor)