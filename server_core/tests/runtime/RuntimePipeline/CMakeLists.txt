# /home/xj/projects/mmo_server/server_core/tests/runtime/RuntimePipeline/CMakeLists.txt
cmake_minimum_required(VERSION 3.22)

# =================================================================
# 1. 汇聚物理拆分后的多维度运行时流水线验证源文件
# =================================================================
set(RUNTIME_PIPELINE_TEST_SOURCES
    RuntimePipelineConstantsTests.cpp
    RuntimePipelineConstructionTests.cpp
    RuntimePipelineInitializationTests.cpp
    RuntimePipelineLayoutTests.cpp
)

# 创建运行时流水线微架构验证可执行目标
add_executable(runtime_pipeline_tests ${RUNTIME_PIPELINE_TEST_SOURCES})

# =================================================================
# 2. 契约级链条：第三方依赖、底层静态库与 GoogleTest 框架强绑定
# =================================================================
find_package(Threads REQUIRED)

target_link_libraries(runtime_pipeline_tests
    PRIVATE
    mmo_runtime_context
    mmo_runtime_phase
    mmo_runtime_pipeline # 🎯 核心测试目标
    gtest
    gtest_main
    Threads::Threads
)

# =================================================================
# 3. 严苛编译选项对齐
# =================================================================
target_compile_options(runtime_pipeline_tests PRIVATE
    -Wall -Wextra -Wpedantic -Werror -Wshadow -Wconversion
)

# =================================================================
# 4. 工业级 Sanitizer 门禁分流控制（🛡️ 强力穿透传递给整个流水线生命周期依赖）
# =================================================================
if(MMO_ENABLE_ASAN AND MMO_ENABLE_TSAN)
    message(FATAL_ERROR "💥 架构级冲突: 内存卫士(ASan)与线程竞态卫士(TSan)无法同时启用！请分流构建。")
endif()

if(MMO_ENABLE_ASAN)
    message(STATUS "🛡️ [PipelineTest] 激活防护: AddressSanitizer & UndefinedBehaviorSanitizer 已挂载")

    # 🎯 穿透应用给测试目标与依赖的核心静态库
    set(ASAN_OPTIONS_LIST -fsanitize=address -fsanitize=undefined -fno-omit-frame-pointer)

    target_compile_options(runtime_pipeline_tests PRIVATE ${ASAN_OPTIONS_LIST})
    target_link_options(runtime_pipeline_tests PRIVATE -fsanitize=address -fsanitize=undefined)

    target_compile_options(mmo_runtime_pipeline PRIVATE ${ASAN_OPTIONS_LIST})
    target_link_options(mmo_runtime_pipeline PRIVATE -fsanitize=address -fsanitize=undefined)

elseif(MMO_ENABLE_TSAN)
    message(STATUS "🚀 [PipelineTest] 激活防护: ThreadSanitizer (TSan 线程竞态检测) 已挂载")

    target_compile_options(runtime_pipeline_tests PRIVATE -fsanitize=thread -fno-omit-frame-pointer)
    target_link_options(runtime_pipeline_tests PRIVATE -fsanitize=thread)

    target_compile_options(mmo_runtime_pipeline PRIVATE -fsanitize=thread -fno-omit-frame-pointer)
    target_link_options(mmo_runtime_pipeline PRIVATE -fsanitize=thread)
endif()

# =================================================================
# ⚡ 5. 硬件层算力榨干微架构优化
# =================================================================
if(CMAKE_BUILD_TYPE STREQUAL "Release")
    target_compile_options(runtime_pipeline_tests PRIVATE -O3 -march=native -fomit-frame-pointer)
    target_compile_options(mmo_runtime_pipeline PRIVATE -O3 -march=native -fomit-frame-pointer)
endif()

# =================================================================
# ⚖️ 6. 工业级 CTest 门禁分流精细化微操
# =================================================================
include(CTest)

# 【门禁 A】流水线静态常量定义与内存对齐布局校验 (CI Fast Pass)
add_test(NAME Runtime_Pipeline_Static_Tests
    COMMAND runtime_pipeline_tests --gtest_filter=RuntimePipelineConstantsTests.*:RuntimePipelineLayoutTests.*
)
set_tests_properties(Runtime_Pipeline_Static_Tests PROPERTIES LABELS "Fast")

# 【门禁 B】流水线动态初始化、依赖拓扑构建与生命周期模拟验证 (涉及主循环演进)
add_test(NAME Runtime_Pipeline_Lifecycle_Tests
    COMMAND runtime_pipeline_tests --gtest_filter=RuntimePipelineConstructionTests.*:RuntimePipelineInitializationTests.*
)
set_tests_properties(Runtime_Pipeline_Lifecycle_Tests PROPERTIES LABELS "Slow" TIMEOUT 15)