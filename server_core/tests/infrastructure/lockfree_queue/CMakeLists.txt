# /home/xj/projects/mmo_server/server_core/tests/infrastructure/lockfree_queue/CMakeLists.txt
cmake_minimum_required(VERSION 3.20)

# =================================================================
# 1. 声明所有基础功能单元测试（CI 门禁快车道 - 毫秒级返回）
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
# 2. 声明高负载压力与冻结级性能测试（绞肉机门禁 - 独占物理核心）
# =================================================================
set(STRESS_SOURCES
    LargePayloadTests.cpp
    LongDurationStabilityTests.cpp
    ConsumerStarvationTests.cpp
)

# 创建统一的验证可执行目标
add_executable(lockfree_queue_tests ${UT_SOURCES} ${STRESS_SOURCES})

# 🚀 链接核心无锁静态库与 GoogleTest 框架 🚀
target_link_libraries(lockfree_queue_tests
    PRIVATE
    mmo::lockfree_queue
    gtest
    gtest_main
)

# =================================================================
# 3. 严苛编译选项对齐（与 Net 模块看齐，挂载 TSan 防御宏）
# =================================================================
target_compile_definitions(lockfree_queue_tests PRIVATE
    BOOST_ASIO_USE_TSAN
)

# =================================================================
# ⚡ 4. 架构师硬核微架构硬件榨干优化（仅在 Release 模式生效）
# =================================================================
if(CMAKE_BUILD_TYPE STREQUAL "Release")
    target_compile_options(lockfree_queue_tests PRIVATE
        -O3 # 开启 GCC 最高聚合级别优化
        -march=native # 解锁宿主机 CPU 的所有原生现代化硬件指令集
        -fomit-frame-pointer # 剥离栈帧指针，将物理寄存器全部腾给原子无锁变量
    )
endif()

# =================================================================
# ⚖️ 5. 工业级精细化 CTest 门禁分流（更新为无锁队列独占标签）
# =================================================================
include(CTest)

# 【独立门禁 1】标准回归单元测试集
add_test(NAME LockFreeQueue_Functional_Tests
    COMMAND lockfree_queue_tests --gtest_filter=MPSCQueueTest.*
)
set_tests_properties(LockFreeQueue_Functional_Tests PROPERTIES LABELS "Fast;CI")

# 【独立门禁 2】饥饿性专项独立压测
add_test(NAME LockFreeQueue_Starvation_Stress_Test
    COMMAND lockfree_queue_tests --gtest_filter=MPSCQueueStarvationTest.ConsumerNeverStarves
)
set_tests_properties(LockFreeQueue_Starvation_Stress_Test PROPERTIES
    LABELS "Queue_Freeze_Stress" TIMEOUT 120 RUN_SERIAL TRUE)

# 【独立门禁 3】大对象所有权转移专项独立压测
add_test(NAME LockFreeQueue_LargePayload_Stress_Test
    COMMAND lockfree_queue_tests --gtest_filter=MPSCQueueRuntimeTest.LargePayloadTransfer
)
set_tests_properties(LockFreeQueue_LargePayload_Stress_Test PROPERTIES
    LABELS "Queue_Freeze_Stress" TIMEOUT 60 RUN_SERIAL TRUE)

# 【独立门禁 4】百万级回绕与序列号回收专项独立压测
add_test(NAME LockFreeQueue_Stability_Stress_Test
    COMMAND lockfree_queue_tests --gtest_filter=MPSCQueueRuntimeTest.LongDurationStability
)
set_tests_properties(LockFreeQueue_Stability_Stress_Test PROPERTIES
    LABELS "Queue_Freeze_Stress" TIMEOUT 120 RUN_SERIAL TRUE)