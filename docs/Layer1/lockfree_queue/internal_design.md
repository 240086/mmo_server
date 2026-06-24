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

5. MPMC Queue Backend
Bounded Ring Buffer

Capacity:
power-of-two

Storage:
std::array<QueueSlot<T>, Capacity>

Producer:
CAS tail

Consumer:
CAS head


状态：

```text
free
reserved by producer
ready
reserved by consumer
free

内存模型

制片人：

Acquire
CAS
Release

消费者：

Acquire
CAS
Release

禁止：

seq_cst

避免全局栅栏。

防止虚假共享

独立缓存行：

head
tail

布局：

alignas(64)
atomic<uint64_t> head;

alignas(64)
atomic<uint64_t> tail;
容量政策

固定容量：

template<
    typename T,
    std::size_t Capacity>

要求：

Capacity >= 2

且：

power-of-two
溢出策略

满队列：

Enqueue()
return false;

不阻塞。

不等待。

不扩容。

空置政策

空队列：

Dequeue()
return false;