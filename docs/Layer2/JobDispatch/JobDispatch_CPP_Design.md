# Layer2 / JobDispatch CPP Design Document V1

文件路径：

```text
/home/xj/projects/mmo_server/docs/Layer2/JobDispatch/JobDispatch_CPP_Design.md
```

---

# 1. Document Status

| Item           | Status                     |
| -------------- | -------------------------- |
| Module         | JobDispatch                |
| Layer          | Layer2 Runtime & Scheduler |
| Version        | V1                         |
| Architecture   | Frozen                     |
| HPP Contracts  | Frozen                     |
| CPP Design     | Frozen                     |
| Implementation | Not Started                |
| Validation     | Not Started                |

---

# 2. Module Position

JobDispatch 是 Layer2 Runtime Scheduler 的核心并行执行引擎。

其职责是：

```text
TaskGraph
    ↓
JobDispatch
    ↓
WorkerPool
    ↓
TaskFunction
```

不负责：

```text
Task Construction
Task Dependency Authoring
Gameplay Logic
Scene Simulation
Network Dispatch
Actor Lifecycle
```

---

# 3. Layer Architecture Position

```text
L1 Infrastructure
│
├── LockFreeQueue
├── Logger
├── MemoryPool
├── Timer
└── Metrics
        ↑

L2 Runtime
│
├── RuntimePipeline
├── RuntimePhase
├── TaskGraph
├── TickScheduler
├── JobDispatch
└── RuntimeContext
        ↑

L3 Simulation
│
├── Scene
├── Entity
├── Combat
├── Skill
├── AI
└── Movement
```

JobDispatch：

```text
Uses:
    LockFreeQueue
    RuntimeContext
    TaskGraph

Provides:
    Parallel Task Execution
```

---

# 4. Design Principles

---

## 4.1 Single Responsibility

WorkerPool：

```text
Execute Only
```

JobDispatch：

```text
Schedule Only
```

TaskGraph：

```text
Topology Only
```

---

## 4.2 Runtime Allocation Free

ExecutePhase() 热路径：

```text
No malloc
No free
No shared_ptr
No unique_ptr
No vector resize
```

允许：

```text
Atomic
Array
MPMCQueue
```

---

## 4.3 Deterministic Topology

同一 TaskGraph：

```text
same input

↓

same dependency release order

↓

same completion state
```

---

## 4.4 O(V+E)

必须保证：

```text
BuildRuntimeState     O(V)

SeedReadyQueue        O(V)

ReleaseSuccessors     O(E)

ExecutePhase          O(V+E)
```

禁止：

```text
O(V²)

O(E²)

Repeated Graph Scan
```

---

# 5. Source File Layout

```text
runtime/JobDispatch/src

├── TaskRegistry.cpp

├── WorkerPool.cpp

├── JobDispatch.cpp

├── JobDispatchPhase.cpp

├── JobDispatchRuntimeState.cpp

├── JobDispatchCompletion.cpp

└── JobDispatchSynchronization.cpp
```

---

# 6. TaskRegistry.cpp

职责：

```text
Registry Validation
```

实现：

```cpp
bool Validate(const TaskGraph&)
```

检查：

```text
TaskId合法

TaskFunction已注册

Node引用合法

Graph节点数量合法
```

复杂度：

```text
O(V)
```

执行时机：

```text
Initialize Only
```

---

# 7. WorkerPool.cpp

---

## Responsibilities

```text
Thread Creation

Thread Lifetime

Task Fetch

Task Execute

Completion Notify
```

---

## Non Responsibilities

```text
Topology

Dependency

Graph

Phase

Scheduler
```

---

## Worker State Machine

```text
Running

↓

TryPop

↓

ExecuteTask

↓

OnTaskCompleted

↓

Loop
```

---

## Core Loop

