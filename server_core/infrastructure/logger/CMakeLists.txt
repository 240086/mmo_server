# /home/xj/projects/mmo_server/server_core/infrastructure/logger/CMakeLists.txt
cmake_minimum_required(VERSION 3.22)

# =====================================================
# 1. 定义路径前缀（严防硬编码污染）
# =====================================================
set(LOGGER_INC_BASE "include/mmo/infrastructure/logger")
set(LOGGER_SRC_BASE "src")

# =====================================================
# 2. 显式列出所有头文件（全量暴露，维护极佳的现代编译契约与IDE感知）
# =====================================================
set(LOGGER_HEADERS
    "${LOGGER_INC_BASE}/ILogSink.hpp"
    "${LOGGER_INC_BASE}/IFormatter.hpp"
    "${LOGGER_INC_BASE}/LogLevel.hpp"
    "${LOGGER_INC_BASE}/LogRecord.hpp"
    "${LOGGER_INC_BASE}/LoggerTypes.hpp"
    "${LOGGER_INC_BASE}/LoggerConstants.hpp"
    "${LOGGER_INC_BASE}/ConsoleSink.hpp"
    "${LOGGER_INC_BASE}/FileSink.hpp"
    "${LOGGER_INC_BASE}/TextFormatter.hpp"
    "${LOGGER_INC_BASE}/Logger.hpp"
)

# =====================================================
# 3. 显式列出所有源文件（不使用 GLOB，确保增量编译追踪的确定性）
# =====================================================
set(LOGGER_SOURCES
    "${LOGGER_SRC_BASE}/ConsoleSink.cpp"
    "${LOGGER_SRC_BASE}/FileSink.cpp"
    "${LOGGER_SRC_BASE}/TextFormatter.cpp"
    "${LOGGER_SRC_BASE}/Logger.cpp"
)

# =====================================================
# 4. 创建静态库目标
# =====================================================
add_library(mmo_logger STATIC
    ${LOGGER_HEADERS}
    ${LOGGER_SOURCES}
)

# =====================================================
# 5. 配置包含路径（实现现代 CMake 的 Interface 严格隔离）
# =====================================================
target_include_directories(mmo_logger
    PUBLIC

    # 暴露给外部模块的根路径，使其他模块可以使用 <mmo/infrastructure/logger/xxx.hpp>
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>

    PRIVATE

    # 内部头文件子目录与 src 内部引用隔离
    ${CMAKE_CURRENT_SOURCE_DIR}/${LOGGER_INC_BASE}
    ${CMAKE_CURRENT_SOURCE_DIR}/src
)

# =====================================================
# 6. 核心基础设施：硬核微架构硬件榨干优化与严苛编译选项
# =====================================================
target_compile_options(mmo_logger PRIVATE
    -O3
    -Wall
    -Wextra
    -Wpedantic
    -Werror
    -Wshadow # 🛡️ 拦截局部变量遮蔽
    -Wconversion # 🛡️ 拦截潜在的隐式类型转换截断
    -Wformat=2 # 🛡️ 强化文本格式化安全检查，杜绝变参格式化安全隐漏洞
    -march=native # 解锁当前宿主机 CPU 的全部现代化原生硬件指令集，全力提速文本拼接
)

# =====================================================
# 7. 契约级链条：挂载网络、无锁底座依赖与原生线程库（PUBLIC 属性自动传递下游）
# =====================================================
find_package(Threads REQUIRED)

target_link_libraries(mmo_logger
    PUBLIC
    mmo_lockfree_queue # 💡 强绑定无锁队列，赋予 Logger 随时爆发成异步架构的超能力
    mmo_packet # 💡 强绑定网络包，使底层组件具备解耦一致性
    Threads::Threads # 💡 强绑定原生 pthread，保障后台 IO 单线程的运行时特权
)

# 给核心 Target 设置规范别名（完全符合现代 C++ 命名空间设计哲学）
add_library(mmo::logger ALIAS mmo_logger)