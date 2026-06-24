// /home/xj/projects/mmo_server/server_core/runtime/JobDispatch/include/mmo/runtime/JobDispatch/WorkerPool.hpp
#pragma once

#include <array>
#include <atomic>
#include "mmo/runtime/JobDispatch/JobDispatchConstants.hpp"
#include "mmo/runtime/JobDispatch/JobDispatchTypes.hpp"
#include "mmo/runtime/JobDispatch/WorkerContext.hpp"
#include "mmo/runtime/JobDispatch/WorkerThread.hpp"
#include "mmo/runtime/JobDispatch/WorkerPoolConfig.hpp"

namespace mmo::runtime
{

    /**
     * @brief 零分配、零自旋的工作物理线程池
     * @note 坚决杜绝通用线程池范式（禁止 Future/Promise/打包任务），完全由底层无锁 ReadyQueue 静态驱动
     */
    class WorkerPool final
    {
    public:
        WorkerPool() = default;
        ~WorkerPool() = default;

        WorkerPool(const WorkerPool &) = delete;
        WorkerPool &operator=(const WorkerPool &) = delete;
        WorkerPool(WorkerPool &&) = delete;
        WorkerPool &operator=(WorkerPool &&) = delete;

    public:
        /**
         * @brief 基于解耦配置面冷启动所有物理工作线程
         * * @warning 【CRITICAL LIFETIME CONTRACT / 严格生命周期契约】
         * 传入的 config 中包含的以下指针：
         * - config.executor
         * - config.completionSink
         * - config.readyQueue
         * 必须在内存中保持绝对有效，直到调用 Stop() 且 Join() 完全返回为止。
         * 若外部资源提前析构，将导致灾难性的悬挂指针（Dangling Pointer）引发段错误。
         * * @return 启动是否成功（如 workerCount 非法将返回 false）
         */
        /**
         * @brief 基于解耦配置面冷启动所有物理工作线程
         */
        bool Start(const WorkerPoolConfig &config);

        /**
         * @brief 触发物理停机标记
         */
        void Stop();

        /**
         * @brief 同步 Join 等待物理工作线程优雅退场
         */
        void Join();

        /**
         * @brief 获取当前线程池是否处于运转状态
         */
        [[nodiscard]] bool Running() const noexcept;

    private:
        /**
         * @brief 物理线程核心极速轮询回环
         * @note 状态机职责收敛：
         * while(running) { try_dequeue -> execute -> notify_sink }
         * 绝不干涉拓扑图入度计算与后继节点释放领域
         */
        void WorkerLoop(WorkerIndex workerIndex);

    private:
        WorkerPoolConfig m_config{};
        std::atomic<bool> m_running{false};

        // 采用定长数组锁死硬件线程空间，消灭动态内存分配开销
        static constexpr std::size_t kMaxHardwareWorkers = 32;
        std::uint16_t m_activeWorkerCount{0};

        std::array<WorkerThread, kMaxHardwareWorkers> m_threads{};
        std::array<WorkerContext, kMaxHardwareWorkers> m_contexts{};
    };

} // namespace mmo::runtime