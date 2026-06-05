# /home/xj/projects/mmo_server/server_core/tests/infrastructure/packet/CMakeLists.txt
cmake_minimum_required(VERSION 3.20)

# =================================================================
# 1. 声明所有基础功能单元测试（执行极快，用于每次 Commit 的 CI 快速门禁）
# =================================================================
set(UT_SOURCES
    PacketBufferTests.cpp
    PacketViewTests.cpp
    PacketReaderTests.cpp
    PacketWriterTests.cpp
    PacketHeaderLayoutTests.cpp # 🌟 内存布局测试属于静态断言级别，执行极快，归入快速 CI
)

# =================================================================
# 2. 声明协议集成与边界健壮性测试（第二批次：验证读写协议闭环与极端漏洞防御）
# =================================================================
set(INTEGRATION_SOURCES
    PacketRoundTripTests.cpp # 🌟 读写全流程回环测试（集成测试）
    PacketBoundaryTests.cpp # 🌟 恶意错包/越界拦截测试（鲁棒性防御测试）
)

# =================================================================
# 3. 声明高负载压力与吞吐量性能测试（第三批次：高频吃 CPU/内存，严禁并跑）
# =================================================================
set(STRESS_SOURCES
    PacketLargePayloadTests.cpp # 🌟 巨型包分配与拷贝负载测试

    # PacketSerializePerformanceTests.cpp
)

# 创建统一的数据包模块测试可执行目标
# 💡 架构师修正：移除了你原本 add_executable 中重复声明的 PacketReaderTests.cpp，防止符号重定义报错
add_executable(packet_tests
    ${UT_SOURCES}
    ${INTEGRATION_SOURCES}
    ${STRESS_SOURCES}
)

# 🚀 链接基础设施底座的 packet 静态库和 GTest 框架 🚀
target_link_libraries(packet_tests
    PRIVATE
    mmo_packet
    gtest
    gtest_main
)

# =================================================================
# ⚡ 4. 架构师硬核微架构硬件榨干优化（仅在 Release 模式生效）
# =================================================================
if(CMAKE_BUILD_TYPE STREQUAL "Release")
    target_compile_options(packet_tests PRIVATE
        -O3
        -march=native
        -fomit-frame-pointer
    )
endif()

# =================================================================
# 5. ⚖️ 工业级微操：在 CTest 中将它们彻底拆分为不同的批次（分流隔离执行）
# =================================================================

# 【第一批次】核心功能与协议头布局测试：多线程并发跑，卡死在 CI 门禁
add_test(NAME Packet_UnitTest
    COMMAND packet_tests --gtest_filter=PacketBufferTests.*:PacketViewTests.*:PacketReaderTests.*:PacketWriterTests.*:PacketHeaderLayoutTests.*)
set_tests_properties(Packet_UnitTest PROPERTIES LABELS "CI;UnitTest")

# 【第二批次】全链路集成与网络边界防御测试：独立批次
add_test(NAME Packet_IntegrationTest
    COMMAND packet_tests --gtest_filter=PacketRoundTripTests.*:PacketBoundaryTests.*)
set_tests_properties(Packet_IntegrationTest PROPERTIES LABELS "Integration;Robustness")

# 【第三批次】大包高负载压测：强行限制【单线程串行运行】，防止并发吃满宿主机物理内存
add_test(NAME Packet_StressTest
    COMMAND packet_tests --gtest_filter=PacketLargePayloadTests.*:*Stress*:*Performance*)
set_tests_properties(Packet_StressTest PROPERTIES LABELS "Stress" RUN_SERIAL TRUE)