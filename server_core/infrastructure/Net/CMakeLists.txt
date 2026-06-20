# /home/xj/projects/mmo_server/server_core/infrastructure/Net/CMakeLists.txt
cmake_minimum_required(VERSION 3.22)

# =====================================================
# 1. 定义路径前缀（严防硬编码污染，对齐现代化多层目录结构）
# =====================================================
set(NET_INC_BASE "include/mmo/infrastructure/Net")
set(NET_SRC_BASE "src")

# =====================================================
# 2. 显式列出所有头文件（全量暴露，维护极佳的现代编译契约与IDE感知）
# =====================================================
set(NET_HEADERS
    "${NET_INC_BASE}/ConnectionId.hpp"
    "${NET_INC_BASE}/IConnection.hpp"
    "${NET_INC_BASE}/INetServer.hpp"
    "${NET_INC_BASE}/NetServer.hpp"
    "${NET_INC_BASE}/NetTypes.hpp"
    "${NET_INC_BASE}/PacketCodec.hpp"
    "${NET_INC_BASE}/PacketFrame.hpp"
    "${NET_INC_BASE}/TcpConnection.hpp"
)

# =====================================================
# 3. 显式列出所有源文件（不使用 GLOB，确保增量编译追踪的确定性）
# =====================================================
set(NET_SOURCES
    "${NET_SRC_BASE}/NetServer.cpp"
    "${NET_SRC_BASE}/PacketCodec.cpp"
    "${NET_SRC_BASE}/TcpConnection.cpp"
)

# =====================================================
# 4. 创建静态库目标
# =====================================================
add_library(mmo_net STATIC
    ${NET_HEADERS}
    ${NET_SOURCES}
)

# =====================================================
# 5. 配置包含路径（实现现代 CMake 的 Interface 严格隔离）
# =====================================================
target_include_directories(mmo_net
    PUBLIC

    # 暴露给外部模块的根路径，使其他模块可以使用 <mmo/infrastructure/Net/xxx.hpp>
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>

    PRIVATE

    # 内部头文件子目录与 src 内部引用隔离
    ${CMAKE_CURRENT_SOURCE_DIR}/${NET_INC_BASE}
    ${CMAKE_CURRENT_SOURCE_DIR}/src
)

# =====================================================
# 6. 核心基础设施：硬核微架构硬件榨干优化与严苛编译选项
# =====================================================
target_compile_options(mmo_net PRIVATE
    -O3
    -Wall
    -Wextra
    -Wpedantic
    -Werror
    -Wshadow        # 🛡️ 拦截局部变量遮蔽
    -Wconversion    # 🛡️ 拦截潜在的隐式类型转换截断
    -march=native   # 解锁当前宿主机 CPU 的全部现代化原生硬件指令集（如AVX2/AVX-512等），加速网络包过滤与校验
)

# =====================================================
# 7. 预处理器宏定义（深度激活 C++20 协程与 Boost.Asio 高性能现代特性）
# =====================================================
target_compile_definitions(mmo_net 
    PRIVATE
    BOOST_ASIO_HAS_CO_AWAIT
    BOOST_ASIO_NO_DEPRECATED
    BOOST_ASIO_DISABLE_FENCED_BLOCK
    BOOST_ASIO_USE_TSAN
)

# =====================================================
# 8. 契约级链条：第三方依赖与并发线程库绑定
# =====================================================
# 1. 强制关闭 Config 模式！不允许查找 xxxConfig.cmake，只允许通过 Linux 系统的 FindBoost.cmake 查找
set(Boost_NO_BOOST_CMAKE ON)

# 2. 限制搜索范围，只允许在 CMAKE_FIND_ROOT_PATH 内部查找，禁止向外界扩展
set(CMAKE_FIND_ROOT_PATH_MODE_PACKAGE ONLY)
set(CMAKE_FIND_ROOT_PATH "/usr")

# 3. 锁死 Linux 系统原生路径
set(Boost_ROOT "/usr")
set(Boost_NO_SYSTEM_PATHS OFF)


# 查找系统或特定路径下的 Boost 库
find_package(Boost 1.75 REQUIRED COMPONENTS system)
find_package(Threads REQUIRED)

target_link_libraries(mmo_net
    PUBLIC
    Threads::Threads          # 💡 强绑定原生 pthread，保障 Asio io_context 多线程事件循环(Event Loop)的安全驱使
    Boost::boost              # 💡 暴露 Boost 头文件给依赖 Net 的上层模块（如 game_server 业务层）

    PRIVATE
    Boost::system             # ⚙️ 仅在内部网络实现中链接 Boost 错误码系统

    # 🔗 纵向依赖链：Net 模块需要组包和解析，在此处解锁内部基础设施依赖
    mmo_packet                # 依赖包解析模块
    mmo_memory_pool           # 依赖内存池模块（用于 Asio 异步读写的 Buffer 分配）
    mmo::lockfree_queue
)

# 给核心 Target 设置规范别名（完全符合现代 C++ 命名空间设计哲学）
add_library(mmo::net ALIAS mmo_net)