# TickScheduler Interface Contract

## Design Goal

TickScheduler is the deterministic runtime driver of Layer2.

TickScheduler is responsible for:

* runtime lifecycle progression
* tick progression
* RuntimeContext update
* TimerQueue advancement
* RuntimePipeline invocation
* tick completion coordination

TickScheduler does not execute gameplay logic.

TickScheduler does not execute task graph logic.

TickScheduler does not manage worker threads.

TickScheduler is the single authoritative runtime clock.

---

# Ownership Model

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

Owned Objects:

* RuntimeContext

Referenced Objects:

* RuntimePipeline
* TimerQueue
* IClock

Not Owned:

* Scene
* Entity
* AOI
* Movement
* Combat
* Skill
* AI
* RPC
* JobDispatch
* WorkerPool

---

# Public Interface

## ITickScheduler

```cpp
class ITickScheduler
{
public:

    virtual ~ITickScheduler() = default;

    virtual bool Initialize() = 0;

    virtual void Shutdown() = 0;

    virtual void RunOneTick() = 0;

    virtual void RunUntilStopped() = 0;

    [[nodiscard]]
    virtual bool IsInitialized() const noexcept = 0;

    [[nodiscard]]
    virtual const RuntimeContext&
    GetRuntimeContext() const noexcept = 0;
};
```

---

# Lifecycle Contract

State Machine

Created

↓

Initialized

↓

Running

↓

Stopped

Initialization:

Created

↓

Initialize()

↓

Initialized

After initialization:

* RunOneTick() is allowed
* RunUntilStopped() is allowed

Shutdown:

Initialized/Running

↓

Shutdown()

↓

Stopped

After shutdown:

RunOneTick()

must fail fast.

---

# Tick Execution Contract

Single Tick Execution

Tick N

↓

Update RuntimeContext(Tick=N)

↓

Process TimerQueue

↓

Execute RuntimePipeline

↓

Commit Tick Completion

↓

TickId = N + 1

Execution order is fixed.

Execution order must never change between runs.

No retries.

No partial execution.

No reordering.

---

# RuntimeContext Contract

TickScheduler is the only writer.

Runtime Thread

↓

RuntimeContext

Worker Threads:

Read Only

Modification outside TickScheduler is forbidden.

RuntimeContext fields visible during Tick N:

* TickId
* DeltaTime
* RuntimeMode
* WorkerCount

All fields remain immutable during Tick execution.

---

# Timer Integration Contract

Before pipeline execution:

```cpp
timerQueue.Tick(now);
```

must execute exactly once.

Order:

Update RuntimeContext

↓

Process Timers

↓

Pipeline Execute

Reason:

timer-triggered work becomes visible within the same tick.

---

# RuntimePipeline Contract

TickScheduler invokes:

```cpp
pipeline.Execute(context);
```

exactly once per tick.

No retries.

No partial execution.

No reordering.

Pipeline failure is fatal.

---

# Determinism Rules

TickScheduler guarantees:

TickId:

0
1
2
3
...

strict monotonic progression.

DeltaTime:

constant during tick execution.

RuntimeMode:

immutable during tick execution.

Pipeline execution order:

identical between runs.

Phase order:

Input
→ Simulation
→ PostSimulation
→ Output

must remain fixed.

---

# Threading Rules

V1:

Single Runtime Thread

Allowed:

* Initialize()
* RunOneTick()
* RunUntilStopped()
* Shutdown()

from runtime thread only.

Forbidden:

* concurrent invocation
* multiple runtime threads
* external RuntimeContext mutation

---

# Failure Philosophy

Runtime is fail-fast.

Invalid state:

* double initialize
* run after shutdown
* null pipeline
* null clock
* null timer queue
* runtime context corruption

must terminate execution.

No recovery.

No silent fallback.

---

# Future Evolution Boundary

Allowed:

* adaptive tick pacing
* distributed scheduler
* cross-node synchronization
* fiber runtime
* multi-scene scheduler
* catch-up policy

Forbidden in V1:

* dynamic phase ordering
* variable execution order
* runtime graph mutation during tick
* multiple tick writers
