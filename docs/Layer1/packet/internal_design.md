<!-- /home/xj/projects/mmo_server/docs/Layer1/lockfree_queue/internal_design.md -->
1. Queue Backend
# Queue Backend

Current implementation:
- bounded MPSC ring buffer

Reason:
- cache locality
- predictable memory layout
- no heap allocation during enqueue
2. False Sharing Prevention
# False Sharing Prevention

Producer and consumer indices are cache-line separated.

Alignment target:
- 64-byte cache line
3. Atomic Semantics
# Atomic Semantics

Producer writes:
- release semantics

Consumer reads:
- acquire semantics
4. Overflow Policy
# Overflow Policy

The queue is bounded.

When full:
- enqueue fails immediately

The queue never blocks producer threads.