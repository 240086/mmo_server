# /home/xj/projects/mmo_server/server_core/tests/infrastructure/ConfigSystem/CMakeLists.txt
cmake_minimum_required(VERSION 3.22)

# =================================================================
# 1. 汇聚物理拆分后的所有多维度测试源文件
# =================================================================
set(CONFIG_TEST_SOURCES
    ConfigLoadTests.cpp
    ConfigParserTests.cpp
    ConfigAccessTests.cpp
    ConfigFailureTests.cpp
    ConfigConcurrencyTests.cpp
)

# 创建配置系统统一验证可执行目标
add_executable(config_system_tests ${CONFIG_TEST_SOURCES})

# 🚀 绑定底层静态库与 GoogleTest 框架
target_link_libraries(config_system_tests
    PRIVATE
    mmo_config_system
    gtest
    gtest_main
)

# =================================================================
# ⚡ 2. 硬件层算力榨干微架构优化（Release 极速通道）
# =================================================================
if(CMAKE_BUILD_TYPE STREQUAL "Release")
    target_compile_options(config_system_tests PRIVATE
        -O3
        -march=native
        -fomit-frame-pointer # 剥离栈帧，释放全部寄存器用于加速热路径无锁/读写锁配置查找
    )
endif()

# =================================================================
# ⚖️ 3. 工业级 CTest 门禁分流精细化微操
# =================================================================
include(CTest)

# 【门禁 A】基础稳定性测试集 (CI Fast Pass)
# 包含：基础解析、加载机制、常规获取、异常容错与边界防御测试。每次 Commit 必须在瞬间跑完。
add_test(NAME ConfigSystem_Core_Functional_Tests
    COMMAND config_system_tests
    --gtest_filter=ConfigLoadTests.*:ConfigParserTests.*:ConfigAccessTests.*:ConfigFailureTests.*
)
set_tests_properties(ConfigSystem_Core_Functional_Tests PROPERTIES LABELS "CI;UnitTest;Fast")

# 【门禁 B】多线程高并发与配置热加载绞肉机测试集 (Stress & Concurrency Check)
# 包含：海量游戏实体线程同时并发读取、后台动态热加载（Reload）读写激烈对撞。作为 Nightly 或推流门禁。
add_test(NAME ConfigSystem_High_Load_Concurrency_Tests
    COMMAND config_system_tests
    --gtest_filter=ConfigConcurrencyTests.*
)
set_tests_properties(ConfigSystem_High_Load_Concurrency_Tests PROPERTIES LABELS "Stress;Stability;Slow")