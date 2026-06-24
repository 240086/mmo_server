
```
mmo_server
├─ CMakeLists.txt
├─ Structure.md
├─ client
├─ docs
│  ├─ Layer1
│  │  ├─ ConfigSystem
│  │  │  ├─ architecture.json
│  │  │  ├─ behavior.md
│  │  │  ├─ freeze_report.md
│  │  │  ├─ internal_design.md
│  │  │  ├─ module.json
│  │  │  └─ state.json
│  │  ├─ EventBus
│  │  │  ├─ architecture.json
│  │  │  ├─ behavior.md
│  │  │  ├─ internal_design.md
│  │  │  ├─ module.json
│  │  │  └─ state.json
│  │  ├─ Logger
│  │  │  ├─ architecture.json
│  │  │  ├─ behavior.md
│  │  │  ├─ internal_design.md
│  │  │  ├─ module.json
│  │  │  └─ state.json
│  │  ├─ MemoryPool
│  │  │  ├─ architecture.json
│  │  │  ├─ behavior.md
│  │  │  ├─ internal_design.md
│  │  │  ├─ module.json
│  │  │  └─ state.json
│  │  ├─ Net
│  │  │  ├─ architecture.json
│  │  │  ├─ behavior.md
│  │  │  ├─ freeze_report.md
│  │  │  ├─ internal_design.md
│  │  │  ├─ module.json
│  │  │  └─ state.json
│  │  ├─ Serialization
│  │  │  ├─ architecture.json
│  │  │  ├─ behavior.md
│  │  │  ├─ internal_design.md
│  │  │  ├─ module.json
│  │  │  └─ state.json
│  │  ├─ architecture.md
│  │  ├─ index.json
│  │  ├─ lockfree_queue
│  │  │  ├─ architecture.json
│  │  │  ├─ architecture_addendum_mpmc.md
│  │  │  ├─ behavior.md
│  │  │  ├─ interface_contract.md
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
│  ├─ Layer2
│  │  ├─ JobDispatch
│  │  │  ├─ JobDispatch_CPP_Design.md
│  │  │  ├─ architecture.json
│  │  │  ├─ behavior.md
│  │  │  ├─ interface_contract.md
│  │  │  ├─ internal_design.md
│  │  │  ├─ module.json
│  │  │  └─ state.json
│  │  ├─ RuntimeContext
│  │  │  ├─ architecture.json
│  │  │  ├─ behavior.md
│  │  │  ├─ freeze_report.md
│  │  │  ├─ interface_contract.md
│  │  │  ├─ internal_design.md
│  │  │  ├─ module.json
│  │  │  └─ state.json
│  │  ├─ RuntimePhase
│  │  │  ├─ architecture.json
│  │  │  ├─ behavior.md
│  │  │  ├─ freeze_report.md
│  │  │  ├─ interface_contract.md
│  │  │  ├─ internal_design.md
│  │  │  ├─ module.json
│  │  │  └─ state.json
│  │  ├─ RuntimePipeline
│  │  │  ├─ architecture.json
│  │  │  ├─ behavior.md
│  │  │  ├─ interface_contract.md
│  │  │  ├─ internal_design.md
│  │  │  └─ state.json
│  │  ├─ TaskGraph
│  │  │  ├─ architecture.json
│  │  │  ├─ behavior.md
│  │  │  ├─ interface_contract.md
│  │  │  ├─ internal_design.md
│  │  │  ├─ module.json
│  │  │  └─ state.json
│  │  ├─ TickScheduler
│  │  │  ├─ architecture.json
│  │  │  ├─ behavior.md
│  │  │  ├─ freeze_report.md
│  │  │  ├─ interface_contract.md
│  │  │  ├─ internal_design.md
│  │  │  ├─ module.json
│  │  │  └─ state.json
│  │  ├─ architecture.json
│  │  ├─ index.json
│  │  ├─ module_dependency.md
│  │  ├─ runtime_thread_model.md
│  │  ├─ tick_lifecycle.md
│  │  └─ 路线.md
│  ├─ index.json
│  └─ 架构.md
├─ game_server
├─ server_core
│  ├─ CMakeLists.txt
│  ├─ Structure.md
│  ├─ infrastructure
│  │  ├─ CMakeLists.txt
│  │  ├─ ConfigSystem
│  │  │  ├─ CMakeLists.txt
│  │  │  ├─ include
│  │  │  │  └─ mmo
│  │  │  │     └─ infrastructure
│  │  │  │        └─ ConfigSystem
│  │  │  │           ├─ ConfigSystem.hpp
│  │  │  │           ├─ ConfigTypes.hpp
│  │  │  │           ├─ ConfigValue.hpp
│  │  │  │           ├─ IConfigSystem.hpp
│  │  │  │           ├─ parser
│  │  │  │           │  ├─ IConfigParser.hpp
│  │  │  │           │  ├─ KeyValueConfigParser.hpp
│  │  │  │           │  └─ ParseResult.hpp
│  │  │  │           └─ source
│  │  │  │              ├─ FileConfigSource.hpp
│  │  │  │              └─ IConfigSource.hpp
│  │  │  └─ src
│  │  │     ├─ ConfigSystem.cpp
│  │  │     ├─ parser
│  │  │     │  └─ KeyValueConfigParser.cpp
│  │  │     └─ source
│  │  │        └─ FileConfigSource.cpp
│  │  ├─ EventBus
│  │  │  ├─ CMakeLists.txt
│  │  │  ├─ include
│  │  │  │  └─ mmo
│  │  │  │     └─ infrastructure
│  │  │  │        └─ EventBus
│  │  │  │           ├─ EventBus.hpp
│  │  │  │           ├─ EventBusTypes.hpp
│  │  │  │           ├─ IEventBus.hpp
│  │  │  │           └─ SubscriptionToken.hpp
│  │  │  └─ src
│  │  │     └─ EventBus.cpp
│  │  ├─ Net
│  │  │  ├─ CMakeLists.txt
│  │  │  ├─ include
│  │  │  │  └─ mmo
│  │  │  │     └─ infrastructure
│  │  │  │        └─ Net
│  │  │  │           ├─ ConnectionId.hpp
│  │  │  │           ├─ IConnection.hpp
│  │  │  │           ├─ INetServer.hpp
│  │  │  │           ├─ NetServer.hpp
│  │  │  │           ├─ NetTypes.hpp
│  │  │  │           ├─ PacketCodec.hpp
│  │  │  │           ├─ PacketFrame.hpp
│  │  │  │           └─ TcpConnection.hpp
│  │  │  └─ src
│  │  │     ├─ NetServer.cpp
│  │  │     ├─ PacketCodec.cpp
│  │  │     └─ TcpConnection.cpp
│  │  ├─ lockfree_queue
│  │  │  ├─ CMakeLists.txt
│  │  │  ├─ include
│  │  │  │  └─ mmo
│  │  │  │     └─ infrastructure
│  │  │  │        └─ lockfree_queue
│  │  │  │           ├─ CacheAligned.hpp
│  │  │  │           ├─ MPMCQueue.hpp
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
│  │  │  │           ├─ ConsoleSink.hpp
│  │  │  │           ├─ FileSink.hpp
│  │  │  │           ├─ IFormatter.hpp
│  │  │  │           ├─ ILogSink.hpp
│  │  │  │           ├─ LogLevel.hpp
│  │  │  │           ├─ LogRecord.hpp
│  │  │  │           ├─ Logger.hpp
│  │  │  │           ├─ LoggerConstants.hpp
│  │  │  │           ├─ LoggerTypes.hpp
│  │  │  │           └─ TextFormatter.hpp
│  │  │  └─ src
│  │  │     ├─ ConsoleSink.cpp
│  │  │     ├─ FileSink.cpp
│  │  │     ├─ Logger.cpp
│  │  │     └─ TextFormatter.cpp
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
│  │  ├─ serialization
│  │  │  ├─ CMakeLists.txt
│  │  │  ├─ include
│  │  │  │  └─ mmo
│  │  │  │     └─ infrastructure
│  │  │  │        └─ serialization
│  │  │  │           ├─ BinaryReader.hpp
│  │  │  │           ├─ BinaryWriter.hpp
│  │  │  │           ├─ PrimitiveSerializer.hpp
│  │  │  │           ├─ SerializationConstants.hpp
│  │  │  │           ├─ SerializationError.hpp
│  │  │  │           ├─ SerializationTraits.hpp
│  │  │  │           ├─ SerializationTypes.hpp
│  │  │  │           └─ StringSerializer.hpp
│  │  │  └─ src
│  │  │     ├─ BinaryReader.cpp
│  │  │     ├─ BinaryWriter.cpp
│  │  │     └─ StringSerializer.cpp
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
│  │  ├─ CMakeLists.txt
│  │  ├─ JobDispatch
│  │  │  ├─ CMakeLists.txt
│  │  │  ├─ include
│  │  │  │  └─ mmo
│  │  │  │     └─ runtime
│  │  │  │        └─ JobDispatch
│  │  │  │           ├─ IJobDispatch.hpp
│  │  │  │           ├─ ITaskCompletionSink.hpp
│  │  │  │           ├─ ITaskExecutor.hpp
│  │  │  │           ├─ JobDispatch.hpp
│  │  │  │           ├─ JobDispatchConstants.hpp
│  │  │  │           ├─ JobDispatchTypes.hpp
│  │  │  │           ├─ TaskFunction.hpp
│  │  │  │           ├─ TaskRegistry.hpp
│  │  │  │           ├─ TaskRuntimeState.hpp
│  │  │  │           ├─ WorkerContext.hpp
│  │  │  │           ├─ WorkerPool.hpp
│  │  │  │           ├─ WorkerPoolConfig.hpp
│  │  │  │           └─ WorkerThread.hpp
│  │  │  └─ src
│  │  ├─ RuntimeContext
│  │  │  ├─ CMakeLists.txt
│  │  │  ├─ include
│  │  │  │  └─ mmo
│  │  │  │     └─ runtime
│  │  │  │        └─ RuntimeContext
│  │  │  │           ├─ RuntimeContext.hpp
│  │  │  │           ├─ RuntimeMode.hpp
│  │  │  │           └─ RuntimeTypes.hpp
│  │  │  └─ src
│  │  ├─ RuntimePhase
│  │  │  ├─ CMakeLists.txt
│  │  │  ├─ include
│  │  │  │  └─ mmo
│  │  │  │     └─ runtime
│  │  │  │        └─ RuntimePhase
│  │  │  │           ├─ RuntimePhase.hpp
│  │  │  │           ├─ RuntimePhaseConstants.hpp
│  │  │  │           ├─ RuntimePhaseId.hpp
│  │  │  │           └─ RuntimePhaseTypes.hpp
│  │  │  └─ src
│  │  ├─ RuntimePipeline
│  │  │  ├─ CMakeLists.txt
│  │  │  ├─ include
│  │  │  │  └─ mmo
│  │  │  │     └─ runtime
│  │  │  │        └─ RuntimePipeline
│  │  │  │           └─ RuntimePipeline.hpp
│  │  │  └─ src
│  │  └─ TaskGraph
│  │     ├─ CMakeLists.txt
│  │     ├─ include
│  │     │  └─ mmo
│  │     │     └─ runtime
│  │     │        └─ TaskGraph
│  │     │           ├─ TaskDependency.hpp
│  │     │           ├─ TaskGraph.hpp
│  │     │           ├─ TaskGraphConstants.hpp
│  │     │           ├─ TaskGraphTypes.hpp
│  │     │           └─ TaskNode.hpp
│  │     └─ src
│  └─ tests
│     ├─ CMakeLists.txt
│     ├─ infrastructure
│     │  ├─ ConfigSystem
│     │  │  ├─ CMakeLists.txt
│     │  │  ├─ ConfigAccessTests.cpp
│     │  │  ├─ ConfigConcurrencyTests.cpp
│     │  │  ├─ ConfigFailureTests.cpp
│     │  │  ├─ ConfigLoadTests.cpp
│     │  │  └─ ConfigParserTests.cpp
│     │  ├─ EventBus
│     │  │  ├─ CMakeLists.txt
│     │  │  ├─ EventBusBasicTests.cpp
│     │  │  ├─ EventBusConcurrencyTests.cpp
│     │  │  ├─ EventBusLongDurationTests.cpp
│     │  │  ├─ EventBusReentrancyTests.cpp
│     │  │  └─ EventBusSnapshotTests.cpp
│     │  ├─ Net
│     │  │  ├─ CMakeLists.txt
│     │  │  ├─ MPSCQueueReceivedPacketStressTests.cpp
│     │  │  ├─ NetServerAcceptTests.cpp
│     │  │  ├─ NetServerConnectionTests.cpp
│     │  │  ├─ NetServerLifecycleTests.cpp
│     │  │  ├─ NetServerLockFreeQueueTests.cpp
│     │  │  ├─ NetServerReceiveStressTests.cpp
│     │  │  ├─ NetServerReceiveTests.cpp
│     │  │  ├─ NetTestHelper.hpp
│     │  │  ├─ PacketCodecTests.cpp
│     │  │  └─ TcpConnectionTests.cpp
│     │  ├─ lockfree_queue
│     │  │  ├─ CMakeLists.txt
│     │  │  ├─ MPMC
│     │  │  │  ├─ MPMCQueueAbiTests.cpp
│     │  │  │  ├─ MPMCQueueBasicTests.cpp
│     │  │  │  ├─ MPMCQueueCapacityTests.cpp
│     │  │  │  ├─ MPMCQueueContentionTests.cpp
│     │  │  │  ├─ MPMCQueueLargePayloadTests.cpp
│     │  │  │  ├─ MPMCQueueMultiConsumerTests.cpp
│     │  │  │  ├─ MPMCQueueMultiProducerTests.cpp
│     │  │  │  ├─ MPMCQueueStressTests.cpp
│     │  │  │  └─ MPMCQueueWrapAroundTests.cpp
│     │  │  └─ MPSC
│     │  │     ├─ ConsumerStarvationTests.cpp
│     │  │     ├─ FIFOOrderTests.cpp
│     │  │     ├─ FalseSharingPressureTests.cpp
│     │  │     ├─ FullQueueTests.cpp
│     │  │     ├─ LargePayloadTests.cpp
│     │  │     ├─ LongDurationStabilityTests.cpp
│     │  │     ├─ MultiProducerCorrectnessTests.cpp
│     │  │     ├─ ProducerBurstTests.cpp
│     │  │     ├─ StressTests.cpp
│     │  │     └─ WrapAroundTests.cpp
│     │  ├─ logger
│     │  │  ├─ CMakeLists.txt
│     │  │  ├─ LoggerInfrastructureTests.cpp
│     │  │  └─ LongDurationStressTests.cpp
│     │  ├─ memory_pool
│     │  │  ├─ CMakeLists.txt
│     │  │  ├─ FixedBlockPoolTests.cpp
│     │  │  ├─ MemoryPoolBoundaryTests.cpp
│     │  │  └─ MemoryPoolStressTests.cpp
│     │  ├─ packet
│     │  │  ├─ CMakeLists.txt
│     │  │  ├─ PacketBoundaryTests.cpp
│     │  │  ├─ PacketBufferTests.cpp
│     │  │  ├─ PacketHeaderLayoutTests.cpp
│     │  │  ├─ PacketLargePayloadTests.cpp
│     │  │  ├─ PacketReaderTests.cpp
│     │  │  ├─ PacketRoundTripTests.cpp
│     │  │  ├─ PacketViewTests.cpp
│     │  │  └─ PacketWriterTests.cpp
│     │  ├─ serialization
│     │  │  ├─ BinaryReaderTests.cpp
│     │  │  ├─ BinaryWriterTests.cpp
│     │  │  ├─ CMakeLists.txt
│     │  │  ├─ PrimitiveSerializationTests.cpp
│     │  │  ├─ SerializationBoundaryTests.cpp
│     │  │  ├─ SerializationRoundTripTests.cpp
│     │  │  ├─ StringSerializationTests.cpp
│     │  │  └─ TestHelpers.hpp
│     │  └─ timer
│     │     ├─ CMakeLists.txt
│     │     ├─ MultiProducerScheduleTests.cpp
│     │     ├─ TimerCancellationTests.cpp
│     │     ├─ TimerPrecisionTests.cpp
│     │     ├─ TimerQueueOrderTests.cpp
│     │     └─ TimerStressTests.cpp
│     └─ runtime
│        ├─ JobDispatch
│        ├─ RuntimeContext
│        │  ├─ CMakeLists.txt
│        │  ├─ RuntimeContextConstructionTests.cpp
│        │  ├─ RuntimeContextLayoutTests.cpp
│        │  ├─ RuntimeModeTests.cpp
│        │  └─ RuntimeTypeTests.cpp
│        ├─ RuntimePhase
│        │  ├─ CMakeLists.txt
│        │  ├─ RuntimePhaseConstructionTests.cpp
│        │  ├─ RuntimePhaseIdTests.cpp
│        │  ├─ RuntimePhaseLayoutTests.cpp
│        │  └─ RuntimePhaseTypesTests.cpp
│        ├─ RuntimePipeline
│        │  ├─ CMakeLists.txt
│        │  ├─ RuntimePipelineConstantsTests.cpp
│        │  ├─ RuntimePipelineConstructionTests.cpp
│        │  ├─ RuntimePipelineInitializationTests.cpp
│        │  └─ RuntimePipelineLayoutTests.cpp
│        └─ TaskGraph
│           ├─ CMakeLists.txt
│           └─ TaskGraphAbiTests.cpp
└─ server_gateway

```