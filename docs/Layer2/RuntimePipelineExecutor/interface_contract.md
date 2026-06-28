# RuntimePipelineExecutor Interface Contract

Version:
    v1.0

Status:
    Frozen

Layer:
    Runtime Layer (Layer2)

Module:
    RuntimePipelineExecutor

---

# 1. Purpose

RuntimePipelineExecutor is responsible for executing a RuntimePipeline during one
runtime tick.

It converts a static RuntimePipeline topology into deterministic runtime execution.

The executor owns no gameplay logic.

It owns no scheduling policy.

It owns no thread management.

It coordinates the execution of runtime phases only.

---

# 2. Responsibilities

The executor SHALL:

- execute phases sequentially
- preserve RuntimePipeline ordering
- invoke JobDispatch for each phase
- update RuntimeContext execution state
- propagate execution failures
- guarantee deterministic execution order

The executor SHALL NOT:

- modify RuntimePipeline topology
- modify TaskGraph topology
- allocate worker threads
- own timers
- own runtime clock
- own gameplay state

---

# 3. Ownership

The executor owns:

- temporary execution state

The executor does NOT own:

- RuntimePipeline
- RuntimeContext
- JobDispatch
- RuntimePhase
- TaskGraph

All dependencies are externally injected.

---

# 4. Lifetime

Construction

↓

Initialize()

↓

ExecutePipeline()

↓

ExecutePipeline()

↓

...

↓

Shutdown()

↓

Destroy

Multiple ExecutePipeline() calls are expected.

Construction occurs once.

Shutdown occurs once.

---

# 5. Thread Model

Construction:

single-thread

Initialize():

single-thread

ExecutePipeline():

runtime thread only

Shutdown():

runtime thread only

Worker execution:

delegated to JobDispatch

No public interface is thread-safe unless explicitly documented.

---

# 6. Determinism Contract

The executor guarantees:

• stable phase ordering

• exactly-once phase execution

• deterministic traversal

• identical execution order for identical RuntimePipeline

The executor never performs:

- random scheduling
- priority scheduling
- work stealing
- speculative execution

---

# 7. RuntimeContext Contract

Before phase execution:

RuntimeContext.currentPhase
shall be updated.

After phase completion:

RuntimeContext.currentPhase
shall represent the completed phase
until the next phase begins.

Tick id shall never be modified.

Delta time shall never be modified.

Worker count shall never be modified.

---

# 8. RuntimePipeline Contract

Pipeline topology is immutable during execution.

The executor shall never:

insert phase

remove phase

reorder phase

duplicate phase

Pipeline validation occurs before execution begins.

Invalid pipelines must fail immediately.

---

# 9. RuntimePhase Contract

Each RuntimePhase shall execute once.

Execution order follows RuntimePipeline.

A phase cannot execute twice in one tick.

Phase execution is synchronous from the executor perspective.

---

# 10. JobDispatch Contract

RuntimePipelineExecutor delegates execution.

JobDispatch owns:

- task scheduling
- worker synchronization
- dependency waiting
- completion detection

RuntimePipelineExecutor only invokes:

Dispatch()

Wait()

(or equivalent interface)

The executor shall never manipulate worker threads directly.

---

# 11. Failure Contract

Initialization failure:

throw exception

Invalid pipeline:

throw exception

Invalid phase:

throw exception

Job dispatch failure:

propagate exception

The executor never silently ignores failures.

Fail-fast is mandatory.

---

# 12. Exception Safety

Initialize()

Strong Guarantee

ExecutePipeline()

Basic Guarantee

Shutdown()

No-throw preferred

Destructor

No-throw

---

# 13. Memory Contract

The executor performs no per-tick heap allocation.

Temporary execution state is stack allocated whenever possible.

Pipeline execution shall not depend on dynamic allocation.

---

# 14. Performance Contract

Per tick complexity

O(number of phases)

Per phase overhead

O(1)

Scheduling complexity

delegated to JobDispatch

Pipeline traversal shall be linear.

---

# 15. ABI Contract

RuntimePipelineExecutor is non-copyable.

RuntimePipelineExecutor is non-movable.

Public interface is ABI stable after V1 freeze.

No breaking interface changes are allowed.

---

# 16. Extension Points

Future versions may introduce:

parallel phase execution

phase profiling

execution tracing

phase filtering

coroutine execution

None of these extensions shall alter
the existing public interface.

---

# 17. Out of Scope

The executor does not provide:

gameplay logic

AI

combat

physics

scene update

timer execution

network processing

resource loading

These responsibilities belong to upper layers.

---

# 18. Testing Requirements

The implementation shall verify:

construction

initialization

shutdown

single pipeline execution

multi-phase execution

empty pipeline

failure propagation

deterministic ordering

runtime context updates

multiple tick execution

All tests must pass under:

GoogleTest

AddressSanitizer

UndefinedBehaviorSanitizer

ThreadSanitizer

Valgrind

before V1 freeze.
