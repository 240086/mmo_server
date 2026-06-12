# /home/xj/projects/mmo_server/server_core/tests/CMakeLists.txt
cmake_minimum_required(VERSION 3.20)

# =================================================================
# 彻底移除 FetchContent，因为根目录已经通过 third_party 引入了 gtest
# =================================================================

# 向下路由到具体的业务测试目录
add_subdirectory(runtime/timer)
add_subdirectory(infrastructure/lockfree_queue)
add_subdirectory(infrastructure/packet)
add_subdirectory(infrastructure/memory_pool)
add_subdirectory(infrastructure/serialization)
add_subdirectory(infrastructure/logger)