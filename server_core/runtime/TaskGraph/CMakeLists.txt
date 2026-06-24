# /home/xj/projects/mmo_server/server_core/runtime/TaskGraph/CMakeLists.txt
cmake_minimum_required(VERSION 3.22)

# =====================================================
# 1. 定义路径前缀（严防硬编码污染，对齐现代化多层目录结构）
# =====================================================
set(TASK_GRAPH_INC_BASE "include/mmo/runtime/TaskGraph")
set(TASK_GRAPH_SRC_BASE "src")

# =====================================================
# 2. 显式列出所有头文件（全量暴露，维护极佳的现代编译契约与IDE感知）
# =====================================================
set(TASK_GRAPH_HEADERS
    "${TASK_GRAPH_INC_BASE}/TaskDependency.hpp"
    "${TASK_GRAPH_INC_BASE}/TaskGraph.hpp"
    "${TASK_GRAPH_INC_BASE}/TaskGraphConstants.hpp"
    "${TASK_GRAPH_INC_BASE}/TaskGraphTypes.hpp"
    "${TASK_GRAPH_INC_BASE}/TaskNode.hpp"
)

# =====================================================
# 3. 显式列出所有源文件（任务图常涉及动态拓扑排序与线程组激活，非全内联）
# =====================================================
set(TASK_GRAPH_SOURCES

    # 预留给未来动态调度、拓扑检测的编译单元，目前若为纯内联或空可暂不列
)

# =====================================================
# 4. 创建静态库目标
# =====================================================
add_library(mmo_runtime_task_graph STATIC
    ${TASK_GRAPH_HEADERS}
    ${TASK_GRAPH_SOURCES}
)

# =====================================================
# 5. 配置包含路径（实现现代 CMake 的 Interface 严格隔离）
# =====================================================
target_include_directories(mmo_runtime_task_graph
    PUBLIC

    # 暴露给外部模块（如 game_server）的根路径，使其能以 <mmo/runtime/TaskGraph/xxx.hpp> 引用
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>

    PRIVATE

    # 内部头文件子目录与 src 内部引用隔离
    ${CMAKE_CURRENT_SOURCE_DIR}/${TASK_GRAPH_INC_BASE}
    ${CMAKE_CURRENT_SOURCE_DIR}/src
)

# =====================================================
# 6. 核心运行时：硬核微架构硬件榨干优化与严苛编译选项
# =====================================================
target_compile_options(mmo_runtime_task_graph PRIVATE
    -O3
    -Wall
    -Wextra
    -Wpedantic
    -Werror
    -Wshadow # 🛡️ 拦截局部变量遮蔽
    -Wconversion # 🛡️ 拦截潜在的隐式类型转换截断
    -march=native # 解锁当前宿主机 CPU 的全部现代化原生硬件指令集，优化无锁原子操作与图遍历
)

# =====================================================
# 7. 契约级链条：横向/纵向依赖静态库强绑定
# =====================================================
target_link_libraries(mmo_runtime_task_graph
    PUBLIC
    mmo_infrastructure # 🔗 依赖 L1 基础设施（如 lockfree_queue 硬件对齐队列）
    mmo_runtime_phase

    PRIVATE
    Threads::Threads
)

# 给运行时 Target 设置规范别名（完全符合现代 C++ 命名空间设计哲学）
add_library(mmo::runtime_task_graph ALIAS mmo_runtime_task_graph)