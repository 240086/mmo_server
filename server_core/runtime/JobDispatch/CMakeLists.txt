# /home/xj/projects/mmo_server/server_core/runtime/JobDispatch/CMakeLists.txt
cmake_minimum_required(VERSION 3.22)

# =====================================================
# 1. 定义路径前缀（严防硬编码污染，对齐现代化多层目录结构）
# =====================================================
set(JOB_DISPATCH_INC_BASE "include/mmo/runtime/JobDispatch")
set(JOB_DISPATCH_SRC_BASE "src")

# =====================================================
# 2. 显式列出所有头文件（全量暴露，维护极佳的现代编译契约与IDE感知）
# =====================================================
set(JOB_DISPATCH_HEADERS
    "${JOB_DISPATCH_INC_BASE}/IJobDispatch.hpp"
    "${JOB_DISPATCH_INC_BASE}/ITaskCompletionSink.hpp"
    "${JOB_DISPATCH_INC_BASE}/ITaskExecutor.hpp"
    "${JOB_DISPATCH_INC_BASE}/JobDispatch.hpp"
    "${JOB_DISPATCH_INC_BASE}/JobDispatchConstants.hpp"
    "${JOB_DISPATCH_INC_BASE}/JobDispatchTypes.hpp"
    "${JOB_DISPATCH_INC_BASE}/TaskFunction.hpp"
    "${JOB_DISPATCH_INC_BASE}/TaskRegistry.hpp"
    "${JOB_DISPATCH_INC_BASE}/TaskRuntimeState.hpp"
    "${JOB_DISPATCH_INC_BASE}/WorkerContext.hpp"
    "${JOB_DISPATCH_INC_BASE}/WorkerPool.hpp"
    "${JOB_DISPATCH_INC_BASE}/WorkerPoolConfig.hpp"
    "${JOB_DISPATCH_INC_BASE}/WorkerThread.hpp"
)

# =====================================================
# 3. 显式列出所有源文件（工作线程池、任务注册表及分发核心）
# =====================================================
set(JOB_DISPATCH_SOURCES
    "${JOB_DISPATCH_SRC_BASE}/JobDispatch.cpp"
    "${JOB_DISPATCH_SRC_BASE}/TaskRegistry.cpp"
    "${JOB_DISPATCH_SRC_BASE}/WorkerPool.cpp"
)

# =====================================================
# 4. 创建静态库目标
# =====================================================
add_library(mmo_runtime_job_dispatch STATIC
    ${JOB_DISPATCH_HEADERS}
    ${JOB_DISPATCH_SOURCES}
)

# =====================================================
# 5. 配置包含路径（实现现代 CMake 的 Interface 严格隔离）
# =====================================================
target_include_directories(mmo_runtime_job_dispatch
    PUBLIC

    # 暴露给外部模块（如 game_server）的根路径，使其能以 <mmo/runtime/JobDispatch/xxx.hpp> 引用
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>

    PRIVATE

    # 内部头文件子目录与 src 内部引用隔离
    ${CMAKE_CURRENT_SOURCE_DIR}/${JOB_DISPATCH_INC_BASE}
    ${CMAKE_CURRENT_SOURCE_DIR}/src
)

# =====================================================
# 6. 核心运行时：硬核微架构硬件榨干优化与严苛编译选项 (C++20)
# =====================================================
target_compile_features(mmo_runtime_job_dispatch PRIVATE cxx_std_20)

target_compile_options(mmo_runtime_job_dispatch PRIVATE
    -O3
    -Wall
    -Wextra
    -Wpedantic
    -Werror
    -Wshadow # 🛡️ 拦截局部变量遮蔽
    -Wconversion # 🛡️ 拦截潜在的隐式类型转换截断
    -march=native # 解锁当前宿主机 CPU 的全部现代化原生硬件指令集，优化无锁原子操作与多线程同步
)

# =====================================================
# 7. 契约级链条：横向/纵向依赖静态库强绑定
# =====================================================
find_package(Threads REQUIRED)

target_link_libraries(mmo_runtime_job_dispatch
    PUBLIC
    mmo_infrastructure # 🔗 依赖 L1 基础设施
    mmo_runtime_context
    mmo_runtime_task_graph # 🎯 修复依赖：引入 TaskGraph 头文件搜索路径
    mmo_runtime_phase # 🎯 修复依赖：引入 RuntimePhase 头文件搜索路径
    

    PRIVATE
    Threads::Threads
)

# 给运行时 Target 设置规范别名（完全符合现代 C++ 命名空间设计哲学）
add_library(mmo::runtime_job_dispatch ALIAS mmo_runtime_job_dispatch)