
---

# docs/Layer1/EventBus/internal_design.md

````md
# EventBus Internal Design

## Design Philosophy

Favor determinism and simplicity over feature richness.

The EventBus serves as a coordination primitive for Runtime and Simulation layers.

---

# Core Data Model

```text
Event Type
    ↓
EventId
    ↓
Handler List
```

EventId Mapping

Each event type maps to a unique EventId.

Example:

PlayerLoginEvent
    ↓
EventId = 1

EntityCreatedEvent
    ↓
EventId = 2

EventId generation is internal.

Consumers never manipulate raw ids.

Internal Structure
unordered_map<
    EventId,
    vector<Handler>
>

Example:

1
 ├─ HandlerA
 ├─ HandlerB

2
 ├─ HandlerC
Subscription Handle

Each subscription receives a unique handle.

Example:

SubscriptionHandle
{
    uint64_t id;
}

Used for:

Unsubscribe(handle);
Dispatch Algorithm

Publish(Event)

Resolve EventId
    ↓
Find Handler List
    ↓
Sequential Invocation

Pseudo:

for(handler : handlers)
{
    handler(event);
}

Complexity:

O(N)
Deterministic Ordering

Registration order equals execution order.

Implementation:

std::vector<Handler>

No sorting.

No priority queues.

No reordering.

Memory Characteristics

Subscription Phase:

May allocate.

Runtime Dispatch:

No allocations.

No copies.

No heap activity.

Future Evolution

V2:

MPSC Event Queue
Async Dispatch
Cross-thread Routing

V3:

Distributed Event Bus
Gateway Event Bridge
RPC Event Routing

The V1 data model remains compatible with future upgrades.
