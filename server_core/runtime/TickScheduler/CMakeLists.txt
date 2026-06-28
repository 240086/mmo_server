# /home/xj/projects/mmo_server/server_core/runtime/TickScheduler/CMakeLists.txt
cmake_minimum_required(VERSION 3.22)

# =====================================================
# 1. 定义路径前缀（严防硬编码污染，对齐现代化多层目录结构）
# =====================================================
set(TICK_SCHEDULER_INC_BASE "include/mmo/runtime/TickScheduler")
set(TICK_SCHEDULER_SRC_BASE "src")

# =====================================================
# 2. 显式列出所有头文件（全量暴露，维护极佳的现代编译契约与IDE感知）
# =====================================================
set(TICK_SCHEDULER_HEADERS
    "${TICK_SCHEDULER_INC_BASE}/ITickScheduler.hpp"
    "${TICK_SCHEDULER_INC_BASE}/TickScheduler.hpp"
    "${TICK_SCHEDULER_INC_BASE}/TickSchedulerConfig.hpp"
    "${TICK_SCHEDULER_INC_BASE}/TickSchedulerTypes.hpp"
)

# =====================================================
# 3. 显式列出所有源文件（主轴生命周期驱动器）
# =====================================================
set(TICK_SCHEDULER_SOURCES
    "${TICK_SCHEDULER_SRC_BASE}/TickScheduler.cpp"
)

# =====================================================
# 4. 创建静态库目标
# =====================================================
add_library(mmo_runtime_tick_scheduler STATIC
    ${TICK_SCHEDULER_HEADERS}
    ${TICK_SCHEDULER_SOURCES}
)

# =====================================================
# 5. 配置包含路径（实现现代 CMake 的 Interface 严格隔离）
# =====================================================
target_include_directories(mmo_runtime_tick_scheduler
    PUBLIC

    # 暴露给外部模块的根路径，使其能以 <mmo/runtime/TickScheduler/xxx.hpp> 引用
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>

    PRIVATE

    # 内部头文件子目录与 src 内部引用隔离
    ${CMAKE_CURRENT_SOURCE_DIR}/${TICK_SCHEDULER_INC_BASE}
    ${CMAKE_CURRENT_SOURCE_DIR}/src
)

# =====================================================
# 6. 核心运行时：硬核微架构硬件榨干优化与严苛编译选项 (C++20)
# =====================================================
target_compile_features(mmo_runtime_tick_scheduler PRIVATE cxx_std_20)

target_compile_options(mmo_runtime_tick_scheduler PRIVATE
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

target_link_libraries(mmo_runtime_tick_scheduler
    PUBLIC
    mmo_runtime_pipeline
    mmo_infrastructure # 🔗 L1 基础设施（包含 timer, lockfree_queue 等）
    mmo_runtime_context # 🔗 核心上下文，Tick 执行依赖环境快照
    mmo_runtime_phase # 🔗 Tick 内状态驱动依赖 Phase 步进
    mmo_runtime_job_dispatch # 🔗 驱动高并发并行作业分发
    mmo_runtime_task_graph # 🔗 将 Tick 内细分任务拓扑编排进 TaskGraph

    PRIVATE
    Threads::Threads
)

# 给运行时 Target 设置规范别名（完全符合现代 C++ 命名空间设计哲学）
add_library(mmo::runtime_tick_scheduler ALIAS mmo_runtime_tick_scheduler)