# /home/xj/projects/mmo_server/server_core/tests/runtime/RuntimePipelineExecutor/CMakeLists.txt
cmake_minimum_required(VERSION 3.22)

# =================================================================
# 1. 汇聚流水线执行器验证、生命周期与高并发/决定论测试源文件
# =================================================================
set(PIPELINE_EXECUTOR_TEST_SOURCES
    RuntimePipelineExecutorConstructionTests.cpp
    RuntimePipelineExecutorLifecycleTests.cpp
    RuntimePipelineExecutorExecutionTests.cpp
    RuntimePipelineExecutorFailureTests.cpp
    RuntimePipelineExecutorIntegrationTests.cpp
    RuntimePipelineExecutorDeterminismTests.cpp
)

# 创建流水线执行器验证可执行目标
add_executable(runtime_pipeline_executor_tests ${PIPELINE_EXECUTOR_TEST_SOURCES})

# 强制开启测试目标 C++20 支持
target_compile_features(runtime_pipeline_executor_tests PRIVATE cxx_std_20)

# 包含公共测试辅助头文件 (能够找到 RuntimePipelineExecutorTestCommon.hpp)
target_include_directories(runtime_pipeline_executor_tests PRIVATE ${CMAKE_CURRENT_SOURCE_DIR})

# =================================================================
# 2. 契约级链条：第三方依赖、底层静态库与 GoogleTest / GoogleMock 强绑定
# =================================================================
find_package(Threads REQUIRED)

target_link_libraries(runtime_pipeline_executor_tests
    PRIVATE
    mmo_runtime_pipeline_executor # 🎯 核心被测目标库 (请根据你真实的 target 名字微调)
    gtest
    gmock # 🎯 显式链接 gmock 支撑 MockJobDispatch
    gtest_main
    Threads::Threads
)

# =================================================================
# 3. 严苛编译选项对齐
# =================================================================
target_compile_options(runtime_pipeline_executor_tests PRIVATE
    -Wall -Wextra -Wpedantic -Werror -Wshadow -Wconversion
)

# =================================================================
# 4. 工业级 Sanitizer 门禁分流控制（🛡️ 强力穿透传递给整个流水线生命周期依赖）
# =================================================================
if(MMO_ENABLE_ASAN AND MMO_ENABLE_TSAN)
    message(FATAL_ERROR "💥 架构级冲突: 内存卫士(ASan)与线程竞态卫士(TSan)无法同时启用！请分流构建。")
endif()

if(MMO_ENABLE_ASAN)
    message(STATUS "🛡️ [PipelineExecutorTest] 激活防护: AddressSanitizer & UndefinedBehaviorSanitizer 已挂载")

    set(ASAN_OPTIONS_LIST -fsanitize=address -fsanitize=undefined -fno-omit-frame-pointer)

    target_compile_options(runtime_pipeline_executor_tests PRIVATE ${ASAN_OPTIONS_LIST})
    target_link_options(runtime_pipeline_executor_tests PRIVATE -fsanitize=address -fsanitize=undefined)

    target_compile_options(mmo_runtime_pipeline_executor PRIVATE ${ASAN_OPTIONS_LIST})
    target_link_options(mmo_runtime_pipeline_executor PRIVATE -fsanitize=address -fsanitize=undefined)

elseif(MMO_ENABLE_TSAN)
    message(STATUS "🚀 [PipelineExecutorTest] 激活防护: ThreadSanitizer (TSan 线程竞态检测) 已挂载")

    target_compile_options(runtime_pipeline_executor_tests PRIVATE -fsanitize=thread -fno-omit-frame-pointer)
    target_link_options(runtime_pipeline_executor_tests PRIVATE -fsanitize=thread)

    target_compile_options(mmo_runtime_pipeline_executor PRIVATE -fsanitize=thread -fno-omit-frame-pointer)
    target_link_options(mmo_runtime_pipeline_executor PRIVATE -fsanitize=thread)
endif()

# =================================================================
# ⚡ 5. 硬件层算力榨干微架构优化
# =================================================================
if(CMAKE_BUILD_TYPE STREQUAL "Release")
    target_compile_options(runtime_pipeline_executor_tests PRIVATE -O3 -march=native -fomit-frame-pointer)
    target_compile_options(mmo_runtime_pipeline_executor PRIVATE -O3 -march=native -fomit-frame-pointer)
endif()

# =================================================================
# ⚖️ 6. 工业级 CTest 门禁分流精细化微操
# =================================================================
include(CTest)

# 【门禁 A】基础生命周期、构造与异常失败契约回归测试 (CI Fast Pass)
add_test(NAME Runtime_PipelineExecutor_Core_Tests
    COMMAND runtime_pipeline_executor_tests --gtest_filter=RuntimePipelineExecutorConstructionTests.*:RuntimePipelineExecutorLifecycleTests.*:RuntimePipelineExecutorFailureTests.*
)
set_tests_properties(Runtime_PipelineExecutor_Core_Tests PROPERTIES LABELS "Fast")

# 【门禁 B】深度执行流、系统整合与高并发决定论验证 (CI Heavy Lock)
add_test(NAME Runtime_PipelineExecutor_Structural_Tests
    COMMAND runtime_pipeline_executor_tests --gtest_filter=RuntimePipelineExecutorExecutionTests.*:RuntimePipelineExecutorIntegrationTests.*:RuntimePipelineExecutorDeterminismTests.*
)
set_tests_properties(Runtime_PipelineExecutor_Structural_Tests PROPERTIES LABELS "Heavy")