# /home/xj/projects/mmo_server/server_core/infrastructure/lockfree_queue/CMakeLists.txt
cmake_minimum_required(VERSION 3.20)

# =====================================================
# 1. 定义路径前缀（简化深层目录书写）
# =====================================================
set(LFQ_INC_BASE "include/mmo/infrastructure/lockfree_queue")

# =====================================================
# 2. 显式列出所有头文件（Header Files）
# =====================================================
# 将头文件列入 target 有助于在 VS Code 的项目树中显示它们
set(LFQ_HEADERS
    "${LFQ_INC_BASE}/CacheAligned.hpp"
    "${LFQ_INC_BASE}/MPSCQueue.hpp"
    "${LFQ_INC_BASE}/QueueSlot.hpp"
    "${LFQ_INC_BASE}/QueueTypes.hpp"
    "${LFQ_INC_BASE}/MPMCQueue.hpp"
)

# =====================================================
# 3. 创建接口库目标（Header-only 必须使用 INTERFACE）
# =====================================================
add_library(mmo_lockfree_queue INTERFACE)

# 将头文件源绑定到接口，保证 IDE 能够正确索引和追踪文件变化
target_sources(mmo_lockfree_queue INTERFACE ${LFQ_HEADERS})

# =====================================================
# 4. 配置包含路径（Header Search Paths）
# =====================================================
target_include_directories(mmo_lockfree_queue
    INTERFACE

    # 使上层模块能以 <mmo/infrastructure/lockfree_queue/xxx.hpp> 引用
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
)

# =====================================================
# 5. 属性设置
# =====================================================
# 给 target 设置一个别名，方便在项目内通过 mmo::lockfree_queue 统一引用
add_library(mmo::lockfree_queue ALIAS mmo_lockfree_queue)