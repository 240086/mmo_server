# RuntimePhase Interface Contract

# Design Goal

RuntimePhase defines a deterministic execution boundary inside the runtime tick.

RuntimePhase provides:

- phase identity
- phase ordering metadata
- phase execution contract

RuntimePhase does not provide:

- task scheduling
- thread management
- gameplay logic
- entity management

---

# Architectural Position

TickScheduler

    ↓

RuntimePipeline

    ↓

RuntimePhase

    ↓

TaskGraph

    ↓

JobDispatch

RuntimePhase acts as the execution boundary between pipeline stages.

---

# Public Types

## RuntimePhaseId

RuntimePhase identifiers are compile-time stable.

RuntimePhaseId must not depend on:

- strings
- hashes
- runtime registration

Example:

```cpp
enum class RuntimePhaseId : uint8_t
{
    Input = 0,

    Simulation,

    PostSimulation,

    Output
};

Rules:

Values must remain stable.

Values may only be appended.

Existing values must never be reordered.

RuntimePhase

RuntimePhase is a lightweight descriptor.

Example:

struct RuntimePhase
{
    RuntimePhaseId id;
};

RuntimePhase must remain:

trivially copyable
standard layout
POD-like
Ownership Model

RuntimePipeline owns all RuntimePhase instances.

RuntimePhase does not own:

tasks
jobs
systems

Ownership:

RuntimePipeline
↓
RuntimePhase

Registration Rules

Phase registration occurs only during runtime initialization.

Runtime execution must not register phases dynamically.

Forbidden:

Tick 100:

RegisterPhase(...)

Reason:

Would break determinism.

Execution Rules

RuntimePipeline executes phases sequentially.

Example:

Input

↓ Barrier

Simulation

↓ Barrier

PostSimulation

↓ Barrier

Output

A phase must complete before the next phase begins.

Synchronization Rules

Every RuntimePhase acts as a synchronization barrier.

Requirements:

All tasks submitted within a phase must finish before phase completion.

No task may survive into the next phase.

Determinism Rules

Given:

same input

same tick

same phase ordering

Execution order must remain identical.

RuntimePhase must never reorder itself.

Thread Safety Rules

Phase registration:

single-thread

Phase execution:

runtime-thread

Parallel work:

delegated to JobDispatch

RuntimePhase itself contains no synchronization primitives.

Forbidden:

std::mutex

std::shared_mutex

std::condition_variable

inside RuntimePhase.

Performance Requirements

Phase lookup:

O(1)

Phase iteration:

O(N)

Expected phase count:

4~16

Optimization goals:

cache friendly
branch predictable
no heap allocation
Failure Philosophy

Invalid phase ordering is fatal.

Duplicate phase registration is fatal.

Missing required phase is fatal.

Policy:

Fail Fast

Runtime shutdown is preferred over corrupted execution.

Future Evolution

V2:

Custom phases

V3:

Phase dependency graph

V4:

Distributed phase execution

Future evolution must preserve:

RuntimePhaseId stability
deterministic ordering
synchronization semantics