```cpp
while (running)
{
    TaskId task;

    if (!TryPop(task))
    {
        wait...
        continue;
    }

    ExecuteTask(task);

    OnTaskCompleted(task);
}
```

---

# 8. JobDispatch.cpp

职责：

```text
Initialize

Shutdown
```

---

## Initialize

负责：

```text
Graph Validation

Registry Validation

WorkerPool Startup

Resource Binding
```

流程：

```text
TaskGraph

↓

Validate

↓

Build Worker Config

↓

WorkerPool.Start
```

---

## Shutdown

流程：

```text
Stop

↓

Join

↓

Cleanup
```

---

# 9. JobDispatchPhase.cpp

职责：

```text
ExecutePhase
```

---

流程：

```text
BuildRuntimeState

↓

SeedReadyQueue

↓

WaitForPhaseCompletion

↓

Return
```

---

复杂度：

```text
O(V+E)
```

---

# 10. JobDispatchRuntimeState.cpp

---

## BuildRuntimeState

输入：

```text
TaskGraph
```

输出：

```text
RuntimeState
```

实现：

```cpp
remainingDependencies.store(
    initialInDegree
);
```

复杂度：

```text
O(V)
```

---

## SeedReadyQueue

实现：

```text
Find InDegree == 0

↓

Push Ready Queue
```

复杂度：

```text
O(V)
```

---

## ReleaseSuccessors

实现：

```cpp
for successor
{
    fetch_sub()

    if (prev == 1)
    {
        enqueue()
    }
}
```

复杂度：

```text
O(E)
```

---

# 11. JobDispatchCompletion.cpp

职责：

```text
Task Completion Handling
```

---

流程：

```text
Task Complete

↓

ReleaseSuccessors

↓

ActiveTaskCount--

↓

Last Task

↓

Epoch++

↓

NotifyAll
```

---

核心：

```cpp
prev =
    fetch_sub();

if (prev == 1)
{
    epoch++;

    notify_all();
}
```

---

# 12. JobDispatchSynchronization.cpp

职责：

```text
Phase Barrier
```

---

机制：

```text
Generation Barrier
```

---

必须使用：

```cpp
atomic.wait

atomic.notify_all
```

---

参考实现：

```cpp
for (;;)
{
    if (activeCount == 0)
    {
        return;
    }

    auto epoch =
        phaseEpoch.load();

    if (activeCount == 0)
    {
        return;
    }

    phaseEpoch.wait(epoch);
}
```

---

目标：

```text
No Lost Wakeup

No Busy Spin

No Condition Variable
```

---

# 13. Thread Ownership Model

唯一所有权：

```text
JobDispatch

    owns

WorkerPool
```

---

WorkerPool：

```text
owns

Worker Threads
```

---

外部引用：

```text
TaskGraph

RuntimeContext
```

采用：

```text
Outlive Contract
```

要求：

```text
Lifetime >

JobDispatch
```

---

# 14. Future Extension Points

V1 不实现：

```text
NUMA Awareness

CPU Affinity

Work Stealing

Priority Scheduling

Fiber Scheduler

Task Batching

Distributed Scheduling
```

预留：

```text
WorkerContext

WorkerPoolConfig

TaskRuntimeState
```

---

# 15. Validation Plan

---

## Unit Tests

```text
TaskRegistry
WorkerPool
JobDispatch
```

---

## Topology Tests

```text
Linear DAG

Diamond DAG

Multi Root DAG

Multi Sink DAG
```

---

## Stress Tests

```text
10,000 Tick

100,000 Tick

1,000,000 Task Completion
```

---

## Sanitizers

必须通过：

```text
ASAN

UBSAN

TSAN
```

---

# 16. Freeze Conclusion

CPP Design Status:

```text
FROZEN_V1
```

进入阶段：

```text
TaskRegistry.cpp Review

↓

WorkerPool.cpp Review

↓

JobDispatch.cpp Review

↓

Implementation

↓

Validation

↓

Freeze V1
```
