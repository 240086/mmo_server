# /home/xj/projects/mmo_server/server_core/infrastructure/memory_pool/CMakeLists.txt
cmake_minimum_required(VERSION 3.22)

# =====================================================
# 1. 定义路径前缀
# =====================================================
set(MEMPOOL_INC_BASE "include/mmo/infrastructure/memory_pool")
set(MEMPOOL_SRC_BASE "src")

# =====================================================
# 2. 显式列出所有头文件（维护极佳的现代编译契约）
# =====================================================
set(MEMPOOL_HEADERS
    "${MEMPOOL_INC_BASE}/MemoryPoolTypes.hpp"
    "${MEMPOOL_INC_BASE}/MemoryBlock.hpp"
    "${MEMPOOL_INC_BASE}/IMemoryPool.hpp"
    "${MEMPOOL_INC_BASE}/FixedBlockPool.hpp"
)

# =====================================================
# 3. 显式列出所有源文件
# =====================================================
set(MEMPOOL_SOURCES
    "${MEMPOOL_SRC_BASE}/FixedBlockPool.cpp"
)

# =====================================================
# 4. 创建静态库目标
# =====================================================
add_library(mmo_memory_pool STATIC
    ${MEMPOOL_HEADERS}
    ${MEMPOOL_SOURCES}
)

# =====================================================
# 5. 配置包含路径（实现现代化现代 CMake 的 Interface 隔离）
# =====================================================
target_include_directories(mmo_memory_pool
    PUBLIC

    # 暴露给外部模块的根路径，使其他模块可以使用 <mmo/infrastructure/memory_pool/xxx.hpp>
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>

    PRIVATE

    # 内部头文件子目录与 src 内部引用隔离
    ${CMAKE_CURRENT_SOURCE_DIR}/${MEMPOOL_INC_BASE}
    ${CMAKE_CURRENT_SOURCE_DIR}/src
)

# =====================================================
# 6. 核心基础设施：硬核微架构硬件榨干优化与严苛编译选项
# =====================================================
target_compile_options(mmo_memory_pool PRIVATE
    -O3
    -Wall
    -Wextra
    -Wpedantic
    -Werror
    -Wshadow # 🛡️ 拦截局部变量遮蔽
    -Wconversion # 🛡️ 拦截潜在的 size_t 到 uint32_t 的隐式不安全截断
    -Wno-unused-parameter
    -march=native # 解锁当前宿主机 CPU 的全部现代化原生硬件指令集
)

target_link_libraries(mmo_memory_pool PUBLIC Threads::Threads)

# 给核心 Target 设置规范别名
add_library(mmo::memory_pool ALIAS mmo_memory_pool)