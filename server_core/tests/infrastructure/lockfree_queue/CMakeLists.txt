# /home/xj/projects/mmo_server/server_core/tests/infrastructure/lockfree_queue/CMakeLists.txt
cmake_minimum_required(VERSION 3.20)

# =================================================================
# 1. 声明所有基础功能单元测试（执行极快，用于 CI 门禁）
# =================================================================
set(UT_SOURCES
    FIFOOrderTests.cpp
    FullQueueTests.cpp
    MultiProducerCorrectnessTests.cpp
    WrapAroundTests.cpp
    StressTests.cpp
    ProducerBurstTests.cpp
    FalseSharingPressureTests.cpp
)

# =================================================================
# 2. 声明高负载压力与性能测试（执行较慢，高频吃 CPU）
# =================================================================
set(STRESS_SOURCES
    LargePayloadTests.cpp
    LongDurationStabilityTests.cpp
    ConsumerStarvationTests.cpp
)

# 创建统一的测试可执行目标
add_executable(lockfree_queue_tests ${UT_SOURCES} ${STRESS_SOURCES})

# 🚀 链接核心模块和 GTest 🚀
target_link_libraries(lockfree_queue_tests
    PRIVATE
    mmo::lockfree_queue
    gtest
    gtest_main
)

# =================================================================
# ⚡ 3. 架构师硬核微架构硬件榨干优化（仅在 Release 模式生效）
# =================================================================
if(CMAKE_BUILD_TYPE STREQUAL "Release")
    target_compile_options(lockfree_queue_tests PRIVATE 
        -O3                          # 开启 GCC 最高聚合级别优化
        -march=native                # 解锁你宿主机 CPU 的所有原生现代化硬件指令集
        -fomit-frame-pointer         # 剥离栈帧指针，将物理寄存器全部腾给原子无锁变量
    )
endif()

# =================================================================
# 4. ⚖️ 工业级微操：在 CTest 中将它们拆分为不同的标签运行
# =================================================================
# 注册标准的单元测试标签
add_test(NAME LockFreeQueue_UnitTest
    COMMAND lockfree_queue_tests --gtest_filter=MPSCQueueTest.*)
set_tests_properties(LockFreeQueue_UnitTest PROPERTIES LABELS "CI;UnitTest")

# 注册高负载的压测标签，并强行限制它们【单线程串行运行】，防止多用例并发导致系统锁死
# 💡 修复对齐：追加了 :*Stability* 和 :*Payload* 确保压测不漏网
add_test(NAME LockFreeQueue_StressTest
    COMMAND lockfree_queue_tests --gtest_filter=*Stress*:*Pressure*:*Starvation*:*Stability*:*Payload*)
set_tests_properties(LockFreeQueue_StressTest PROPERTIES LABELS "Stress" RUN_SERIAL TRUE)