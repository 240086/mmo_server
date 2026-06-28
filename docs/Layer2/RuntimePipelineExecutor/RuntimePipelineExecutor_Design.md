# RuntimePipelineExecutor Design

Version

V1 Architecture Frozen

Layer

Layer2 Runtime

Module

RuntimePipelineExecutor

1. Module Responsibilities

RuntimePipelineExecutor is responsible for deterministic execution of one RuntimePipeline.

It acts as the execution engine between TickScheduler and RuntimePipeline.

Responsibilities:

execute RuntimePhase sequentially
invoke TaskGraph execution
synchronize JobDispatch
update RuntimeContext
provide deterministic runtime ordering

It owns execution only.

It never owns pipeline topology.

Pipeline topology belongs to RuntimePipeline.

1. Position inside Runtime
TickScheduler
      │
      ▼
RuntimePipelineExecutor
      │
      ▼
RuntimePipeline
      │
      ▼
RuntimePhase
      │
      ▼
TaskGraph
      │
      ▼
JobDispatch

PipelineExecutor 是真正的 Runtime Driver。

TickScheduler 负责时间。

PipelineExecutor 负责执行。

1. Design Philosophy

PipelineExecutor must never build runtime topology.

PipelineExecutor never creates phases.

PipelineExecutor never edits TaskGraph.

PipelineExecutor only executes.

This separation guarantees:

deterministic runtime
immutable runtime topology
simple testing
future hot reload
4. Execution Ownership

PipelineExecutor owns

execution order
runtime phase iteration
synchronization
completion waiting

Pipeline owns

phases
topology

TaskGraph owns

dependency graph

JobDispatch owns

worker threads
5. Execution Flow

For every Tick

TickScheduler

↓

RuntimePipelineExecutor

↓

for phase in pipeline

↓

ExecutePhase()

↓

TaskGraph

↓

Submit Ready Tasks

↓

WorkerPool

↓

Wait Completion

↓

Next Phase

↓

Finish Tick
6. Runtime Algorithm
ExecutePipeline()

for phase

    ExecutePhase()

        TaskGraph.Reset()

        while graph not completed

            CollectReadyTasks()

            SubmitTasks()

            WaitFinishedTasks()

            NotifyGraph()

return

No phase overlap.

No graph overlap.

No task reordering.

1. Phase Execution Rule

Each phase executes independently.

Phase A

↓

complete

↓

Phase B

↓

complete

↓

Phase C

Forbidden

A

↓

B starts before A ends
8. TaskGraph Interaction

PipelineExecutor may call

Reset()

CollectReadyTasks()

MarkCompleted()

IsFinished()

It must never modify dependency topology.

1. JobDispatch Interaction

PipelineExecutor submits tasks.

JobDispatch executes tasks.

PipelineExecutor waits.

Submit()

↓

Worker

↓

Complete

↓

CompletionSink

↓

PipelineExecutor

No busy loop.

1. RuntimeContext Interaction

Before phase

CurrentPhase

TickId

DeltaTime

After phase

PhaseCompleted

RuntimeContext remains single owner.

1. TickScheduler Interaction

TickScheduler calls

PipelineExecutor.Execute()

PipelineExecutor never sleeps.

PipelineExecutor never controls Tick.

1. Failure Policy

Pipeline topology error

↓

throw

Task dependency cycle

↓

throw

worker failure

↓

throw

phase missing

↓

throw

fail-fast

1. Thread Model

Main Thread

TickScheduler

PipelineExecutor

TaskGraph Scheduling

Worker Threads

JobDispatch

Execute Task

Callbacks always return to main runtime thread.

1. Determinism

Guaranteed

Phase Order

YES

Task Dependency

YES

Same Tick Order

YES

RuntimeContext Update

YES

Worker Completion Order

Normalized

Callback Thread

Main Runtime Thread

1. State Machine
Idle

↓

ExecutingPipeline

↓

ExecutingPhase

↓

DispatchTasks

↓

WaitingTasks

↓

CompletePhase

↓

NextPhase

↓

PipelineFinished

↓

Idle
16. Dependency Graph
RuntimePipelineExecutor

├── RuntimePipeline
├── RuntimeContext
├── TaskGraph
├── JobDispatch
└── ITaskCompletionSink

Forbidden

Timer

Network

Scene

Entity

Combat

AI

Executor 不允许直接依赖 L1 基础设施（除 Runtime 所需的调度接口）之外的业务模块，也不允许依赖 L3/L4/L5。

1. Locking Strategy

PipelineExecutor itself

No mutex

No atomic scheduling

No lock-free container

Synchronization delegated to JobDispatch.

1. Memory Ownership

Never owns

Pipeline

TaskGraph

JobDispatch

Only references

RuntimePipeline&

RuntimeContext&

IJobDispatch&

No dynamic allocation during Tick.

1. Extension Points

Future

PipelineExecutor

├── Profiler Hook
├── Metrics Hook
├── Trace Hook
├── Debug Hook
├── ECS Scheduler Hook
└── Coroutine Hook

Core execution algorithm must remain unchanged.

1. Non-Responsibilities

PipelineExecutor never

builds pipeline
creates TaskGraph
owns worker threads
schedules timers
updates network
processes gameplay
performs resource loading

It is a deterministic execution coordinator only.

1. Performance Targets
Item Target
Dynamic allocation per tick 0
Phase traversal O(P)
Task scheduling O(V+E)（由 TaskGraph 决定）
Phase execution ordering Deterministic
Main-thread blocking Only waiting for current phase completion
Tick overlap Forbidden
2. Design Invariants

整个模块应保持以下不可变约束（Invariants）：

Pipeline Topology 在执行期间不可修改。
同一 Tick 内所有 Phase 必须严格串行执行。
每个 Phase 完成后才能进入下一个 Phase。
TaskGraph 仅负责依赖分析，不负责线程调度。
JobDispatch 仅负责执行任务，不负责决定执行顺序。
PipelineExecutor 是 Runtime 主线程唯一的执行协调者。
TickScheduler 不感知 TaskGraph 和 JobDispatch 的实现细节，只调用 ExecutePipeline()。
整个执行过程保持确定性（Deterministic），即相同输入产生相同执行顺序。
