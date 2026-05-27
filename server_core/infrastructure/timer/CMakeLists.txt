# /home/xj/projects/mmo_server/server_core/infrastructure/timer/CMakeLists.txt
cmake_minimum_required(VERSION 3.20)

# 注意：project 通常只在根目录定义。在子模块中，我们直接定义 Target。
# 如果非要定义，建议与 Target 同名。

# =====================================================
# 1. 定义路径前缀（简化深层目录书写）
# =====================================================
set(TIMER_INC_BASE "include/mmo/infrastructure/timer")
set(TIMER_SRC_BASE "src")

# =====================================================
# 2. 显式列出所有头文件（Header Files）
# =================================/====================
# 将头文件列入 target 有助于在 VS Code/Visual Studio 的项目树中显示它们
set(TIMER_HEADERS
    "${TIMER_INC_BASE}/IClock.hpp"
    "${TIMER_INC_BASE}/SteadyClock.hpp"
    "${TIMER_INC_BASE}/TimerQueue.hpp"
    "${TIMER_INC_BASE}/TimerTask.hpp"
    "${TIMER_INC_BASE}/TimerTypes.hpp"
)

# =====================================================
# 3. 显式列出所有源文件（Source Files）
# =====================================================
set(TIMER_SOURCES
    "${TIMER_SRC_BASE}/SteadyClock.cpp"
    "${TIMER_SRC_BASE}/TimerQueue.cpp"
)

# =====================================================
# 4. 创建静态库目标
# =====================================================
add_library(mmo_timer STATIC
    ${TIMER_HEADERS}
    ${TIMER_SOURCES}
)

# =====================================================
# 5. 配置包含路径（Header Search Paths）
# =====================================================
target_include_directories(mmo_timer
    PUBLIC

    # 使上层模块能以 <mmo/infrastructure/timer/xxx.hpp> 引用
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
    PRIVATE

    # src 仅供模块内部 .cpp 引用
    ${CMAKE_CURRENT_SOURCE_DIR}/src
)

# =====================================================
# 6. 属性设置（可选）
# =====================================================
# 给 target 设置一个别名，方便在项目内通过 mmo::timer 引用（更具现代感的命名空间写法）
add_library(mmo::timer ALIAS mmo_timer)