# /home/xj/projects/mmo_server/server_core/infrastructure/ConfigSystem/CMakeLists.txt
cmake_minimum_required(VERSION 3.22)

# =====================================================
# 1. 定义路径前缀（严防硬编码污染，对齐现代化多层目录结构）
# =====================================================
set(CONFIG_INC_BASE "include/mmo/infrastructure/ConfigSystem")
set(CONFIG_SRC_BASE "src")

# =====================================================
# 2. 显式列出所有头文件（全量暴露，维护极佳的现代编译契约与IDE感知）
# =====================================================
set(CONFIG_HEADERS
    "${CONFIG_INC_BASE}/IConfigSystem.hpp"
    "${CONFIG_INC_BASE}/ConfigSystem.hpp"
    "${CONFIG_INC_BASE}/ConfigTypes.hpp"
    "${CONFIG_INC_BASE}/ConfigValue.hpp"

    # Parser 抽象与具体实现
    "${CONFIG_INC_BASE}/parser/IConfigParser.hpp"
    "${CONFIG_INC_BASE}/parser/KeyValueConfigParser.hpp"
    "${CONFIG_INC_BASE}/parser/ParseResult.hpp"

    # Source 抽象与具体实现
    "${CONFIG_INC_BASE}/source/IConfigSource.hpp"
    "${CONFIG_INC_BASE}/source/FileConfigSource.hpp"
)

# =====================================================
# 3. 显式列出所有源文件（不使用 GLOB，确保增量编译追踪的确定性）
# =====================================================
set(CONFIG_SOURCES
    "${CONFIG_SRC_BASE}/ConfigSystem.cpp"
    "${CONFIG_SRC_BASE}/parser/KeyValueConfigParser.cpp"
    "${CONFIG_SRC_BASE}/source/FileConfigSource.cpp"
)

# =====================================================
# 4. 创建静态库目标
# =====================================================
add_library(mmo_config_system STATIC
    ${CONFIG_HEADERS}
    ${CONFIG_SOURCES}
)

# =====================================================
# 5. 配置包含路径（实现现代 CMake 的 Interface 严格隔离）
# =====================================================
target_include_directories(mmo_config_system
    PUBLIC

    # 暴露给外部模块的根路径，使其他模块可以使用 <mmo/infrastructure/ConfigSystem/xxx.hpp>
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>

    PRIVATE

    # 内部头文件子目录与 src 内部引用隔离
    ${CMAKE_CURRENT_SOURCE_DIR}/${CONFIG_INC_BASE}
    ${CMAKE_CURRENT_SOURCE_DIR}/src
)

# =====================================================
# 6. 核心基础设施：硬核微架构硬件榨干优化与严苛编译选项
# =====================================================
target_compile_options(mmo_config_system PRIVATE
    -O3
    -Wall
    -Wextra
    -Wpedantic
    -Werror
    -Wshadow # 🛡️ 拦截局部变量遮蔽
    -Wconversion # 🛡️ 拦截潜在的隐式类型转换截断
    -march=native # 解锁当前宿主机 CPU 的全部现代化原生硬件指令集，加速配置项哈希与字符串解析
)

# =====================================================
# 7. 契约级链条：第三方依赖与并发线程库绑定
# =====================================================
find_package(Threads REQUIRED)

target_link_libraries(mmo_config_system
    PUBLIC
    Threads::Threads # 💡 强绑定原生 pthread，保障配置热加载、多线程并发只读/写锁的安全存取

    PRIVATE

    # 如果你的 Serialization（序列化）模块已经完成，且 Config 依赖它来解析 JSON/XML，请在此处解锁链接：
    # mmo_serialization
)

# 给核心 Target 设置规范别名（完全符合现代 C++ 命名空间设计哲学）
add_library(mmo::config_system ALIAS mmo_config_system)