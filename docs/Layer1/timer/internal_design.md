1. Internal Data Structures
# Internal Data Structures

TimerQueue internally uses:

- std::priority_queue<TimerTask>
- std::unordered_set<TimerId> cancelledTimers_
- atomic nextId_
- monotonic sequence counter

2. Heap Ordering
# Heap Ordering
Priority order:
    1. earliest deadline
    2. lowest sequence number

3. Lazy Cancellation Strategy
# Lazy Cancellation Strategy
Cancellation does not erase heap elements directly.
Cancelled timer IDs are inserted into a cancellation set.
Expired cancelled timers are discarded during dispatch.

4. Complexity Analysis
# Complexity Analysis
Schedule:O(log n)
Dispatch:O(k log n)
Cancellation:O(1)

5. Memory Ownership
# Memory Ownership
TimerQueue owns all TimerTask instances.
Callbacks are destroyed after execution or cancellation cleanup.