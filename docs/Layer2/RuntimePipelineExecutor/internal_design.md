# RuntimePipelineExecutor Internal Design

Version

V1

Layer

Layer2 Runtime

---

# 1. Internal Responsibilities

RuntimePipelineExecutor is responsible for deterministic execution of an immutable RuntimePipeline.

Internal responsibilities include:

- runtime phase iteration
- TaskGraph scheduling
- JobDispatch coordination
- completion synchronization
- runtime context updates

Topology construction is explicitly outside this module.

---

# 2. Internal Components

```
RuntimePipelineExecutor

├── ExecutePipeline()
├── ExecutePhase()
├── DispatchReadyTasks()
├── WaitPhaseCompletion()
├── CompleteTask()
└── UpdateRuntimeContext()
```

No additional helper objects are dynamically allocated.

---

# 3. Internal Data Members

```
RuntimePipeline&

RuntimeContext&

IJobDispatch&
```

Only references are stored.

No ownership exists.

---

# 4. Execution Pipeline

For one Tick:

```
ExecutePipeline()

↓

foreach Phase

↓

ExecutePhase()

↓

ResetGraph()

↓

DispatchReadyTasks()

↓

WaitCompletion()

↓

NextPhase()
```

---

# 5. ExecutePipeline()

Responsibilities

- iterate RuntimePipeline
- preserve order
- execute every phase exactly once

Pseudo code

```
for phase

    ExecutePhase(phase)
```

Complexity

```
O(P)
```

P = phase count

---

# 6. ExecutePhase()

Responsibilities

- reset graph runtime state
- schedule root tasks
- drive dependency execution
- wait until graph completes

Pseudo code

```
graph.Reset()

while !graph.Completed()

    DispatchReadyTasks()

    WaitPhaseCompletion()
```

---

# 7. DispatchReadyTasks()

Responsibilities

Collect every executable task.

Submit all tasks to JobDispatch.

Pseudo code

```
ready = graph.CollectReady()

for task

    dispatch.Submit(task)
```

PipelineExecutor never executes task bodies.

---

# 8. Completion Processing

Worker Thread

↓

Task Finished

↓

CompletionSink

↓

PipelineExecutor

↓

graph.MarkCompleted()

↓

New Ready Tasks

This guarantees deterministic dependency traversal.

---

# 9. RuntimeContext Update

Before every phase

```
CurrentPhase
```

After phase

```
CompletedPhase
```

Pipeline completion

```
PipelineFinished
```

TickScheduler remains responsible for TickId.

---

# 10. Thread Interaction

Main Thread

```
PipelineExecutor

TaskGraph Scheduler
```

Worker Threads

```
Task Execution
```

Synchronization point

```
WaitPhaseCompletion()
```

---

# 11. Synchronization Strategy

PipelineExecutor contains

No mutex

No condition_variable

No spin lock

Synchronization is delegated entirely to JobDispatch.

---

# 12. Memory Strategy

Runtime Tick

Heap Allocation

None

Task Allocation

None

Phase Allocation

None

Pipeline Allocation

None

Temporary state is allocated on stack only.

---

# 13. Failure Handling

Pipeline missing

↓

throw

TaskGraph invalid

↓

throw

Dependency cycle

↓

throw

Dispatch failure

↓

throw

Partial execution rollback is not supported.

---

# 14. Complexity Analysis

ExecutePipeline

```
O(P)
```

ExecutePhase

```
O(V+E)
```

Dispatch

```
O(R)
```

R = ready task count

Overall Tick

```
O(P+V+E)
```

---

# 15. Design Invariants

Pipeline topology never changes.

TaskGraph topology never changes.

Every phase executes exactly once.

No phase overlap.

No callback executes on PipelineExecutor stack.

RuntimeContext remains single-owner.
