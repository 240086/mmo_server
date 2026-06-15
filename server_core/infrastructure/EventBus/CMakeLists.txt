# /home/xj/projects/mmo_server/server_core/infrastructure/EventBus/CMakeLists.txt
cmake_minimum_required(VERSION 3.22)

# =====================================================
# 1. 定义路径前缀（严防硬编码污染）
# =====================================================
set(EVENTBUS_INC_BASE "include/mmo/infrastructure/EventBus")
set(EVENTBUS_SRC_BASE "src")

# =====================================================
# 2. 显式列出所有头文件（全量暴露，维护极佳的现代编译契约与IDE感知）
# =====================================================
set(EVENTBUS_HEADERS
    "${EVENTBUS_INC_BASE}/EventBusTypes.hpp"
    "${EVENTBUS_INC_BASE}/SubscriptionToken.hpp"
    "${EVENTBUS_INC_BASE}/IEventBus.hpp"
    "${EVENTBUS_INC_BASE}/EventBus.hpp"
)

# =====================================================
# 3. 显式列出所有源文件（不使用 GLOB，确保增量编译追踪的确定性）
# =====================================================
set(EVENTBUS_SOURCES
    "${EVENTBUS_SRC_BASE}/EventBus.cpp"
)

# =====================================================
# 4. 创建静态库目标
# =====================================================
add_library(mmo_event_bus STATIC
    ${EVENTBUS_HEADERS}
    ${EVENTBUS_SOURCES}
)

# =====================================================
# 5. 配置包含路径（实现现代 CMake 的 Interface 严格隔离）
# =====================================================
target_include_directories(mmo_event_bus
    PUBLIC

    # 暴露给外部模块的根路径，使其他模块可以使用 <mmo/infrastructure/EventBus/xxx.hpp>
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>

    PRIVATE

    # 内部头文件子目录与 src 内部引用隔离
    ${CMAKE_CURRENT_SOURCE_DIR}/${EVENTBUS_INC_BASE}
    ${CMAKE_CURRENT_SOURCE_DIR}/src
)

# =====================================================
# 6. 核心基础设施：硬核微架构硬件榨干优化与严苛编译选项
# =====================================================
target_compile_options(mmo_event_bus PRIVATE
    -O3
    -Wall
    -Wextra
    -Wpedantic
    -Werror
    -Wshadow # 🛡️ 拦截局部变量遮蔽
    -Wconversion # 🛡️ 拦截潜在的隐式类型转换截断
    -march=native # 解锁当前宿主机 CPU 的全部现代化原生硬件指令集，全力加速 FNV-1a 编译期哈希与无锁指针置换
)

# =====================================================
# 7. 契约级链条：EventBus 无锁快照与底层无锁原子组件的绑定
# =====================================================
find_package(Threads REQUIRED)

target_link_libraries(mmo_event_bus
    PUBLIC
    Threads::Threads # 💡 强绑定原生 pthread，保障快照临界区与写锁的安全存取

    PRIVATE

    # 按照架构设计，EventBus V1 主要处理解耦发布，若要进一步引入无锁组件，可在此处链接
    # mmo_lockfree_queue
)

# 给核心 Target 设置规范别名（完全符合现代 C++ 命名空间设计哲学）
add_library(mmo::event_bus ALIAS mmo_event_bus)