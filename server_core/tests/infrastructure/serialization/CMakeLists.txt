# /home/xj/projects/mmo_server/server_core/tests/infrastructure/serialization/CMakeLists.txt
cmake_minimum_required(VERSION 3.20)

# =================================================================
# 1. 声明基础单元测试（验证基础 Primitive/String 读写与核心逻辑，执行极快）
# =================================================================
set(UT_SOURCES

    # PrimitiveSerializationTests.cpp
    StringSerializationTests.cpp

    BinaryWriterTests.cpp
    BinaryReaderTests.cpp
)

# =================================================================
# 2. 声明双向对撞与确定性验证测试（数据流完整闭环验证）
# =================================================================
set(ROUNDTRIP_SOURCES

    SerializationRoundTripTests.cpp
)

# =================================================================
# 3. 声明防御性边界测试（恶意长度网络轰炸、残缺包、空流拦截等）
# =================================================================
set(BOUNDARY_SOURCES

    SerializationBoundaryTests.cpp
)

# 创建统一的序列化模块测试可执行目标
add_executable(serialization_tests
    ${UT_SOURCES}
    ${ROUNDTRIP_SOURCES}
    ${BOUNDARY_SOURCES}
    PrimitiveSerializationTests.cpp
    SerializationBoundaryTests.cpp
    SerializationRoundTripTests.cpp
    BinaryWriterTests.cpp
    BinaryReaderTests.cpp
)

# 🚀 链接序列化子模块静态库、底层网络包底座和 GTest 框架 🚀
target_link_libraries(serialization_tests
    PRIVATE
    mmo_serialization
    mmo_packet
    gtest
    gtest_main
)

# =================================================================
# ⚡ 4. 架构师硬核微架构硬件榨干优化（仅在 Release 模式生效）
# =================================================================
if(CMAKE_BUILD_TYPE STREQUAL "Release")
    target_compile_options(serialization_tests PRIVATE
        -O3
        -march=native
        -fomit-frame-pointer # 剥离栈帧指针，全力加速底层二进制平铺拷贝
    )
endif()

# =================================================================
# 5. ⚖️ 工业级微操：在 CTest 中将它们拆分为不同的标签运行（分流隔离执行）
# =================================================================

# 【第一批次】标准基础单元测试：卡死在每次 Commit 的 CI 核心门禁（多线程并发跑）
add_test(NAME Serialization_UnitTest
    COMMAND serialization_tests
    --gtest_filter=PrimitiveSerializationTests.*:StringSerializationTests.*:BinaryWriterTests.*:BinaryReaderTests.*)
set_tests_properties(Serialization_UnitTest PROPERTIES LABELS "CI;UnitTest")

# 【第二批次】两极双向对撞完整性测试：多维度基础/文本数据全量 RoundTrip 确定性断言
add_test(NAME Serialization_RoundTripTest
    COMMAND serialization_tests
    --gtest_filter=SerializationRoundTripTests.*)
set_tests_properties(Serialization_RoundTripTest PROPERTIES LABELS "CI;RoundTrip")

# 【第三批次】高抗压网络防御边界测试（截断网络包、溢出长度拒绝攻击）
add_test(NAME Serialization_BoundaryTest
    COMMAND serialization_tests
    --gtest_filter=SerializationBoundaryTests.*)
set_tests_properties(Serialization_BoundaryTest PROPERTIES LABELS "Robustness;Boundary")