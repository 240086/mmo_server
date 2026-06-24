# /home/xj/projects/mmo_server/server_core/tests/runtime/JobDispatch/CMakeLists.txt
cmake_minimum_required(VERSION 3.22)

# =================================================================
# 1. 汇聚任务分发微架构验证与高并发多线程测试源文件
# =================================================================
set(JOB_DISPATCH_TEST_SOURCES
    JobDispatchTests.cpp
    TaskRegistryTests.cpp
    WorkerPoolTests.cpp
)

# 创建工作任务分发微架构验证可执行目标
add_executable(job_dispatch_tests ${JOB_DISPATCH_TEST_SOURCES})

# 强制开启测试目标 C++20 支持
target_compile_features(job_dispatch_tests PRIVATE cxx_std_20)

# =================================================================
# 2. 契约级链条：第三方依赖、底层静态库与 GoogleTest 框架强绑定
# =================================================================
find_package(Threads REQUIRED)

target_link_libraries(job_dispatch_tests
    PRIVATE
    mmo_runtime_job_dispatch # 🎯 核心测试目标
    gtest
    gtest_main
    Threads::Threads
)

# =================================================================
# 3. 严苛编译选项对齐
# =================================================================
target_compile_options(job_dispatch_tests PRIVATE
    -Wall -Wextra -Wpedantic -Werror -Wshadow -Wconversion
)

# =================================================================
# 4. 工业级 Sanitizer 门禁分流控制（🛡️ 强力穿透传递给整个流水线生命周期依赖）
# =================================================================
if(MMO_ENABLE_ASAN AND MMO_ENABLE_TSAN)
    message(FATAL_ERROR "💥 架构级冲突: 内存卫士(ASan)与线程竞态卫士(TSan)无法同时启用！请分流构建。")
endif()

if(MMO_ENABLE_ASAN)
    message(STATUS "🛡️ [JobDispatchTest] 激活防护: AddressSanitizer & UndefinedBehaviorSanitizer 已挂载")

    set(ASAN_OPTIONS_LIST -fsanitize=address -fsanitize=undefined -fno-omit-frame-pointer)

    target_compile_options(job_dispatch_tests PRIVATE ${ASAN_OPTIONS_LIST})
    target_link_options(job_dispatch_tests PRIVATE -fsanitize=address -fsanitize=undefined)

    target_compile_options(mmo_runtime_job_dispatch PRIVATE ${ASAN_OPTIONS_LIST})
    target_link_options(mmo_runtime_job_dispatch PRIVATE -fsanitize=address -fsanitize=undefined)

elseif(MMO_ENABLE_TSAN)
    message(STATUS "🚀 [JobDispatchTest] 激活防护: ThreadSanitizer (TSan 线程竞态检测) 已挂载")

    target_compile_options(job_dispatch_tests PRIVATE -fsanitize=thread -fno-omit-frame-pointer)
    target_link_options(job_dispatch_tests PRIVATE -fsanitize=thread)

    target_compile_options(mmo_runtime_job_dispatch PRIVATE -fsanitize=thread -fno-omit-frame-pointer)
    target_link_options(mmo_runtime_job_dispatch PRIVATE -fsanitize=thread)
endif()

# =================================================================
# ⚡ 5. 硬件层算力榨干微架构优化
# =================================================================
if(CMAKE_BUILD_TYPE STREQUAL "Release")
    target_compile_options(job_dispatch_tests PRIVATE -O3 -march=native -fomit-frame-pointer)
    target_compile_options(mmo_runtime_job_dispatch PRIVATE -O3 -march=native -fomit-frame-pointer)
endif()

# =================================================================
# ⚖️ 6. 工业级 CTest 门禁分流精细化微操
# =================================================================
include(CTest)

# 【门禁 A】基础组件与注册表回归测试 (CI Fast Pass)
add_test(NAME Runtime_JobDispatch_Registry_Tests
    COMMAND job_dispatch_tests --gtest_filter=TaskRegistryTests.*
)
set_tests_properties(Runtime_JobDispatch_Registry_Tests PROPERTIES LABELS "Fast")

# 【门禁 B】线程池核心并发、伸缩性与分发吞吐压力测试 (CI Heavy Lock)
add_test(NAME Runtime_JobDispatch_Concurrency_Tests
    COMMAND job_dispatch_tests --gtest_filter=WorkerPoolTests.*:JobDispatchTests.*
)
set_tests_properties(Runtime_JobDispatch_Concurrency_Tests PROPERTIES LABELS "Heavy")