# /home/xj/projects/mmo_server/server_core/runtime/CMakeLists.txt
cmake_minimum_required(VERSION 3.22)

# 挂载底层并行拆分的运行时核心四剑客组件
add_subdirectory(RuntimeContext)
add_subdirectory(RuntimePhase)
add_subdirectory(RuntimePipeline)
add_subdirectory(TaskGraph) # 🚀 成功挂载核心多线程驱动任务图

# 汇聚成对外的统一运行时骨架公共接口库
add_library(mmo_runtime INTERFACE)

target_link_libraries(mmo_runtime
    INTERFACE
    mmo_runtime_context
    mmo_runtime_phase
    mmo_runtime_pipeline
    mmo_runtime_task_graph # 🎯 向上层完备输出任务图异步并行调度能力
    mmo_infrastructure
)

# 设置聚合库的现代 C++ 命名空间别名
add_library(mmo::runtime ALIAS mmo_runtime)