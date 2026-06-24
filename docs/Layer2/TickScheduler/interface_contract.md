docs/Layer2/TickScheduler/interface_contract.md
Design Goal

TickScheduler is the deterministic runtime driver.

Responsibilities:

runtime lifecycle control
tick progression
RuntimeContext update
RuntimePipeline invocation
timer integration
tick synchronization

TickScheduler owns runtime execution.

TickScheduler does not execute gameplay logic.

TickScheduler does not execute task graph logic.

TickScheduler does not manage worker threads.

Ownership Model
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

RuntimeContext
RuntimePipeline reference
TimerQueue reference
IClock reference

Not Owned:

Scene
Entity
AOI
Combat
Skill
AI
RPC
Public Interface
ITickScheduler
class ITickScheduler
{
public:

    virtual ~ITickScheduler() = default;

    virtual bool Initialize() = 0;

    virtual void Shutdown() = 0;

    virtual void RunOneTick() = 0;

    virtual void RunUntilStopped() = 0;

    [[nodiscard]]
    virtual bool IsRunning() const noexcept = 0;

    [[nodiscard]]
    virtual const RuntimeContext&
    GetRuntimeContext() const noexcept = 0;
};
Lifecycle Contract

Initialization:

Constructed

↓

Initialize()

↓

Running

Shutdown:

Running

↓

Shutdown()

↓

Stopped

After shutdown:

RunOneTick()

must fail fast.

Tick Execution Contract

Single Tick:

Update RuntimeContext

↓

Process TimerQueue

↓

Execute RuntimePipeline

↓

Frame Complete

↓

TickId++

Execution order is fixed.

Must never change between runs.

RuntimeContext Contract

TickScheduler is the only writer.

Runtime Thread

↓

RuntimeContext

Worker threads:

Read Only

Modification outside TickScheduler is forbidden.

Timer Integration Contract

Before pipeline execution:

TimerQueue.Tick(now)

must execute.

Order:

Update Context

↓

Process Timers

↓

Pipeline Execute

Reason:

timer-triggered tasks become visible within same tick.

RuntimePipeline Contract

TickScheduler invokes:

pipeline.Execute(context);

Exactly once per tick.

No retries.

No reordering.

No partial execution.

Pipeline failure is fatal.

Determinism Rules

TickScheduler must guarantee:

TickId:

0
1
2
3
...

strict monotonic progression.

FrameIndex:

0
1
2
3
...

strict monotonic progression.

DeltaTime:

fixed during tick.

RuntimeMode:

immutable during tick execution.

Execution order:

identical between runs.

Threading Rules

V1:

Runtime Thread

only.

Allowed:

Initialize()
RunOneTick()
RunUntilStopped()
Shutdown()

from runtime thread.

Forbidden:

multiple concurrent callers.

Failure Philosophy

Runtime is fail-fast.

Invalid state:

double initialize

run after shutdown

null pipeline

null clock

context corruption

must terminate execution.

No recovery.

No silent fallback.

Future Evolution Boundary

Allowed:

adaptive tick pacing

distributed scheduler

cross-node synchronization

fiber runtime

multi-scene scheduler

Forbidden in V1:

dynamic phase ordering

variable execution order

runtime graph mutation during tick
