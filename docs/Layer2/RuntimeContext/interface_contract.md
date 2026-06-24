# RuntimeContext Interface Contract

## Design Goal

RuntimeContext defines the immutable runtime metadata contract shared by all Layer2 runtime modules.

The interface must remain:

* deterministic
* lightweight
* POD-friendly
* cache-friendly
* dependency-free

RuntimeContext is a data contract.

RuntimeContext is not a service provider.

RuntimeContext is not a dependency injection container.

---

# Public Types

## TickId

Represents globally monotonic runtime tick progression.

Requirements:

* unsigned
* monotonic increasing
* never reused

Type:

```cpp
using TickId = std::uint64_t;
```

---

## FrameIndex

Represents runtime frame sequence.

Requirements:

* unsigned
* monotonic increasing
* never reused

Type:

```cpp
using FrameIndex = std::uint64_t;
```

---

## TickDurationMs

Represents fixed simulation step.

Requirements:

* positive
* immutable during tick execution

Type:

```cpp
using TickDurationMs = std::uint32_t;
```

---

## WorkerCount

Represents active runtime worker thread count.

Requirements:

* non-zero
* immutable during tick execution

Type:

```cpp
using WorkerCount = std::uint32_t;
```

---

# RuntimeMode

Represents runtime lifecycle state.

Valid states:

```text
Initializing
Running
Stopping
Stopped
```

State transitions:

```text
Initializing
      ↓
Running
      ↓
Stopping
      ↓
Stopped
```

Illegal transitions are forbidden.

---

# RuntimeContext

Runtime execution metadata snapshot.

Properties:

* trivially copyable
* standard layout
* noexcept constructible

Contains:

```cpp
TickId
FrameIndex
TickDurationMs
RuntimeMode
WorkerCount
```

Must not contain:

```cpp
std::string
std::vector
std::unordered_map
virtual function
ownership pointer
```

---

# Thread Safety

Writer:

```text
Runtime Thread
```

Readers:

```text
Worker Threads
```

Synchronization:

```text
Tick Boundary
```

No internal synchronization.

No mutex.

No atomic members.

---

# ABI Stability Rules

V1 guarantees:

* fixed field order
* standard layout
* stable public aliases

Future versions may append fields.

Existing fields must not change semantic meaning.

---

# Future Evolution

V2:

RuntimeMetricsSnapshot

V3:

DistributedRuntimeMetadata

V4:

CrossNodeRuntimeContext
