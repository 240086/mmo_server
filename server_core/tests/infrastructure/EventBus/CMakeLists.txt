# /home/xj/projects/mmo_server/server_core/tests/infrastructure/EventBus/CMakeLists.txt
cmake_minimum_required(VERSION 3.22)

# =================================================================
# 1. 汇聚物理拆分后的所有多维度测试源文件
# =================================================================
set(EVENTBUS_TEST_SOURCES
    EventBusBasicTests.cpp
    EventBusReentrancyTests.cpp
    EventBusSnapshotTests.cpp
    EventBusConcurrencyTests.cpp
    EventBusLongDurationTests.cpp
)

# 创建事件总线统一验证可执行目标
add_executable(event_bus_tests ${EVENTBUS_TEST_SOURCES})

# 🚀 绑定底层静态库与 GoogleTest 框架
target_link_libraries(event_bus_tests
    PRIVATE
    mmo_event_bus
    gtest
    gtest_main
)

# =================================================================
# ⚡ 2. 硬件层算力榨干微架构优化（Release 极速通道）
# =================================================================
if(CMAKE_BUILD_TYPE STREQUAL "Release")
    target_compile_options(event_bus_tests PRIVATE
        -O3
        -march=native
        -fomit-frame-pointer # 剥离栈帧，释放全部寄存器用于加速热路径 Publish
    )
endif()

# =================================================================
# ⚖️ 3. 工业级 CTest 门禁分流精细化微操
# =================================================================
include(CTest)

# 【门禁 A】基础稳定性测试集 (CI Fast Pass)
# 包含：单线程功能、重入死锁安全、快照静态隔离语义。每次 Commit 代码必须在 1 秒内跑完。
add_test(NAME EventBus_Core_Functional_Tests
    COMMAND event_bus_tests
    --gtest_filter=EventBusBasicTests.*:EventBusReentrancyTests.*:EventBusSnapshotTests.*
)
set_tests_properties(EventBus_Core_Functional_Tests PROPERTIES LABELS "CI;UnitTest;Fast")

# 【门禁 B】高并发抗压与长时间稳定性测试集 (Stress & Stability Check)
# 包含：多线程读写绞肉机冲突对撞、百万级事件流冲击。单独作为每日构建(Nightly Build)或推流门禁。
add_test(NAME EventBus_High_Load_Stress_Tests
    COMMAND event_bus_tests
    --gtest_filter=EventBusConcurrencyTests.*:EventBusLongDurationTests.*
)
set_tests_properties(EventBus_High_Load_Stress_Tests PROPERTIES LABELS "Stress;Stability;Slow")