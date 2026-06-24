# Internal Design
Core Philosophy

RuntimeContext is a lightweight immutable metadata container.

RuntimeContext is not a service container.

RuntimeContext is not a dependency injection container.

RuntimeContext is not a gameplay context.

Storage Model

V1:

RuntimeContext
{
    TickId

    FrameIndex

    DeltaTime

    RuntimeMode

    WorkerCount
}

No dynamic allocation.

No ownership graph.

No virtual dispatch.

Data Layout

Recommended layout:

struct RuntimeContext
{
    TickId tickId;

    uint64_t frameIndex;

    std::chrono::milliseconds deltaTime;

    RuntimeMode mode;

    uint32_t workerCount;
};
Complexity

Read:

O(1)

Write:

O(1)

Memory:

O(1)
Cache Behavior

Single cache line target.

Avoid:

std::string

std::vector

std::unordered_map

inside RuntimeContext.

Synchronization Strategy

V1:

Single Writer

Multiple Readers

Synchronization point:

Tick Boundary

No runtime locking.

No mutex.

No condition_variable.

Future Evolution

V2:

RuntimeMetricsSnapshot

V3:

DistributedRuntimeContext

V4:

CrossNodeTickMetadata
