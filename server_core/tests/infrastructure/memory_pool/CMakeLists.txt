# /home/xj/projects/mmo_server/server_core/tests/infrastructure/memory_pool/CMakeLists.txt
cmake_minimum_required(VERSION 3.20)

# =================================================================
# 1. 声明基础单元测试（验证基础 Allocate/Deallocate 的正确性，执行极快）
# =================================================================
set(UT_SOURCES
    FixedBlockPoolTests.cpp
)

# =================================================================
# 2. 预留给未来 V2/V3 演进的高负载压力与并发竞争测试源文件
# =================================================================
set(BOUNDARY_SOURCES
    MemoryPoolBoundaryTests.cpp
)

set(STRESS_SOURCES
    MemoryPoolStressTests.cpp
)

# 创建统一的内存池模块测试可执行目标
add_executable(memory_pool_tests
    ${UT_SOURCES}
    ${STRESS_SOURCES}
    ${BOUNDARY_SOURCES}
)

# 🚀 链接基础设施底座的 mmo_memory_pool 静态库和 GTest 框架 🚀
target_link_libraries(memory_pool_tests
    PRIVATE
    mmo_memory_pool
    gtest
    gtest_main
)

# =================================================================
# ⚡ 3. 架构师硬核微架构硬件榨干优化（仅在 Release 模式生效）
# =================================================================
if(CMAKE_BUILD_TYPE STREQUAL "Release")
    target_compile_options(memory_pool_tests PRIVATE
        -O3
        -march=native
        -fomit-frame-pointer # 剥离栈帧指针，将物理寄存器资源压榨给内存拷贝与指针偏移算力
    )
endif()

# =================================================================
# 4. ⚖️ 工业级微操：在 CTest 中将它们拆分为不同的标签运行（分流隔离执行）
# =================================================================

# 【第一批次】标准功能单元测试：匹配所有基础功能验证，多线程并发跑，卡死在每次 Commit 的 CI 门禁
add_test(NAME MemoryPool_UnitTest
    COMMAND memory_pool_tests
    --gtest_filter=FixedBlockPoolTests.*)
set_tests_properties(MemoryPool_UnitTest PROPERTIES LABELS "CI;UnitTest")

# 【第二批次】防御性边界测试（如 Double Free, Foreign Pointer 拦截测试）
add_test(NAME MemoryPool_BoundaryTest
    COMMAND memory_pool_tests
    --gtest_filter=MemoryPoolBoundaryTests.*)
set_tests_properties(MemoryPool_BoundaryTest PROPERTIES LABELS "Robustness")

# 【第三批次】高负载压力测试（串行运行，严禁与其他模块并跑吃满宿主机 CPU/内存）
add_test(NAME MemoryPool_StressTest
    COMMAND memory_pool_tests
    --gtest_filter=MemoryPoolStressTests.*)
set_tests_properties(MemoryPool_StressTest PROPERTIES LABELS "Stress" RUN_SERIAL TRUE)