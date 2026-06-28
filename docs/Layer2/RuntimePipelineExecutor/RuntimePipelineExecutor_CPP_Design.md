# RuntimePipelineExecutor CPP Design

Version:
    v1.0

Status:
    Frozen (CPP Design)

Layer:
    Runtime Layer (L2)

Module:
    RuntimePipelineExecutor

--------------------------------------------------------------------

# 1. Purpose

RuntimePipelineExecutor is responsible for deterministic execution of
the RuntimePipeline during each runtime tick.

It is the execution engine of the runtime pipeline.

Responsibilities:

- Traverse RuntimePipeline
- Execute RuntimePhase in deterministic order
- Drive phase lifecycle
- Propagate RuntimeContext
- Maintain runtime execution consistency

It does NOT:

- own RuntimePipeline
- own RuntimePhase
- schedule worker threads
- build TaskGraph
- allocate runtime objects

--------------------------------------------------------------------

# 2. Ownership

Ownership model

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

RuntimePipelineExecutor never owns any RuntimePhase.

RuntimePipeline owns phase lifetime.

--------------------------------------------------------------------

# 3. Dependencies

Required modules

RuntimeContext

RuntimePipeline

RuntimePhase

Future dependencies

TaskGraph

JobDispatch

Metrics

Profiler

Logger

No Layer3 dependency is allowed.

--------------------------------------------------------------------

# 4. Internal Members

RuntimePipelineExecutor stores only references.

Example

RuntimePipeline&

No runtime data is permanently cached.

No execution state survives across ticks.

--------------------------------------------------------------------

# 5. Execute() Workflow

Execution algorithm

Execute(context)

↓

Validate executor state

↓

Read pipeline

↓

Traverse phases

↓

Execute each phase

↓

Return

Pseudo flow

for phase in pipeline

    ExecutePhase(phase)

No phase reordering.

No dynamic insertion.

No runtime sorting.

Traversal order equals pipeline order.

--------------------------------------------------------------------

# 6. ExecutePhase()

ExecutePhase()

Responsibilities

Validate phase

↓

Update runtime context

↓

Invoke phase

↓

Return

V1 performs direct phase invocation.

Future versions may replace this with

TaskGraph

↓

JobDispatch

↓

WorkerPool

without changing Execute().

--------------------------------------------------------------------

# 7. RuntimeContext Flow

RuntimeContext

↓

TickScheduler

↓

PipelineExecutor

↓

RuntimePhase

All phases observe the same RuntimeContext instance.

Executor never creates copies.

No RuntimeContext ownership transfer.

--------------------------------------------------------------------

# 8. State Machine

Created

↓

Initialized

↓

Running

↓

Stopped

Execute()

may only be called while

Running

Calling Execute()

outside Running

is programming error.

--------------------------------------------------------------------

# 9. Determinism

Guaranteed

Pipeline order

Stable traversal

Single execution

Same RuntimeContext

No parallel phase execution

No random ordering

No iterator invalidation

--------------------------------------------------------------------

# 10. Exception Policy

RuntimePipelineExecutor itself shall not throw.

Programming errors

↓

assert

or

logic_error

during initialization only.

Runtime tick execution

must never propagate exceptions.

Future RuntimePhase failures

shall be handled inside executor.

Tick boundary must remain valid.

--------------------------------------------------------------------

# 11. Memory Policy

RuntimePipelineExecutor owns no heap objects.

No dynamic allocation inside Execute().

No std::shared_ptr creation.

No std::unique_ptr creation.

No vector growth.

No container rebuild.

Execution should be allocation free.

--------------------------------------------------------------------

# 12. Threading Model

Execute()

Runtime thread only.

Reset()

Runtime thread only.

Shutdown()

Runtime thread only.

No internal synchronization.

No mutex.

No atomic.

Thread safety is provided by Runtime ownership.

--------------------------------------------------------------------

# 13. Future Integration

Current

Execute()

↓

RuntimePhase

Future

Execute()

↓

TaskGraphBuilder

↓

TaskGraph

↓

JobDispatch

↓

WorkerPool

↓

Completion Barrier

↓

Next Phase

The public interface remains unchanged.

--------------------------------------------------------------------

# 14. Performance Targets

Time complexity

O(P)

P

number of RuntimePhase

Memory complexity

O(1)

Additional allocations

0

Phase traversal

linear

Cache behavior

sequential traversal

--------------------------------------------------------------------

# 15. Failure Philosophy

Pipeline execution prioritizes

determinism

over

maximum throughput.

Execution order

must never change.

Skipped execution

is preferred over

undefined runtime state.

Simulation correctness

takes priority over latency.

--------------------------------------------------------------------

# 16. Extension Points

Reserved for V2

TaskGraph generation

Parallel execution

Dependency scheduling

Barrier synchronization

Profiler hooks

Metrics collection

Runtime tracing

Coroutine scheduling

These extensions must not require ABI changes.

--------------------------------------------------------------------

# 17. V1 Constraints

Allowed

Sequential phase traversal

Single runtime thread

Reference-only ownership

Stable execution order

Forbidden

Parallel RuntimePhase execution

Dynamic phase insertion

Pipeline mutation

Background execution

Worker ownership

Runtime object ownership

Dynamic scheduling

--------------------------------------------------------------------

# 18. Design Summary

RuntimePipelineExecutor is intentionally lightweight.

It serves as the execution bridge between

TickScheduler

and

RuntimePipeline.

Its implementation focuses on

deterministic execution,

minimal ownership,

stable execution order,

and future compatibility with

TaskGraph

and

JobDispatch,

without introducing unnecessary complexity into V1.

--------------------------------------------------------------------

# 19. Internal Call Graph

TickScheduler

↓

RunOneTick()

↓

RuntimePipelineExecutor::Execute()

↓

for RuntimePhase

↓

ExecutePhase()

↓

RuntimePhase::Execute()

↓

(return)

↓

Next RuntimePhase

↓

(return)

↓

Tick End

No recursion.

No nested pipeline execution.

No reentrant execution.

--------------------------------------------------------------------

# 20. Future Evolution Graph

V1

TickScheduler

↓

PipelineExecutor

↓

RuntimePhase

--------------------------------

V2

TickScheduler

↓

PipelineExecutor

↓

TaskGraphBuilder

↓

TaskGraph

↓

JobDispatch

↓

WorkerPool

↓

Completion

↓

Next Phase

--------------------------------

V3

TickScheduler

↓

PipelineExecutor

↓

TaskGraph

↓

Coroutine Scheduler

↓

WorkerPool

↓

Frame Barrier

↓

RuntimePhase Complete
