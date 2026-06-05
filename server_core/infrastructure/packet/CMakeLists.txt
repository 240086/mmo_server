# /home/xj/projects/mmo_server/server_core/infrastructure/packet/CMakeLists.txt

cmake_minimum_required(VERSION 3.22)

# 统一不在此处声明 project，直接定义 Target

# =====================================================
# 1. 定义路径前缀
# =====================================================
set(PACKET_INC_BASE "include/mmo/infrastructure/packet")
set(PACKET_SRC_BASE "src")

# =====================================================
# 2. 显式列出所有头文件
# =====================================================
set(PACKET_HEADERS
    "${PACKET_INC_BASE}/PacketBuffer.hpp"
    "${PACKET_INC_BASE}/PacketConstants.hpp"
    "${PACKET_INC_BASE}/PacketFlags.hpp"
    "${PACKET_INC_BASE}/PacketHeader.hpp"
    "${PACKET_INC_BASE}/PacketReader.hpp"
    "${PACKET_INC_BASE}/PacketTypes.hpp"
    "${PACKET_INC_BASE}/PacketView.hpp"
    "${PACKET_INC_BASE}/PacketWriter.hpp"
)

# =====================================================
# 3. 显式列出所有源文件
# =====================================================
set(PACKET_SOURCES
    "${PACKET_SRC_BASE}/PacketBuffer.cpp"
    "${PACKET_SRC_BASE}/PacketView.cpp"
    "${PACKET_SRC_BASE}/PacketReader.cpp"
    "${PACKET_SRC_BASE}/PacketWriter.cpp"
)

# =====================================================
# 4. 创建静态库目标
# =====================================================
add_library(mmo_packet STATIC
    ${PACKET_HEADERS}
    ${PACKET_SOURCES}
)

# =====================================================
# 5. 配置包含路径
# =====================================================
target_include_directories(mmo_packet
    PUBLIC

    # 1. 暴露给外部模块的根路径，使外部可以 <mmo/infrastructure/packet/xxx.hpp> 引用
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>

    PRIVATE

    # 2. 关键：将内部头文件子目录设为 PRIVATE，解决你当前 .cpp 找不到平铺头文件的问题
    ${CMAKE_CURRENT_SOURCE_DIR}/${PACKET_INC_BASE}

    # 3. src 供内部引用
    ${CMAKE_CURRENT_SOURCE_DIR}/src
)

# =====================================================
# 6. 编译选项与链接
# =====================================================
target_compile_options(mmo_packet PRIVATE
    -O3
    -Wall
    -Wextra
    -Wpedantic
    -Werror
    -Wshadow
    -Wconversion
    -Wno-unused-parameter
    -march=native
)

target_link_libraries(mmo_packet PUBLIC Threads::Threads)

# 给 target 设置别名
add_library(mmo::packet ALIAS mmo_packet)