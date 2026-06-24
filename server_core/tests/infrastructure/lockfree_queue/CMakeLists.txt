# /home/xj/projects/mmo_server/server_core/tests/infrastructure/lockfree_queue/CMakeLists.txt
cmake_minimum_required(VERSION 3.22)

project(lockfree_queue_tests LANGUAGES CXX)

# =================================================================
# 1. 物理目录源码汇聚（区分 MPSC 与 MPMC 独立限界域）
# =================================================================
# MPSC 测试源文件
set(MPSC_SOURCES
    MPSC/ConsumerStarvationTests.cpp
    MPSC/FIFOOrderTests.cpp
    MPSC/FalseSharingPressureTests.cpp
    MPSC/FullQueueTests.cpp
    MPSC/LargePayloadTests.cpp
    MPSC/LongDurationStabilityTests.cpp
    MPSC/MultiProducerCorrectnessTests.cpp
    MPSC/ProducerBurstTests.cpp
    MPSC/StressTests.cpp
    MPSC/WrapAroundTests.cpp
)

# MPMC 测试源文件 (新扩张版图)
set(MPMC_SOURCES
    MPMC/MPMCQueueAbiTests.cpp
    MPMC/MPMCQueueBasicTests.cpp
    MPMC/MPMCQueueCapacityTests.cpp
    MPMC/MPMCQueueMultiProducerTests.cpp
    MPMC/MPMCQueueMultiConsumerTests.cpp
    MPMC/MPMCQueueContentionTests.cpp
    MPMC/MPMCQueueWrapAroundTests.cpp
    MPMC/MPMCQueueStressTests.cpp
    MPMC/MPMCQueueLargePayloadTests.cpp
)

# 创建无锁队列模块统一的架构验证可执行目标
add_executable(lockfree_queue_tests ${MPSC_SOURCES} ${MPMC_SOURCES})

# =================================================================
# 2. 契约级链条：第三方依赖、底层静态库与 GoogleTest 框架绑定
# =================================================================
find_package(Threads REQUIRED)

target_link_libraries(lockfree_queue_tests
    PRIVATE
    mmo::lockfree_queue # 🎯 待验证的核心无锁核心库
    gtest
    gtest_main
    Threads::Threads
)

# =================================================================
# 3. 严苛编译选项对齐与 TSan 防御宏挂载
# =================================================================
target_compile_options(lockfree_queue_tests PRIVATE
    -Wall -Wextra -Wpedantic -Werror -Wshadow -Wconversion
)

target_compile_definitions(lockfree_queue_tests PRIVATE
    BOOST_ASIO_USE_TSAN # 挂载网络/并发层兼容的 TSan 标帜
)

# =================================================================
# 4. 工业级 Sanitizer 门禁分流控制（🛡️ 智能解析 ALIAS 与目标类型强力穿透）
# =================================================================
if(MMO_ENABLE_ASAN AND MMO_ENABLE_TSAN)
    message(FATAL_ERROR "💥 架构级冲突: 内存卫士(ASan)与线程竞态卫士(TSan)无法同时启用！请分流构建。")
endif()

# 🎯 架构级修复一：解析 mmo::lockfree_queue 别名背后的真实物理 Target
set(REAL_QUEUE_TARGET "mmo::lockfree_queue")
get_target_property(IS_ALIAS mmo::lockfree_queue ALIASED_TARGET)

if(IS_ALIAS)
    set(REAL_QUEUE_TARGET ${IS_ALIAS})
endif()

# 🎯 架构级修复二：自省该目标的真实物理类型（防止接口库 INTERFACE 报错）
get_target_property(TARGET_TYPE ${REAL_QUEUE_TARGET} TYPE)

if(TARGET_TYPE STREQUAL "INTERFACE_LIBRARY")
    set(SCOPE_KEYWORD "INTERFACE")
else()
    set(SCOPE_KEYWORD "PRIVATE")
endif()

if(MMO_ENABLE_ASAN)
    message(STATUS "🛡️ [LockFreeQueueTest] 激活防护: AddressSanitizer & UndefinedBehaviorSanitizer 已挂载")

    set(ASAN_OPTIONS_LIST -fsanitize=address -fsanitize=undefined -fno-omit-frame-pointer)

    # 穿透应用给测试目标
    target_compile_options(lockfree_queue_tests PRIVATE ${ASAN_OPTIONS_LIST})
    target_link_options(lockfree_queue_tests PRIVATE -fsanitize=address -fsanitize=undefined)

    # 🎯 依据目标物理特性，自适应采用 INTERFACE 或 PRIVATE 注入选项
    target_compile_options(${REAL_QUEUE_TARGET} ${SCOPE_KEYWORD} ${ASAN_OPTIONS_LIST})
    target_link_options(${REAL_QUEUE_TARGET} ${SCOPE_KEYWORD} -fsanitize=address -fsanitize=undefined)

