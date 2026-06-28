# /home/xj/projects/mmo_server/server_core/tests/runtime/TickScheduler/CMakeLists.txt
cmake_minimum_required(VERSION 3.22)

# =================================================================
# 1. 汇聚定时器主循环验证、决定论（Determinism）与极限稳定性测试源文件
# =================================================================
set(TICK_SCHEDULER_TEST_SOURCES
    TickSchedulerConstructionTests.cpp
    TickSchedulerContextTests.cpp
    TickSchedulerDeterminismTests.cpp
    TickSchedulerFailureTests.cpp
    TickSchedulerLifecycleTests.cpp
    TickSchedulerTickTests.cpp
    TickSchedulerTimerTests.cpp
    TickSchedulerStressTests.cpp # 🎯 稳定性压测
)

# 创建滴答调度器验证可执行目标
add_executable(tick_scheduler_tests ${TICK_SCHEDULER_TEST_SOURCES})

# 强制开启测试目标 C++20 支持
target_compile_features(tick_scheduler_tests PRIVATE cxx_std_20)

# 包含公共测试辅助头文件
target_include_directories(tick_scheduler_tests PRIVATE ${CMAKE_CURRENT_SOURCE_DIR})

# =================================================================
# 2. 契约级链条：第三方依赖、底层静态库与 GoogleTest 框架强绑定
# =================================================================
find_package(Threads REQUIRED)

target_link_libraries(tick_scheduler_tests
    PRIVATE
    mmo_runtime_tick_scheduler # 🎯 核心测试目标
    gtest
    gtest_main
    Threads::Threads
)

# =================================================================
# 3. 严苛编译选项对齐
# =================================================================
target_compile_options(tick_scheduler_tests PRIVATE
    -Wall -Wextra -Wpedantic -Werror -Wshadow -Wconversion
)

# =================================================================
# 4. 工业级 Sanitizer 门禁分流控制（🛡️ 强力穿透传递给整个流水线生命周期依赖）
# =================================================================
if(MMO_ENABLE_ASAN AND MMO_ENABLE_TSAN)
    message(FATAL_ERROR "💥 架构级冲突: 内存卫士(ASan)与线程竞态卫士(TSan)无法同时启用！请分流构建。")
endif()

if(MMO_ENABLE_ASAN)
    message(STATUS "🛡️ [TickSchedulerTest] 激活防护: AddressSanitizer & UndefinedBehaviorSanitizer 已挂载")

    set(ASAN_OPTIONS_LIST -fsanitize=address -fsanitize=undefined -fno-omit-frame-pointer)

    target_compile_options(tick_scheduler_tests PRIVATE ${ASAN_OPTIONS_LIST})
    target_link_options(tick_scheduler_tests PRIVATE -fsanitize=address -fsanitize=undefined)

    target_compile_options(mmo_runtime_tick_scheduler PRIVATE ${ASAN_OPTIONS_LIST})
    target_link_options(mmo_runtime_tick_scheduler PRIVATE -fsanitize=address -fsanitize=undefined)

elseif(MMO_ENABLE_TSAN)
    message(STATUS "🚀 [TickSchedulerTest] 激活防护: ThreadSanitizer (TSan 线程竞态检测) 已挂载")

    target_compile_options(tick_scheduler_tests PRIVATE -fsanitize=thread -fno-omit-frame-pointer)
    target_link_options(tick_scheduler_tests PRIVATE -fsanitize=thread)

    target_compile_options(mmo_runtime_tick_scheduler PRIVATE -fsanitize=thread -fno-omit-frame-pointer)
    target_link_options(mmo_runtime_tick_scheduler PRIVATE -fsanitize=thread)
endif()

# =================================================================
# ⚡ 5. 硬件层算力榨干微架构优化
# =================================================================
if(CMAKE_BUILD_TYPE STREQUAL "Release")
    target_compile_options(tick_scheduler_tests PRIVATE -O3 -march=native -fomit-frame-pointer)
    target_compile_options(mmo_runtime_tick_scheduler PRIVATE -O3 -march=native -fomit-frame-pointer)
endif()

# =================================================================
# ⚖️ 6. 工业级 CTest 门禁分流精细化微操
# =================================================================
include(CTest)

# 【门禁 A】基础生命周期、基础滴答与契约回归测试 (CI Fast Pass)
add_test(NAME Runtime_TickScheduler_Core_Tests
    COMMAND tick_scheduler_tests --gtest_filter=TickSchedulerConstructionTests.*:TickSchedulerContextTests.*:TickSchedulerLifecycleTests.*:TickSchedulerTickTests.*
)
set_tests_properties(Runtime_TickScheduler_Core_Tests PROPERTIES LABELS "Fast")

# 【门禁 B】高精度决定论验证、定时器整合与万次帧抖动压力测试 (CI Heavy Lock)
add_test(NAME Runtime_TickScheduler_Structural_Tests
    COMMAND tick_scheduler_tests --gtest_filter=TickSchedulerDeterminismTests.*:TickSchedulerTimerTests.*:TickSchedulerStressTests.*:TickSchedulerFailureTests.*
)
set_tests_properties(Runtime_TickScheduler_Structural_Tests PROPERTIES LABELS "Heavy")