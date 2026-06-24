# RuntimeContext Behavior Contract
Design Goal

RuntimeContext provides deterministic runtime metadata for all runtime modules.

RuntimeContext contains execution state only.

RuntimeContext must never contain gameplay state.

Ownership Model

RuntimeContext is owned by Runtime Thread.

Runtime Thread
        ↓
RuntimeContext

Worker threads may only observe runtime metadata.

Worker threads must never mutate RuntimeContext.

Runtime Metadata

RuntimeContext stores:

TickId

FrameIndex

DeltaTime

RuntimeMode

WorkerCount

RuntimeContext does not store:

Scene

Entity

Player

AOI

Combat

Network Connection
Tick Visibility

At tick start:

Runtime Thread

↓

Update RuntimeContext

↓

Publish Context

↓

Dispatch Tasks

Worker threads observe a stable snapshot.

Context must remain logically immutable during task execution.

Runtime Lifecycle

Created

↓

Runtime Initialize

↓

Tick Execution

↓

Runtime Shutdown

↓

Destroyed

Thread Safety Rules

Single Writer

Runtime Thread

Multiple Readers

Worker Threads

Synchronization must occur at tick boundaries.

RuntimeContext must not require locks during task execution.

Determinism Rules

TickId must increase monotonically.

FrameIndex must increase monotonically.

DeltaTime must remain fixed within a tick.

RuntimeMode must not change during a tick.

Failure Philosophy

RuntimeContext corruption is fatal.

Policy:

Fail Fast

Invalid runtime metadata must terminate runtime execution.
