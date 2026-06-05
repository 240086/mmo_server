# MemoryPool Behavior Contract

## allocation_returns_valid_block

Allocate() 成功时：

返回有效 MemoryBlock

------------------------------------------------

## allocation_fails_when_pool_exhausted

池耗尽时：

返回 Invalid Block

不触发堆分配

------------------------------------------------

## deallocated_block_can_be_reused

释放后：

块可再次被 Allocate()

------------------------------------------------

## double_free_is_rejected

同一块内存：

禁止重复释放

------------------------------------------------

## pool_never_grows_implicitly

池容量固定

禁止运行期自动扩容

------------------------------------------------

## allocation_is_O1

Allocate()

时间复杂度：

O(1)

------------------------------------------------

## deallocation_is_O1

Deallocate()

时间复杂度：

O(1)

------------------------------------------------

## ownership_transfer_is_explicit

Allocate

Pool → Caller

Deallocate

Caller → Pool

1. capacity == blockSize

2. no fallback malloc

3. double free behavior

4. Worst Case O(1)
