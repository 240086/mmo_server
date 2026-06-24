cmake_minimum_required(VERSION 3.22)

# =================================================================
# 1. 汇聚任务图微架构验证与 ABI 稳定性契约测试源文件
# =================================================================
set(TASK_GRAPH_TEST_SOURCES
    TaskGraphAbiTests.cpp
)

# 创建任务图微架构验证可执行目标
add_executable(task_graph_tests ${TASK_GRAPH_TEST_SOURCES})

# =================================================================
# 2. 契约级链条：第三方依赖、底层静态库与 GoogleTest 框架强绑定
# =================================================================
find_package(Threads REQUIRED)

target_link_libraries(task_graph_tests
    PRIVATE
    mmo_runtime_task_graph # 🎯 核心测试目标
    gtest
    gtest_main
    Threads::Threads
)

# =================================================================
# 3. 严苛编译选项对齐
# =================================================================
target_compile_options(task_graph_tests PRIVATE
    -Wall -Wextra -Wpedantic -Werror -Wshadow -Wconversion
)

# =================================================================
# 4. 工业级 Sanitizer 门禁分流控制（🛡️ 强力穿透传递给整个流水线生命周期依赖）
# =================================================================
if(MMO_ENABLE_ASAN AND MMO_ENABLE_TSAN)
    message(FATAL_ERROR "💥 架构级冲突: 内存卫士(ASan)与线程竞态卫士(TSan)无法同时启用！请分流构建。")
endif()

if(MMO_ENABLE_ASAN)
    message(STATUS "🛡️ [TaskGraphTest] 激活防护: AddressSanitizer & UndefinedBehaviorSanitizer 已挂载")

    set(ASAN_OPTIONS_LIST -fsanitize=address -fsanitize=undefined -fno-omit-frame-pointer)

    target_compile_options(task_graph_tests PRIVATE ${ASAN_OPTIONS_LIST})
    target_link_options(task_graph_tests PRIVATE -fsanitize=address -fsanitize=undefined)

    target_compile_options(mmo_runtime_task_graph PRIVATE ${ASAN_OPTIONS_LIST})
    target_link_options(mmo_runtime_task_graph PRIVATE -fsanitize=address -fsanitize=undefined)

elseif(MMO_ENABLE_TSAN)
    message(STATUS "🚀 [TaskGraphTest] 激活防护: ThreadSanitizer (TSan 线程竞态检测) 已挂载")

    target_compile_options(task_graph_tests PRIVATE -fsanitize=thread -fno-omit-frame-pointer)
    target_link_options(task_graph_tests PRIVATE -fsanitize=thread)

    target_compile_options(mmo_runtime_task_graph PRIVATE -fsanitize=thread -fno-omit-frame-pointer)
    target_link_options(mmo_runtime_task_graph PRIVATE -fsanitize=thread)
endif()

# =================================================================
# ⚡ 5. 硬件层算力榨干微架构优化
# =================================================================
if(CMAKE_BUILD_TYPE STREQUAL "Release")
    target_compile_options(task_graph_tests PRIVATE -O3 -march=native -fomit-frame-pointer)
    target_compile_options(mmo_runtime_task_graph PRIVATE -O3 -march=native -fomit-frame-pointer)
endif()

# =================================================================
# ⚖️ 6. 工业级 CTest 门禁分流精细化微操
# =================================================================
include(CTest)

# 【门禁 A】任务图静态常量定义、内存对齐与 ABI 稳定性校验 (CI Fast Pass)
add_test(NAME Runtime_TaskGraph_Abi_Tests
    COMMAND task_graph_tests --gtest_filter=TaskGraphAbiTests.*
)
set_tests_properties(Runtime_TaskGraph_Abi_Tests PROPERTIES LABELS "Fast")