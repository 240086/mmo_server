# EventBus Behavior Contract

## Purpose

EventBus provides synchronous and deterministic event coordination inside a single process.

Its primary purpose is to decouple producers and consumers without introducing ownership transfer or transport semantics.

---

# Event Model

Every event type is a plain C++ structure.

Example:

```cpp
struct PlayerLoginEvent
{
    uint64_t playerId;
};
```

Events are immutable during dispatch.

Handlers must not modify event payload.

Subscription Contract

Subscribers register handlers for a specific event type.

Example:

Subscribe<PlayerLoginEvent>(handler);

A subscriber only receives events of the subscribed type.

No polymorphic event hierarchy is required.

Publish Contract

Publishing an event immediately invokes all subscribed handlers.

Example:

Publish(event);

Dispatch occurs synchronously on the caller thread.

No internal buffering exists.

No deferred execution exists.

Handler Invocation Order

V1 guarantees deterministic invocation order.

Handlers execute in the same order as registration.

Example:

Subscribe(A)
Subscribe(B)
Subscribe(C)

Publish()

Execution:

A
B
C
Threading Contract

EventBus itself provides no cross-thread synchronization.

Concurrent Subscribe(), Unsubscribe(), and Publish() operations are undefined behavior.

V1 assumes:

Configuration Phase
    ↓
Runtime Phase

Subscriptions are expected to be established during initialization.

Ownership Contract

EventBus never owns event objects.

Event lifetime remains owned by the caller.

Handlers receive events as:

const Event&

No event copies are performed.

Failure Contract

Exceptions escaping handlers are undefined behavior.

Event handlers must be noexcept or internally guarded.

V1 does not provide handler isolation.

Complexity Targets

Subscribe:
O(1)

Publish:
O(N)

Unsubscribe:
O(N)

Where N is the number of handlers for a given event type.

Explicit Non-Goals

V1 intentionally excludes:

Async dispatch
Event persistence
Event replay
Distributed events
Priority dispatch
Event filtering
Event batching
Event scheduling

These features belong to future layers.
