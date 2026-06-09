
```
mmo_server
├─ CMakeLists.txt
├─ Structure.md
├─ client
├─ docs
│  ├─ Layer1
│  │  ├─ MemoryPool
│  │  │  ├─ architecture.json
│  │  │  ├─ behavior.md
│  │  │  ├─ internal_design.md
│  │  │  ├─ module.json
│  │  │  └─ state.json
│  │  ├─ architecture.md
│  │  ├─ index.json
│  │  ├─ lockfree_queue
│  │  │  ├─ architecture.json
│  │  │  ├─ behavior.md
│  │  │  ├─ internal_design.md
│  │  │  ├─ module.json
│  │  │  └─ state.json
│  │  ├─ packet
│  │  │  ├─ architecture.json
│  │  │  ├─ behavior.md
│  │  │  ├─ internal_design.md
│  │  │  ├─ module.json
│  │  │  └─ state.json
│  │  └─ timer
│  │     ├─ architecture.json
│  │     ├─ behavior.md
│  │     ├─ internal_design.md
│  │     ├─ module.json
│  │     └─ state.json
│  ├─ index.json
│  └─ 架构.md
├─ game_server
├─ server_core
│  ├─ CMakeLists.txt
│  ├─ Structure.md
│  ├─ infrastructure
│  │  ├─ CMakeLists.txt
│  │  ├─ lockfree_queue
│  │  │  ├─ CMakeLists.txt
│  │  │  ├─ include
│  │  │  │  └─ mmo
│  │  │  │     └─ infrastructure
│  │  │  │        └─ lockfree_queue
│  │  │  │           ├─ CacheAligned.hpp
│  │  │  │           ├─ MPSCQueue.hpp
│  │  │  │           ├─ QueueSlot.hpp
│  │  │  │           └─ QueueTypes.hpp
│  │  │  └─ src
│  │  ├─ logger
│  │  │  ├─ CMakeLists.txt
│  │  │  ├─ include
│  │  │  │  └─ mmo
│  │  │  │     └─ infrastructure
│  │  │  │        └─ logger
│  │  │  └─ src
│  │  ├─ memory_pool
│  │  │  ├─ CMakeLists.txt
│  │  │  ├─ include
│  │  │  │  └─ mmo
│  │  │  │     └─ infrastructure
│  │  │  │        └─ memory_pool
│  │  │  │           ├─ FixedBlockPool.hpp
│  │  │  │           ├─ IMemoryPool.hpp
│  │  │  │           ├─ MemoryBlock.hpp
│  │  │  │           └─ MemoryPoolTypes.hpp
│  │  │  └─ src
│  │  │     └─ FixedBlockPool.cpp
│  │  ├─ packet
│  │  │  ├─ CMakeLists.txt
│  │  │  ├─ include
│  │  │  │  └─ mmo
│  │  │  │     └─ infrastructure
│  │  │  │        └─ packet
│  │  │  │           ├─ PacketBuffer.hpp
│  │  │  │           ├─ PacketConstants.hpp
│  │  │  │           ├─ PacketFlags.hpp
│  │  │  │           ├─ PacketHeader.hpp
│  │  │  │           ├─ PacketReader.hpp
│  │  │  │           ├─ PacketTypes.hpp
│  │  │  │           ├─ PacketView.hpp
│  │  │  │           └─ PacketWriter.hpp
│  │  │  └─ src
│  │  │     ├─ PacketBuffer.cpp
│  │  │     ├─ PacketReader.cpp
│  │  │     ├─ PacketView.cpp
│  │  │     └─ PacketWriter.cpp
│  │  └─ timer
│  │     ├─ CMakeLists.txt
│  │     ├─ include
│  │     │  └─ mmo
│  │     │     └─ infrastructure
│  │     │        └─ timer
│  │     │           ├─ IClock.hpp
│  │     │           ├─ SteadyClock.hpp
│  │     │           ├─ TimerQueue.hpp
│  │     │           ├─ TimerTask.hpp
│  │     │           └─ TimerTypes.hpp
│  │     └─ src
│  │        ├─ SteadyClock.cpp
│  │        └─ TimerQueue.cpp
│  ├─ runtime
│  │  └─ CMakeLists.txt
│  └─ tests
│     ├─ CMakeLists.txt
│     ├─ infrastructure
│     │  ├─ lockfree_queue
│     │  │  ├─ CMakeLists.txt
│     │  │  ├─ ConsumerStarvationTests.cpp
│     │  │  ├─ FIFOOrderTests.cpp
│     │  │  ├─ FalseSharingPressureTests.cpp
│     │  │  ├─ FullQueueTests.cpp
│     │  │  ├─ LargePayloadTests.cpp
│     │  │  ├─ LongDurationStabilityTests.cpp
│     │  │  ├─ MultiProducerCorrectnessTests.cpp
│     │  │  ├─ ProducerBurstTests.cpp
│     │  │  ├─ StressTests.cpp
│     │  │  └─ WrapAroundTests.cpp
│     │  ├─ memory_pool
│     │  │  ├─ CMakeLists.txt
│     │  │  └─ FixedBlockPoolTests.cpp
│     │  └─ packet
│     │     ├─ CMakeLists.txt
│     │     ├─ PacketBoundaryTests.cpp
│     │     ├─ PacketBufferTests.cpp
│     │     ├─ PacketHeaderLayoutTests.cpp
│     │     ├─ PacketLargePayloadTests.cpp
│     │     ├─ PacketReaderTests.cpp
│     │     ├─ PacketRoundTripTests.cpp
│     │     ├─ PacketViewTests.cpp
│     │     └─ PacketWriterTests.cpp
│     └─ runtime
│        └─ timer
│           ├─ CMakeLists.txt
│           ├─ MultiProducerScheduleTests.cpp
│           ├─ TimerCancellationTests.cpp
│           ├─ TimerPrecisionTests.cpp
│           ├─ TimerQueueOrderTests.cpp
│           └─ TimerStressTests.cpp
└─ server_gateway

```