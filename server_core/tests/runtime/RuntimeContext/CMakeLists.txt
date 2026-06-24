# /home/xj/projects/mmo_server/server_core/tests/runtime/RuntimeContext/CMakeLists.txt
cmake_minimum_required(VERSION 3.22)

# =================================================================
# 1. 汇聚物理拆分后的多维度运行时测试源文件
# =================================================================
set(RUNTIME_CTX_TEST_SOURCES
    RuntimeContextConstructionTests.cpp
    RuntimeContextLayoutTests.cpp
    RuntimeModeTests.cpp
    RuntimeTypeTests.cpp
)

# 创建运行时基础设施统一验证可执行目标
add_executable(runtime_context_tests ${RUNTIME_CTX_TEST_SOURCES})

# =================================================================
# 2. 契约级链条：第三方依赖、底层静态库与 GoogleTest 框架强绑定
# =================================================================
find_package(Threads REQUIRED)

target_link_libraries(runtime_context_tests
    PRIVATE
    mmo_runtime_context # 🎯 核心运行时上下文静态库
    gtest
    gtest_main
    Threads::Threads
)

# =================================================================
# 3. 严苛编译选项对齐
# =================================================================
target_compile_options(runtime_context_tests PRIVATE
    -Wall -Wextra -Wpedantic -Werror -Wshadow -Wconversion
)

# =================================================================
# 4. 工业级 Sanitizer 门禁分流控制（🛡️ 强力穿透传递给底层依赖）
# =================================================================
if(MMO_ENABLE_ASAN AND MMO_ENABLE_TSAN)
    message(FATAL_ERROR "💥 架构级冲突: 内存卫士(ASan)与线程竞态卫士(TSan)无法同时启用！请分流构建。")
endif()

if(MMO_ENABLE_ASAN)
    message(STATUS "🛡️ [RuntimeTest] 激活防护: AddressSanitizer & UndefinedBehaviorSanitizer 已挂载")

    # 🎯 穿透应用给测试目标
    target_compile_options(runtime_context_tests PRIVATE -fsanitize=address -fsanitize=undefined -fno-omit-frame-pointer)
    target_link_options(runtime_context_tests PRIVATE -fsanitize=address -fsanitize=undefined)

    # 🎯 强行穿透给依赖的运行时上下文库
    target_compile_options(mmo_runtime_context PRIVATE -fsanitize=address -fsanitize=undefined -fno-omit-frame-pointer)
    target_link_options(mmo_runtime_context PRIVATE -fsanitize=address -fsanitize=undefined)

elseif(MMO_ENABLE_TSAN)
    message(STATUS "🚀 [RuntimeTest] 激活防护: ThreadSanitizer (TSan 线程竞态检测) 已挂载")

    # 🎯 穿透应用给测试目标
    target_compile_options(runtime_context_tests PRIVATE -fsanitize=thread -fno-omit-frame-pointer)
    target_link_options(runtime_context_tests PRIVATE -fsanitize=thread)

    # 🎯 强行穿透给依赖的运行时上下文库（保障多线程环境拉起/销毁时的竞态检测）
    target_compile_options(mmo_runtime_context PRIVATE -fsanitize=thread -fno-omit-frame-pointer)
    target_link_options(mmo_runtime_context PRIVATE -fsanitize=thread)
endif()

# =================================================================
# ⚡ 5. 硬件层算力榨干微架构优化
# =================================================================
if(CMAKE_BUILD_TYPE STREQUAL "Release")
    target_compile_options(runtime_context_tests PRIVATE -O3 -march=native -fomit-frame-pointer)
    target_compile_options(mmo_runtime_context PRIVATE -O3 -march=native -fomit-frame-pointer)
endif()

# =================================================================
# ⚖️ 6. 工业级 CTest 门禁分流精细化微操
# =================================================================
include(CTest)

# 【门禁 A】上下文内存布局与静态对象类型校验 (CI Fast Pass)
add_test(NAME Runtime_Context_Static_Tests
    COMMAND runtime_context_tests --gtest_filter=RuntimeContextLayoutTests.*:RuntimeTypeTests.*
)
set_tests_properties(Runtime_Context_Static_Tests PROPERTIES LABELS "Fast")

# 【门禁 B】上下文生命周期构建、模式切换与动态行为验证
add_test(NAME Runtime_Context_Lifecycle_Tests
    COMMAND runtime_context_tests --gtest_filter=RuntimeContextConstructionTests.*:RuntimeModeTests.*
)
set_tests_properties(Runtime_Context_Lifecycle_Tests PROPERTIES LABELS "Slow" TIMEOUT 30)