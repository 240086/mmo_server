1. Design Philosophy
# Design Philosophy

The timer system is designed for deterministic MMO runtime execution.

The primary objective is runtime stability and predictable execution order,
not maximum throughput.

Timer callbacks are always executed by the runtime-owned thread.
The timer system never creates internal worker threads.
2. Clock Semantics
# Clock Semantics

The timer module exclusively uses monotonic clocks.

System wall-clock time must never affect timer execution behavior.

Clock source:
- std::chrono::steady_clock
3. Scheduling Rules
# Scheduling Rules

Timers are scheduled using absolute deadlines.

Timers with identical deadlines must preserve insertion order (FIFO).

Scheduling operations may be called concurrently from multiple producer threads.
4. Dispatch Rules
# Dispatch Rules

Expired timers are dispatched exclusively by the runtime-owned thread.

Dispatch execution is single-threaded.

Timer callbacks are forbidden from executing concurrently.
5. Cancellation Semantics
# Cancellation Semantics

The timer queue uses lazy cancellation.

Cancelled timers may remain inside the internal heap
until popped during dispatch.

Cancelled timers must never execute callbacks.
6. Determinism Guarantees
# Determinism Guarantees

The timer system guarantees:

- Stable execution ordering
- Same-deadline FIFO ordering
- Single-thread callback execution
- Monotonic time progression
7. Failure Philosophy
# Failure Philosophy

Under heavy runtime load, slight timer drift is preferred
over burst catch-up execution.

The timer system prioritizes simulation stability
over strict real-time precision.
8. Future Evolution
# Future Evolution

Current implementation:
- Min-heap priority queue

Future possible evolution:
- Hierarchical timing wheel
- Intrusive timer nodes
- Coroutine-aware timers
- Lock-free scheduling queues