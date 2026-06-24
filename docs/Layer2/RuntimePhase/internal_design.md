# Core Philosophy

RuntimePhase is a lightweight execution descriptor.

RuntimePhase is not a gameplay system.

RuntimePhase is not a task container.

RuntimePhase is not a scheduler.

Runtime Flow
TickScheduler

      ↓

RuntimePipeline

      ↓

RuntimePhase

      ↓

TaskGraph

      ↓

JobDispatch
Phase Model

V1:

Input

Simulation

PostSimulation

Output
Future MMO Mapping

Simulation Phase:

Movement

Combat

Skill

AI

AOI

Output Phase:

Replication

Event Flush

Network Dispatch
Complexity

Phase Lookup

O(1)

Phase Iteration

O(N)

N = phase count

Expected:

4 ~ 16
Storage Model

Recommended:

std::vector<RuntimePhase>

Reason:

Stable order

Cache friendly

Small N

No unordered_map.

No tree.

No graph.

V1 is linear.

最重要的架构决策

这里是整个 Layer2 最关键的一条。

我建议：

RuntimePhase 不使用字符串

禁止：

std::string phaseName;

禁止：

GetPhase("Simulation");

使用：

enum class RuntimePhaseId
{
    Input,

    Simulation,

    PostSimulation,

    Output
};

原因：

未来：

每 Tick 30次
每秒 30~60 Tick

持续数年运行

不应该有：

字符串比较
哈希查找
动态分配
