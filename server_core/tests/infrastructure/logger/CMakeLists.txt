# /home/xj/projects/mmo_server/server_core/tests/infrastructure/logger/CMakeLists.txt
cmake_minimum_required(VERSION 3.20)

# =================================================================
# 1. 声明基础基础设施测试（验证 Sink 隔离、Level 过滤、异常不外逃，执行极快）
# =================================================================
set(INFRA_UT_SOURCES
    LoggerInfrastructureTests.cpp
)

# =================================================================
# 2. 声明长时多线程高压稳定性压测（全核心并发多线程不加锁吞吐性能对撞验证）
# =================================================================
set(STRESS_SOURCES
    LongDurationStressTests.cpp
)

# 创建统一的日志模块测试可执行目标
add_executable(logger_tests
    ${INFRA_UT_SOURCES}
    ${STRESS_SOURCES}
)

# 🚀 链接日志子模块静态库、底层无锁队列底座和 GTest 框架 🚀
target_link_libraries(logger_tests
    PRIVATE
    mmo_logger
    mmo_lockfree_queue
    gtest
    gtest_main
)

# =================================================================
# ⚡ 3. 架构师硬核微架构硬件榨干优化（仅在 Release 模式生效）
# =================================================================
if(CMAKE_BUILD_TYPE STREQUAL "Release")
    target_compile_options(logger_tests PRIVATE
        -O3
        -march=native
        -fomit-frame-pointer # 剥离栈帧指针，全力加速前台线程栈上并发格式化的响应时延
    )
endif()

# =================================================================
# 4. ⚖️ 工业级微操：在 CTest 中将它们拆分为不同的标签运行（分流隔离执行）
# =================================================================

# 【第一批次】标准基础单元测试：卡死在每次 Commit 的 CI 核心门禁（多线程并发，秒级完成）
add_test(NAME Logger_InfrastructureTest
    COMMAND logger_tests
    --gtest_filter=LoggerInfrastructureTests.*)
set_tests_properties(Logger_InfrastructureTest PROPERTIES LABELS "CI;UnitTest")

# 【第二批次】高抗压多线程长时稳定性压测（多线程激进并发、竞争吞吐验证、跑数分钟以上）
# 注意：此测试包含多线程极高负载，通常不放入 CI 门禁，由夜间构建（Nightly Build）或手动触发
add_test(NAME Logger_LongDurationStressTest
    COMMAND logger_tests
    --gtest_filter=LongDurationStressTests.*)
set_tests_properties(Logger_LongDurationStressTest PROPERTIES LABELS "Stress;Stability")