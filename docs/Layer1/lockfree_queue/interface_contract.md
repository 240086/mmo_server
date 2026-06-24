公共 API
template<
    typename T,
    std::size_t Capacity>
class MPMCQueue final
{
public:

    MPMCQueue();

    bool Enqueue(const T& value);

    bool Enqueue(T&& value);

    bool Dequeue(T& value);

    bool Empty() const noexcept;

    std::size_t ApproxSize() const noexcept;

    constexpr std::size_t CapacityValue() const noexcept;
};
非功能性需求
无堆分配
No allocation after construction.
锁免费
No mutex.
No spin lock.
缓存友好
head isolated

tail isolated
确定性
Bounded latency.

No blocking path.
