<!-- /home/xj/projects/mmo_server/docs/Layer1/lockfree_queue/behavior.md -->
Design Philosophy

# Design Philosophy

The queue is designed as the primary ingress transport
mechanism for MMO runtime message delivery.

The queue prioritizes:

- predictable ownership
- low contention
- stable runtime delivery

over strict global ordering guarantees.
Threading Semantics

# Threading Semantics

The queue supports:

- multiple concurrent producers
- exactly one consumer

The consumer thread exclusively owns dequeued messages.
Ordering Guarantees

# Ordering Guarantees

The queue guarantees FIFO ordering per producer thread.

Global ordering across producers is not guaranteed.
Memory Ownership

# Memory Ownership

Before enqueue:

- producer owns message

After successful enqueue:

- queue owns message

After dequeue:

- consumer owns message
Runtime Philosophy

# Runtime Philosophy

The queue is not responsible for simulation determinism.

Deterministic runtime ordering is established
at the runtime tick layer.

# Queue Contract

The queue guarantees:

- MPSC safety
- single-consumer ownership
- per-producer FIFO ordering

The queue does NOT guarantee:

- global producer ordering
- blocking semantics
- dynamic growth

# Memory Contract

Producer publish:

- release semantics

Consumer acquire:

- acquire semantics

The queue never exposes partially written elements.

# Invalid Usage

The same object instance must not be accessed
concurrently after ownership transfer.

# MPMCQueue Contract

合同1
Queue shall be lock-free.
合同2
Queue shall not allocate memory
after construction.
合同3
Queue shall support
multiple producers.
合同 4
Queue shall support
multiple consumers.
合同5
FIFO ordering is guaranteed.

注意：

这里是：

global FIFO

based on successful enqueue order

不是：

per-thread FIFO
合同6
Queue shall remain valid under
continuous wraparound.
合同7
Queue shall not contain mutex.
合同8
Queue shall not contain condition_variable.
合同9
Queue shall be TSAN clean.
合同10
Queue shall be bounded.
