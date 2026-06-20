# /home/xj/projects/mmo_server/server_core/tests/runtime/timer/CMakeLists.txt
cmake_minimum_required(VERSION 3.20)

# 1. 创建测试可执行目标
add_executable(mmo_timer_tests
    TimerCancellationTests.cpp
    TimerQueueOrderTests.cpp
    MultiProducerScheduleTests.cpp
    TimerStressTests.cpp
    TimerPrecisionTests.cpp
)

# 2. 🚀 链接核心模块和 GTest 🚀
target_link_libraries(mmo_timer_tests
    PRIVATE
    mmo::timer # 💡 优化：使用现代感更强的命名空间别名
    gtest # 链接 GTest 核心
    gtest_main # 自动生成 main() 函数
)

# 3. 注册到 CTest 框架
add_test(NAME TimerModuleTests COMMAND mmo_timer_tests)