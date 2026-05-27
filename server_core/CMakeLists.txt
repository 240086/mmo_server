# /home/xj/projects/mmo_server/server_core/CMakeLists.txt
cmake_minimum_required(VERSION 3.20)

# 🚀 职责：收拢并驱动 Core 内部所有的垂直分层
# 警告：不要在这里写 project()，这会导致 CMake 变量作用域重置

# 1. 进入 L1 基础设施层 (Timer, Logger, Network)
add_subdirectory(infrastructure)

# 2. 💡 未来扩展边界：进入 L2 运行时层 (EventLoop, Actor Model, TaskScheduler)
# add_subdirectory(runtime)

enable_testing()
add_subdirectory(tests) # 👈 确保核心层负责拉起它内部的测试

# =====================================================
# 聚合接口库 (Interface Library)
# 这是一个高级技巧：定义一个不产生实体文件的库，
# 让 gateway 或 game_server 链接 mmo_core 时，
# 能自动获得 infrastructure 的所有头文件路径和链接权限。
# =====================================================
add_library(mmo_core INTERFACE)
target_link_libraries(mmo_core INTERFACE
    mmo_infrastructure

    # mmo_runtime
)