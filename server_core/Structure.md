
```
server_core
├─ .clang-format
├─ .editorconfig
├─ CMakeLists.txt
├─ Structure.md
├─ infrastructure
│  ├─ CMakeLists.txt
│  ├─ logger
│  │  ├─ CMakeLists.txt
│  │  ├─ include
│  │  │  └─ mmo
│  │  │     └─ infrastructure
│  │  │        └─ logger
│  │  └─ src
│  └─ timer
│     ├─ CMakeLists.txt
│     ├─ include
│     │  └─ mmo
│     │     └─ infrastructure
│     │        └─ timer
│     │           ├─ IClock.hpp
│     │           ├─ SteadyClock.hpp
│     │           ├─ TimerQueue.hpp
│     │           ├─ TimerTask.hpp
│     │           └─ TimerTypes.hpp
│     └─ src
│        ├─ SteadyClock.cpp
│        └─ TimerQueue.cpp
├─ runtime
│  └─ CMakeLists.txt
└─ tests
   └─ runtime
      └─ timer
         ├─ TimerCancellationTests.cpp
         └─ TimerQueueOrderTests.cpp

```