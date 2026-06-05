MemoryPool V1 Internal Design

Storage Layout

Pool Memory Region
│
├── Block 0
├── Block 1
├── Block 2
├── ...
└── Block N

Free List

head
 ↓
block7
 ↓
block3
 ↓
block12

Allocate

pop head

Deallocate

push head

Complexity

Allocate O(1)

Deallocate O(1)

Memory Fragmentation

External Fragmentation = 0

Internal Fragmentation

Controlled by BlockSize

Thread Model

V1

Single Thread

V2

MPSC

V3

Thread Local Pool

建议补充：

1. 保留 IMemoryPool
2. 强化 MemoryBlock 元数据
3. FixedBlockPool 参数化(blockSize, blockCount)
4. 增加 V2 PoolRegistry 演进路径

Intrusive Free List

PoolStatistics

PoolRegistry(V2)
