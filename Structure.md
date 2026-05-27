
```
mmo_server
├─ CMakeLists.txt
├─ Structure.md
├─ client
├─ docs
│  ├─ Layer1
│  │  ├─ architecture.md
│  │  ├─ index.json
│  │  ├─ lockfree_queue
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
│  │  ├─ logger
│  │  │  ├─ CMakeLists.txt
│  │  │  ├─ include
│  │  │  │  └─ mmo
│  │  │  │     └─ infrastructure
│  │  │  │        └─ logger
│  │  │  └─ src
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