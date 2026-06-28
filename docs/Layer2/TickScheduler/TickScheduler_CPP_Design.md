# TickScheduler CPP Design

Version:
v1

Status:
Frozen Candidate

Layer:
Layer2 Runtime

Module:
TickScheduler

---

# Design Goal

TickScheduler is the root deterministic runtime driver.

Responsibilities:

- Fixed-step tick progression
- RuntimeContext advancement
- TimerQueue advancement
- RuntimePipeline execution
- Runtime lifecycle management

TickScheduler does NOT:

- Execute gameplay logic
- Execute tasks directly
- Manage worker threads
- Manage task graphs
- Perform dependency scheduling

Those responsibilities belong to:

RuntimePipeline

JobDispatch

TaskGraph

WorkerPool

---

# Runtime Ownership Model

Runtime

└── TickScheduler
        │
        ├── RuntimeContext
        │
        ├── RuntimePipeline
        │
        ├── TimerQueue
        │
        └── IClock

Ownership Rules:

RuntimeContext:

Owned

RuntimePipeline:

Referenced

TimerQueue:

Referenced

IClock:

Referenced

---

# Core Execution Flow

RunUntilStopped()

↓

Initialize Validation

↓

Runtime Loop

↓

WaitNextTick()

↓

RunOneTick()

↓

Shutdown

---

# RunOneTick Workflow

RunOneTick()

↓

Validate State

↓

UpdateRuntimeContext()

↓

ProcessTimers()

↓

ExecutePipeline()

↓

AdvanceTick()

↓

Return

Execution order is immutable.

Execution order must never change.

---

# RuntimeContext Update Workflow

UpdateRuntimeContext()

Updates:

TickId

DeltaTime

Rules:

TickId:

N

↓

N + 1

FrameIndex:

N

↓

N + 1

DeltaTime:

Fixed

Configured by TickSchedulerConfig

---

# Timer Processing Workflow

TimerQueue.Tick(now)

must execute before:

RuntimePipeline.Execute(context)

Reason:

Timer-triggered runtime events
must become visible within the same tick.

Execution Order:

Context Update

↓

TimerQueue.Tick(now)

↓

Pipeline.Execute(context)

---

# Pipeline Execution Workflow

V1 RuntimePipeline is a passive phase container.

TickScheduler only advances runtime state.

RuntimePipeline execution is not yet integrated in V1.

Future versions may introduce:

RuntimePipelineExecutor
or
JobDispatch integration.

---

# Tick Advancement Workflow

AdvanceTick()

Operations:

context.tickId += 1

context.frameIndex += 1

Rules:

Must execute only after successful pipeline completion.

Must execute exactly once.

---

# WaitNextTick Algorithm

Goal:

Maintain deterministic fixed timestep execution.

Clock Source:

IClock

Scheduling Strategy:

sleep_until(nextTickTime)

Pseudo:

while(now < nextTickTime)
{
    sleep_until(nextTickTime);
}

Properties:

Low CPU usage

Deterministic pacing

No busy-spin

---

# Tick Overrun Policy

Definition:

Pipeline execution exceeds tick interval.

Example:

Tick Interval:

50ms

Actual Tick Cost:

73ms

Overrun:

23ms

Policy:

Log Warning

Continue Runtime

No Catch-Up

No Tick Compression

No Rollback

Reason:

Determinism first.

Future V2 may introduce catch-up strategy.

V1 explicitly forbids catch-up.

---

# State Machine

Created

↓

Initialize()

↓

Initialized

↓

RunUntilStopped()

↓

Running

↓

Shutdown()

↓

Stopped

Forbidden:

Created
    ↓
RunOneTick()

Stopped
    ↓
RunOneTick()

Stopped
    ↓
Initialize()

Double Initialize()

Double Shutdown()

All invalid transitions:

Fail Fast

---

# Failure Handling

Policy:

Fail Fast

Failure Cases:

Null Pipeline

Null TimerQueue

Null Clock

Invalid State

Pipeline Execution Failure

RuntimeContext Corruption

Action:

Immediate Runtime Termination

No recovery.

No fallback.

No silent ignore.

---

# Threading Model

V1 Runtime Thread Only

Writer:

TickScheduler

Readers:

Worker Threads

RuntimeContext Ownership:

Single Writer

Multiple Readers

Thread Safety Strategy:

TickScheduler writes context
before pipeline execution.

Worker threads observe
stable tick state.

No concurrent writes allowed.

---

# Memory Model

RuntimeContext:

Embedded Object

Pipeline:

Reference

TimerQueue:

Reference

Clock:

Reference

Dynamic Allocation:

None

Per Tick Allocation:

None

Heap Allocation During Tick:

Forbidden

---

# Complexity Analysis

RunOneTick()

Time:

O(1)
+
Pipeline Execution Cost

Space:

O(1)

UpdateRuntimeContext()

Time:

O(1)

Space:

O(1)

WaitNextTick()

Time:

O(1)

Space:

O(1)

---

# Cache Behavior

Hot Data:

RuntimeContext

TickSchedulerState

TickSchedulerConfig

Characteristics:

Contiguous

Small footprint

Cache friendly

No dynamic containers

No pointer chasing

---

# TSAN Analysis

Writer:

Runtime Thread

Readers:

Worker Threads

Synchronization Boundary:

RuntimePipeline execution barrier

Requirements:

RuntimeContext fields
shared with worker threads

must follow RuntimeContext contract.

No unsynchronized shared mutable state
inside TickScheduler.

Expected Result:

TSAN Clean

---

# ASAN Analysis

No raw ownership pointers.

No manual memory management.

Expected Result:

ASAN Clean

---

# UBSAN Analysis

Potential Risks:

State transition errors

Tick overflow

Mitigation:

Fail Fast

Runtime assertions

Expected Result:

UBSAN Clean

---

# Valgrind Analysis

Dynamic Allocation:

None

Expected Result:

0 Leak

0 Invalid Access

---

# Future Evolution Boundary

Allowed:

Catch-Up Policy

Pause / Resume

Distributed Tick Source

Multi Scene Tick Domain

Adaptive Tick Pacing

Forbidden In V1:

Variable Tick Interval

Tick Rollback

Runtime Graph Mutation

Dynamic Phase Reordering

Pipeline Retry

Multi Writer RuntimeContext

---

# Integration Requirements

Required Dependencies:

RuntimeContext

RuntimePipeline

TimerQueue

IClock

Required Preconditions:

Initialize() completed

Runtime state valid

Pipeline valid

TimerQueue valid

Clock valid

Required Postconditions:

Tick completed

Context advanced

Pipeline executed

Determinism preserved

---

# Freeze Criteria

All unit tests pass

Lifecycle tests pass

Tick progression tests pass

Timer integration tests pass

Pipeline invocation tests pass

Overrun tests pass

TSAN pass

ASAN pass

UBSAN pass

Valgrind pass

No dynamic allocation during tick

Deterministic execution verified

ABI stable

Ready for Layer3 integration
