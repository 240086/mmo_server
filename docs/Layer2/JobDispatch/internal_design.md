# JobDispatch Internal Design

## Overview

JobDispatch is the execution engine of Layer2.

TaskGraph supplies topology.

JobDispatch supplies runtime execution.

---

# Core Components

TaskGraph
↓
TaskRuntimeState
↓
ReadyQueue
↓
WorkerPool
↓
JobDispatch

---

# TaskRuntimeState

Purpose:

Maintain mutable runtime scheduling state.

Topology remains immutable.

Structure:

struct alignas(64) TaskRuntimeState final
{
std::atomic<uint32_t>
remainingDependencies{0};
};

Reason:

Prevent false sharing.

Each runtime state occupies an independent cache line.

---

# ReadyQueue

Backend:

MPMCQueue<TaskId>

Capacity:

TaskGraphMaxNodesV1

Properties:

* bounded
* lock-free
* fixed-size
* allocation-free

---

# WorkerPool

Worker count source:

RuntimeContext::workerCount

Properties:

* fixed size
* startup allocation only
* runtime stable

Responsibilities:

* fetch task
* execute task
* release successors

---

# Active Task Counter

Purpose:

Detect phase completion.

Structure:

std::atomic<uint32_t>
activeTaskCount;

Increment:

Task becomes runnable.

Decrement:

Task finishes execution.

---

# Barrier Model

Synchronization primitive:

std::atomic::wait

Workflow:

Main Thread

wait(activeTaskCount)

Worker Thread

notify_all()

Properties:

* no mutex
* no spin lock
* deterministic

---

# Task Execution

TaskId
↓
Function Registry
↓
TaskFunction
↓
Execute

Structure:

using TaskFunction =
void(*)(RuntimeContext&);

TaskFunction registry[TaskGraphMaxNodesV1];

No virtual dispatch.

No std::function.

No heap allocation.

---

# Runtime Complexity

Task Dispatch:

O(1)

Task Release:

O(child_count)

Ready Queue:

O(1)

Topology Traversal:

CSR-based

O(1) child range lookup

---

# Memory Guarantees

Runtime allocation:

None

Pointer ownership:

None

Topology mutation:

None

Dynamic graph rebuild:

Prohibited
