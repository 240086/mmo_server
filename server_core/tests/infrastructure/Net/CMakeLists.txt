# /home/xj/projects/mmo_server/server_core/tests/infrastructure/Net/CMakeLists.txt
# /home/xj/projects/mmo_server/server_core/tests/infrastructure/Net/CMakeLists.txt
cmake_minimum_required(VERSION 3.22)

# =================================================================
# 1. 跨平台宿主环境防御
# =================================================================
set(Boost_NO_BOOST_CMAKE ON)
set(CMAKE_FIND_ROOT_PATH_MODE_PACKAGE ONLY)
set(CMAKE_FIND_ROOT_PATH "/usr")
set(Boost_ROOT "/usr")
set(Boost_NO_SYSTEM_PATHS OFF)

# =================================================================
# 2. 汇聚物理拆分后的多维度网络层测试源文件
# =================================================================
set(NET_TEST_SOURCES
    PacketCodecTests.cpp
    TcpConnectionTests.cpp
    NetServerLifecycleTests.cpp
    NetServerAcceptTests.cpp
    NetServerReceiveTests.cpp
    NetServerLockFreeQueueTests.cpp
    NetServerConnectionTests.cpp
    MPSCQueueReceivedPacketStressTests.cpp
    NetServerReceiveStressTests.cpp
)

# 创建网络基础设施统一验证可执行目标
add_executable(net_infrastructure_tests ${NET_TEST_SOURCES} NetTestHelper.hpp)

# =================================================================
# 3. 契约级链条：第三方依赖、底层静态库与 GoogleTest 框架强绑定
# =================================================================
find_package(Boost 1.75 REQUIRED COMPONENTS system)
find_package(Threads REQUIRED)

target_link_libraries(net_infrastructure_tests
    PRIVATE
    mmo_net # 🎯 核心网络静态库
    mmo::lockfree_queue # 🔗 无锁队列纵向依赖
    mmo::packet # 🔗 包解析依赖
    gtest
    gtest_main
    Threads::Threads
    Boost::boost
    Boost::system
)

# =================================================================
# 4. 严苛编译选项对齐
# =================================================================
target_compile_options(net_infrastructure_tests PRIVATE
    -Wall -Wextra -Wpedantic -Werror -Wshadow -Wconversion
)

# =================================================================
# 5. 预处理器宏定义同步
# =================================================================
target_compile_definitions(net_infrastructure_tests PRIVATE
    BOOST_ASIO_HAS_CO_AWAIT
    BOOST_ASIO_NO_DEPRECATED
    BOOST_ASIO_DISABLE_FENCED_BLOCK
    BOOST_ASIO_USE_TSAN
)

# =================================================================
# 6. 工业级 Sanitizer 门禁分流控制（🔥 强力修复：穿透传递给底层依赖）
# =================================================================
# 注意：这里改为采用全局或上层定义的变量，不进行就地默认值覆盖
if(MMO_ENABLE_ASAN AND MMO_ENABLE_TSAN)
    message(FATAL_ERROR "💥 架构级冲突: 内存卫士(ASan)与线程竞态卫士(TSan)无法同时启用！请分流构建。")
endif()

if(MMO_ENABLE_ASAN)
    message(STATUS "🛡️ [NetTest] 激活防护: AddressSanitizer & UndefinedBehaviorSanitizer 已挂载")

    # 🎯 穿透应用给测试目标
    target_compile_options(net_infrastructure_tests PRIVATE -fsanitize=address -fsanitize=undefined -fno-omit-frame-pointer)
    target_link_options(net_infrastructure_tests PRIVATE -fsanitize=address -fsanitize=undefined)

    # 🎯 强行穿透给依赖的静态库（确保整个链条的内存边界同步被查）
    target_compile_options(mmo_net PRIVATE -fsanitize=address -fsanitize=undefined -fno-omit-frame-pointer)
    target_link_options(mmo_net PRIVATE -fsanitize=address -fsanitize=undefined)

elseif(MMO_ENABLE_TSAN)
    message(STATUS "🚀 [NetTest] 激活防护: ThreadSanitizer (TSan 线程竞态检测) 已挂载")

    # 🎯 穿透应用给测试目标
    target_compile_options(net_infrastructure_tests PRIVATE -fsanitize=thread -fno-omit-frame-pointer)
    target_link_options(net_infrastructure_tests PRIVATE -fsanitize=thread)

    # 🎯 🔥 终极防线：必须强行让底层网络库也带上 TSan 插桩，否则 Asio 后台线程的内存重排无法被追踪！
    target_compile_options(mmo_net PRIVATE -fsanitize=thread -fno-omit-frame-pointer)
    target_link_options(mmo_net PRIVATE -fsanitize=thread)
endif()

# =================================================================
# ⚡ 7. 硬件层算力榨干微架构优化
# =================================================================
if(CMAKE_BUILD_TYPE STREQUAL "Release")
    target_compile_options(net_infrastructure_tests PRIVATE -O3 -march=native -fomit-frame-pointer)

    # 同步压榨底层库性能
    target_compile_options(mmo_net PRIVATE -O3 -march=native -fomit-frame-pointer)
endif()

# =================================================================
# ⚖️ 8. 工业级 CTest 门禁分流精细化微操（更新为真实注册的过滤套件名）
# =================================================================
include(CTest)

# 【门禁 A】基础协议与状态机验证测试集 (CI Fast Pass)
add_test(NAME Net_Core_Functional_Tests
    COMMAND net_infrastructure_tests --gtest_filter=PacketCodecTests.*:TcpConnectionTests.StateTransition*
)
set_tests_properties(Net_Core_Functional_Tests PROPERTIES LABELS "Fast")

# 【门禁 B】网络生命周期与高并发连接测试集
add_test(NAME Net_High_Load_Concurrency_Tests
    COMMAND net_infrastructure_tests --gtest_filter=TcpConnectionTests.Backpressure*:NetServerLifecycleTests.*:NetServerAcceptTests.*:NetServerReceiveTests.*:NetServerLockFreeQueueTests.*:NetServerConnectionTests.*
)
set_tests_properties(Net_High_Load_Concurrency_Tests PROPERTIES LABELS "Slow" TIMEOUT 60)

# 【门禁 C】⚠️ 独立压测门禁 1：无锁队列密集数据包搬运压测（注意：对齐实际 TestSuite 命名）
add_test(NAME Net_MPSCQueue_Freeze_Stress_Test
    COMMAND net_infrastructure_tests --gtest_filter=MPSCQueueReceivedPacketStressTests.*
)
set_tests_properties(Net_MPSCQueue_Freeze_Stress_Test PROPERTIES LABELS "Freeze_Stress" TIMEOUT 120)

# 【门禁 D】⚠️ 独立压测门禁 2：全链路 Socket 到内存队列极限压力测试（注意：对齐实际 TestSuite 命名）
add_test(NAME Net_ServerReceive_Freeze_Stress_Test
    COMMAND net_infrastructure_tests --gtest_filter=NetServerReceiveStressTests.*
)
set_tests_properties(Net_ServerReceive_Freeze_Stress_Test PROPERTIES LABELS "Freeze_Stress" TIMEOUT 120)