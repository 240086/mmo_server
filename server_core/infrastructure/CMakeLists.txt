# /home/xj/projects/mmo_server/server_core/infrastructure/CMakeLists.txt
cmake_minimum_required(VERSION 3.20)

# 🚀 职责：收拢并驱动所有的 L1 基础设施子模块
add_subdirectory(timer)
add_subdirectory(lockfree_queue)
add_subdirectory(packet)
add_subdirectory(memory_pool)
add_subdirectory(serialization)
add_subdirectory(logger)
add_subdirectory(EventBus)
add_subdirectory(ConfigSystem)
add_subdirectory(Net)


# 💡 未来扩展边界：当推进日志、网络模块时，直接加在这里：
# add_subdirectory(logger)
# add_subdirectory(net)

# 逻辑聚合
add_library(mmo_infrastructure INTERFACE)
target_link_libraries(mmo_infrastructure INTERFACE 
    mmo::timer
    mmo::lockfree_queue
    mmo::packet
    mmo::memory_pool    
    mmo::serialization
    mmo::logger
    mmo::event_bus
    mmo::config_system
    mmo::net
)