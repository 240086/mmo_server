Logger Internal Design V1

1. Design Goals
High reliability
Deterministic behavior
Low complexity
Minimal allocation
Future async compatibility
2. Core Architecture
Caller
    │
    ▼
Logger
    │
    ▼
LogRecord
    │
    ▼
Formatter
    │
    ▼
Sink Dispatcher
    │
 ┌──┴─────┐
 ▼        ▼
Console   File
Sink      Sink
3. Core Data Structures
LogLevel
enum class LogLevel : uint8_t
{
    Trace,
    Debug,
    Info,
    Warn,
    Error,
    Fatal
};

空间复杂度：

1 byte
LogRecord
struct LogRecord
{
    uint64_t timestamp;

    uint32_t threadId;

    LogLevel level;

    std::string_view message;
};

特点：

Trivially movable
Small object
No ownership
4. Logger
class Logger
{
private:

    LogLevel minimumLevel_;

    std::vector<ILogSink*> sinks_;

    TextFormatter formatter_;
};

职责：

filter
record creation
dispatch
5. Formatter

V1：

TextFormatter

输出：

[Timestamp]
[Level]
[TID]
Message

例如：

2026-01-01 12:00:00
[INFO]
[TID=17]
Server Started
6. Sink Model

统一接口：

ILogSink

实现：

ConsoleSink
FileSink

设计模式：

Strategy Pattern
7. Complexity Analysis
Filter
O(1)
Record Creation
O(1)
Formatting
O(message_length)

理论最优。

Sink Dispatch
O(number_of_sinks)

通常：

1~4

视为常数。

Total
O(message_length)
8. Memory Model

V1 原则：

No hidden heap allocation in hot path

Formatter：

Fixed-size stack buffer

建议：

char buffer[1024];
9. Future Evolution Path

V1

Sync Logger

↓

V2

MPSC Queue
Async Logger

↓

V3

Distributed Trace
Structured Event
