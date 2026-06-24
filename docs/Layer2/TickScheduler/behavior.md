# TickScheduler Behavior Contract

## Design Goal

TickScheduler is the root execution driver
for the MMO runtime.

TickScheduler is responsible for:

- fixed-step progression
- runtime context update
- runtime pipeline dispatch

TickScheduler must never execute gameplay logic.

------------------------------------------------

# Tick Model

Fixed timestep:

Tick N

↓

Tick N+1

↓

Tick N+2

Tick progression must remain monotonic.

Tick rollback is forbidden.

------------------------------------------------

# Runtime Flow

Initialize

↓

Running

↓

Stopping

↓

Stopped

------------------------------------------------

# Tick Execution Flow

Update RuntimeContext

↓

Dispatch RuntimePipeline

↓

Wait Completion

↓

Advance Tick

------------------------------------------------

# Tick Determinism

TickId increases by exactly one.

FrameIndex increases by exactly one.

DeltaTime remains constant.

------------------------------------------------

# Tick Barrier

A tick cannot finish until:

RuntimePipeline completes.

No phase overlap is allowed.

------------------------------------------------

# Failure Philosophy

Runtime execution failure is fatal.

Policy:

Fail Fast

Invalid tick progression
must terminate runtime execution.
