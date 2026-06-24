Tick Lifecycle
Design Goal

The runtime executes simulation through deterministic fixed-step ticks.

A tick represents a complete and isolated simulation cycle.

All simulation updates must occur inside ticks.

No simulation state may be modified outside tick execution.

Core Philosophy

The tick is the smallest deterministic execution unit.

Tick N

↓

Complete

↓

Tick N+1

A tick must never overlap another tick.

Tick States

Each tick progresses through the following lifecycle.

Created

↓

Pipeline Build

↓

TaskGraph Build

↓

Task Dispatch

↓

Task Execution

↓

Barrier Synchronization

↓

Commit

↓

Completed
Phase 1
Tick Creation

Runtime creates a new tick context.

Responsibilities:

assign TickId
assign FrameIndex
assign DeltaTime
initialize execution state

Output:

RuntimeContext

No simulation code executes during this phase.

Phase 2
Pipeline Build

RuntimePipeline determines execution phases.

Example:

Input

↓

Movement

↓

AOI

↓

Skill

↓

Combat

↓

Output

Pipeline order must remain deterministic.

Runtime must never reorder phases dynamically.

Phase 3
TaskGraph Build

Each phase generates tasks.

Example:

Movement Phase

↓

MovementTask_A

MovementTask_B

MovementTask_C

Task dependencies are explicitly declared.

Example:

Movement
    ↓
AOI
    ↓
Combat

Output:

TaskGraph
Phase 4
Task Dispatch

Ready tasks are submitted to JobDispatch.

Runtime thread performs submission.

Worker threads remain execution-only.

Workers never generate tasks.

Workers never advance ticks.

Phase 5
Task Execution

Worker threads execute ready tasks.

Execution order among independent tasks is unspecified.

Dependency order is enforced by TaskGraph.

Example:

Movement_A

Movement_B

Movement_C

may execute concurrently.

Phase 6
Barrier Synchronization

Runtime waits for all tasks of the current synchronization domain.

Example:

Movement Tasks

↓

Barrier

↓

AOI Tasks

The next phase never starts before the previous phase completes.

Barrier is owned by Runtime.

Workers never synchronize directly.

Phase 7
Commit

All phase-local modifications become visible.

Responsibilities:

apply buffered updates
publish state transitions
finalize tick results

Commit must execute on runtime thread.

Commit order must be deterministic.

Phase 8
Tick Completion

Runtime finalizes tick execution.

Responsibilities:

advance frame counter
update runtime metrics
prepare next tick

Output:

Tick N Completed
Tick Invariants

The following invariants must always hold.

Invariant 1

Only one active tick exists.

Never:

Tick N

Tick N+1

running simultaneously
Invariant 2

Pipeline order is fixed.

Never:

Combat

↓

Movement

if configured otherwise.

Invariant 3

Task dependency order is explicit.

Never:

thread scheduling order

as dependency mechanism.

Invariant 4

Tick completion is atomic.

A tick is either:

completed

or

not completed

No partial completion state exists.

Failure Handling

Task failure must not corrupt runtime state.

Current V1 policy:

Task Failure

↓

Runtime Error

↓

Tick Abort

↓

Runtime Shutdown

Fail-fast.

Future V2 may support:

Phase Recovery
Future Evolution

V2

Phase Parallelism

V3

Cross Scene Tick Scheduling

V4

Distributed Tick Coordination
