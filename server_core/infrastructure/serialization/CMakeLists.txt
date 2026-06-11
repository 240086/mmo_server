# /home/xj/projects/mmo_server/server_core/infrastructure/serialization/CMakeLists.txt
cmake_minimum_required(VERSION 3.22)

# =====================================================
# 1. 定义路径前缀（严防硬编码污染）
# =====================================================
set(SERIAL_INC_BASE "include/mmo/infrastructure/serialization")
set(SERIAL_SRC_BASE "src")

# =====================================================
# 2. 显式列出所有头文件（全量暴露，维护极佳的现代编译契约与IDE感知）
# =====================================================
set(SERIAL_HEADERS
    "${SERIAL_INC_BASE}/SerializationTypes.hpp"
    "${SERIAL_INC_BASE}/SerializationTraits.hpp"
    "${SERIAL_INC_BASE}/PrimitiveSerializer.hpp"
    "${SERIAL_INC_BASE}/StringSerializer.hpp"
    "${SERIAL_INC_BASE}/BinaryWriter.hpp"
    "${SERIAL_INC_BASE}/BinaryReader.hpp"
)

# =====================================================
# 3. 显式列出所有源文件（不使用 GLOB，确保增量编译追踪的确定性）
# =====================================================
set(SERIAL_SOURCES
    "${SERIAL_SRC_BASE}/StringSerializer.cpp"
    "${SERIAL_SRC_BASE}/BinaryWriter.cpp"
    "${SERIAL_SRC_BASE}/BinaryReader.cpp"
)

# =====================================================
# 4. 创建静态库目标
# =====================================================
add_library(mmo_serialization STATIC
    ${SERIAL_HEADERS}
    ${SERIAL_SOURCES}
    include/mmo/infrastructure/serialization/SerializationConstants.hpp
    include/mmo/infrastructure/serialization/SerializationError.hpp
)

# =====================================================
# 5. 配置包含路径（实现现代 CMake 的 Interface 严格隔离）
# =====================================================
target_include_directories(mmo_serialization
    PUBLIC

    # 暴露给外部模块的根路径，使其他模块可以使用 <mmo/infrastructure/serialization/xxx.hpp>
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>

    PRIVATE

    # 内部头文件子目录与 src 内部引用隔离
    ${CMAKE_CURRENT_SOURCE_DIR}/${SERIAL_INC_BASE}
    ${CMAKE_CURRENT_SOURCE_DIR}/src
)

# =====================================================
# 6. 核心基础设施：硬核微架构硬件榨干优化与严苛编译选项
# =====================================================
target_compile_options(mmo_serialization PRIVATE
    -O3
    -Wall
    -Wextra
    -Wpedantic
    -Werror
    -Wshadow # 🛡️ 拦截局部变量遮蔽
    -Wconversion # 🛡️ 拦截潜在的 size_t 到 uint32_t 的隐式不安全截断（序列化长度校验的核心防线）
    -Wno-unused-parameter
    -march=native # 解锁当前宿主机 CPU 的全部现代化原生硬件指令集，榨干内存拷贝性能
)

# =====================================================
# 7. 契约级链条：挂载网络层底座依赖（PUBLIC 属性确保向下游自动传递）
# =====================================================
target_link_libraries(mmo_serialization
    PUBLIC
    mmo_packet # 💡 强绑定网络包底座，使依赖序列化的上层模块天然具备解析网络包能力
)

# 给核心 Target 设置规范别名（完全符合现代 C++ 命名空间设计哲学）
add_library(mmo::serialization ALIAS mmo_serialization)