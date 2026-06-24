V1：

std::array<
    RuntimePhase,
    RuntimePhaseConstants::PhaseCount
>

不要：

std::vector

原因：

Phase 数量固定。

不会动态增长。

复杂度：

GetPhase()

O(1)

Execute()

O(N)

N = 4
