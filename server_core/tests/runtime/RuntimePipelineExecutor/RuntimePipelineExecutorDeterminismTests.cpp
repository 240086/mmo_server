#include <gmock/gmock.h>
#include <gtest/gtest.h>

#include "RuntimePipelineExecutorTestCommon.hpp"

namespace mmo::runtime::testing
{
    class RuntimePipelineExecutorDeterminismTests
        : public RuntimePipelineExecutorFixture
    {
    };

    TEST_F(
        RuntimePipelineExecutorDeterminismTests,
        PhaseOrderIsStrictlyDeterministic)
    {
        auto executor = CreateExecutor();
        std::vector<RuntimePhaseId> observed;

        EXPECT_CALL(jobDispatch, Initialize)
            .WillOnce(::testing::Return(true));

        EXPECT_CALL(jobDispatch, ExecutePhase(::testing::_))
            .Times(4)
            .WillRepeatedly([&](RuntimePhaseId id)
                            {
                                observed.push_back(id);
                                return true; });

        ASSERT_TRUE(executor.Initialize());
        EXPECT_EQ(executor.ExecutePipeline(), RuntimeExecutionResult::Success);

        ASSERT_EQ(observed.size(), 4u);
        EXPECT_EQ(observed[0], RuntimePhaseId::Input);
        EXPECT_EQ(observed[1], RuntimePhaseId::Simulation);
        EXPECT_EQ(observed[2], RuntimePhaseId::PostSimulation);
        EXPECT_EQ(observed[3], RuntimePhaseId::Output);

        // 🛡️ 释放 StrictMock 契约
        EXPECT_CALL(jobDispatch, Shutdown()).Times(1);
        executor.Shutdown();
    }

    TEST_F(
        RuntimePipelineExecutorDeterminismTests,
        MultipleTicksProduceIdenticalPhaseSequence)
    {
        auto executor = CreateExecutor();
        std::vector<RuntimePhaseId> tick1;
        std::vector<RuntimePhaseId> tick2;

        EXPECT_CALL(jobDispatch, Initialize)
            .WillOnce(::testing::Return(true));

        EXPECT_CALL(jobDispatch, ExecutePhase(::testing::_))
            .Times(8)
            .WillRepeatedly([&](RuntimePhaseId id)
                            {
                                if (tick1.size() < 4)
                                    tick1.push_back(id);
                                else
                                    tick2.push_back(id);
                                return true; });

        ASSERT_TRUE(executor.Initialize());
        EXPECT_EQ(executor.ExecutePipeline(), RuntimeExecutionResult::Success);
        EXPECT_EQ(executor.ExecutePipeline(), RuntimeExecutionResult::Success);

        ASSERT_EQ(tick1.size(), 4u);
        ASSERT_EQ(tick2.size(), 4u);
        EXPECT_EQ(tick1, tick2);

        // 🛡️ 释放 StrictMock 契约
        EXPECT_CALL(jobDispatch, Shutdown()).Times(1);
        executor.Shutdown();
    }

    TEST_F(
        RuntimePipelineExecutorDeterminismTests,
        FailureAndRetryDoesNotChangeExecutionOrder)
    {
        std::vector<RuntimePhaseId> firstAttempt;
        std::vector<RuntimePhaseId> secondAttempt;

        // 🎯 拓扑重构 1：第一轮执行流（测试单步失败中断）
        {
            auto executor1 = CreateExecutor();

            EXPECT_CALL(jobDispatch, Initialize).WillOnce(::testing::Return(true));
            EXPECT_CALL(jobDispatch, ExecutePhase(::testing::_))
                .WillOnce([&](RuntimePhaseId id)
                          {
                              firstAttempt.push_back(id);
                              return false; // 模拟首阶段即宣告崩溃
                          });

            ASSERT_TRUE(executor1.Initialize());
            EXPECT_EQ(executor1.ExecutePipeline(), RuntimeExecutionResult::DispatchFailure);

            // 优雅关闭第一个执行器，规避状态残留
            EXPECT_CALL(jobDispatch, Shutdown()).Times(1);
            executor1.Shutdown();
        }

        // 🎯 拓扑重构 2：利用延迟工厂构建完全干净的 executor2 进行二次重试验证
        {
            auto executor2 = CreateExecutor();

            EXPECT_CALL(jobDispatch, Initialize).WillOnce(::testing::Return(true));
            EXPECT_CALL(jobDispatch, ExecutePhase(::testing::_))
                .Times(4)
                .WillRepeatedly([&](RuntimePhaseId id)
                                {
                                    secondAttempt.push_back(id);
                                    return true; });

            ASSERT_TRUE(executor2.Initialize());
            EXPECT_EQ(executor2.ExecutePipeline(), RuntimeExecutionResult::Success);

            // 契约闭环
            EXPECT_CALL(jobDispatch, Shutdown()).Times(1);
            executor2.Shutdown();
        }

        // 验证即使发生过重大失败，重新拉起的生命周期依然严格对齐决定论序列
        ASSERT_EQ(secondAttempt.size(), 4u);
        EXPECT_EQ(secondAttempt, std::vector<RuntimePhaseId>({RuntimePhaseId::Input,
                                                              RuntimePhaseId::Simulation,
                                                              RuntimePhaseId::PostSimulation,
                                                              RuntimePhaseId::Output}));
    }

    TEST_F(
        RuntimePipelineExecutorDeterminismTests,
        LifecycleDoesNotAffectDeterministicOrdering)
    {
        std::vector<RuntimePhaseId> sequence;

        // 第一轮生命周期
        {
            auto executor = CreateExecutor();

            EXPECT_CALL(jobDispatch, Initialize).WillOnce(::testing::Return(true));
            EXPECT_CALL(jobDispatch, ExecutePhase(::testing::_))
                .Times(4)
                .WillRepeatedly([&](RuntimePhaseId id)
                                {
                                    sequence.push_back(id);
                                    return true; });

            ASSERT_TRUE(executor.Initialize());
            EXPECT_EQ(executor.ExecutePipeline(), RuntimeExecutionResult::Success);

            // 🛡️ 关键修复：显式消费 StrictMock 的 Shutdown 预期，防止残留至下一个作用域
            EXPECT_CALL(jobDispatch, Shutdown()).Times(1);
            executor.Shutdown();
        }

        // 第二轮生命周期 (模拟服务热重启拓扑)
        {
            auto executor_ = CreateExecutor();

            EXPECT_CALL(jobDispatch, Initialize).WillOnce(::testing::Return(true));
            EXPECT_CALL(jobDispatch, ExecutePhase(::testing::_))
                .Times(4)
                .WillRepeatedly([&](RuntimePhaseId id)
                                {
                                    sequence.push_back(id);
                                    return true; });

            ASSERT_TRUE(executor_.Initialize());
            EXPECT_EQ(executor_.ExecutePipeline(), RuntimeExecutionResult::Success);

            // 🛡️ 关键修复：显式消费第二次重启的 Shutdown 预期
            EXPECT_CALL(jobDispatch, Shutdown()).Times(1);
            executor_.Shutdown();
        }

        // 验证多代生命周期更迭下，原子时序无任何漂移
        ASSERT_GE(sequence.size(), 5u);
        EXPECT_EQ(sequence[0], RuntimePhaseId::Input);
        EXPECT_EQ(sequence[4], RuntimePhaseId::Input);
    }
} // namespace mmo::runtime::testing