elseif(MMO_ENABLE_TSAN)
    message(STATUS "🚀 [LockFreeQueueTest] 激活防护: ThreadSanitizer (TSan 线程竞态检测) 已挂载")

    set(TSAN_OPTIONS_LIST -fsanitize=thread -fno-omit-frame-pointer)

    # 穿透应用给测试目标
    target_compile_options(lockfree_queue_tests PRIVATE ${TSAN_OPTIONS_LIST})
    target_link_options(lockfree_queue_tests PRIVATE -fsanitize=thread)

    # 🎯 依据目标物理特性，自适应采用 INTERFACE 或 PRIVATE 注入选项
    target_compile_options(${REAL_QUEUE_TARGET} ${SCOPE_KEYWORD} ${TSAN_OPTIONS_LIST})
    target_link_options(${REAL_QUEUE_TARGET} ${SCOPE_KEYWORD} -fsanitize=thread)
endif()

# =================================================================
# ⚡ 5. 硬件层算力榨干微架构优化（仅在无 Sanitizer 介入的 Release 下全力轰鸣）
# =================================================================
if(CMAKE_BUILD_TYPE STREQUAL "Release" AND NOT MMO_ENABLE_ASAN AND NOT MMO_ENABLE_TSAN)
    message(STATUS "🔥 [LockFreeQueueTest] 解锁极致性能优化模式 (-O3 -march=native)")

    set(RELEASE_OPTS -O3 -march=native -fomit-frame-pointer)

    target_compile_options(lockfree_queue_tests PRIVATE ${RELEASE_OPTS})
    target_compile_options(${REAL_QUEUE_TARGET} ${SCOPE_KEYWORD} ${RELEASE_OPTS})
endif()

# =================================================================
# ⚖️ 6. 工业级 CTest 门禁分流精细化微操（划分为 MPSC 与 MPMC 独立流水线）
# =================================================================
include(CTest)

# ----------------- MPSC 门禁分流 -----------------
# 【MPSC 门禁 1】回归单元测试集
add_test(NAME LockFreeQueue_MPSC_Functional_Tests
    COMMAND lockfree_queue_tests --gtest_filter=MPSCQueueTest.*
)
set_tests_properties(LockFreeQueue_MPSC_Functional_Tests PROPERTIES LABELS "Fast;CI;MPSC")

# 【MPSC 门禁 2】饥饿性与大负载长周期专项压测
add_test(NAME LockFreeQueue_MPSC_Stress_Tests
    COMMAND lockfree_queue_tests --gtest_filter=MPSCQueueStarvationTest.*:MPSCQueueRuntimeTest.*
)
set_tests_properties(LockFreeQueue_MPSC_Stress_Tests PROPERTIES
    LABELS "Queue_Freeze_Stress;MPSC" TIMEOUT 240 RUN_SERIAL TRUE)

# ----------------- MPMC 门禁分流 (新规) -----------------
# 【MPMC 门禁 1】ABI 规约与基础正确性 (CI 快车道)
add_test(NAME LockFreeQueue_MPMC_Functional_Tests
    COMMAND lockfree_queue_tests --gtest_filter=MPMCQueueAbiTests.*:MPMCQueueBasicTests.*:MPMCQueueCapacityTests.*
)
set_tests_properties(LockFreeQueue_MPMC_Functional_Tests PROPERTIES LABELS "Fast;CI;MPMC")

# 【MPMC 门禁 2】多端竞争、回绕与大负载专项（绞肉机门禁 - 独占物理核心）
add_test(NAME LockFreeQueue_MPMC_Contention_Stress_Tests
    COMMAND lockfree_queue_tests --gtest_filter=MPMCQueueMultiProducerTests.*:MPMCQueueMultiConsumerTests.*:MPMCQueueContentionTests.*:MPMCQueueWrapAroundTests.*
)
set_tests_properties(LockFreeQueue_MPMC_Contention_Stress_Tests PROPERTIES
    LABELS "Queue_Freeze_Stress;MPMC" TIMEOUT 120 RUN_SERIAL TRUE)

# 【MPMC 门禁 3】百万级大数据载荷与长期稳定性测试
add_test(NAME LockFreeQueue_MPMC_Stability_Stress_Tests
    COMMAND lockfree_queue_tests --gtest_filter=MPMCQueueLargePayloadTests.*:MPMCQueueStressTests.*
)
set_tests_properties(LockFreeQueue_MPMC_Stability_Stress_Tests PROPERTIES
    LABELS "Queue_Freeze_Stress;MPMC" TIMEOUT 180 RUN_SERIAL TRUE)