Logger V1 Behavior Contract
BC-001 Deterministic Submission
Logger::Log()

必须产生一个唯一的 LogRecord。

每次调用产生且仅产生一个 LogRecord。

不得产生隐式副作用。
BC-002 Level Filtering
低于当前 Logger Level 的日志必须被直接丢弃。

不得进入 Formatter。

不得进入 Sink。

复杂度：

O(1)
BC-003 Record Immutability
LogRecord 一旦创建。

不得修改。

保证：

deterministic replay
future async compatibility
BC-004 Sink Isolation
Sink 之间完全隔离。

例如：

FileSink 失败

不得影响

ConsoleSink
BC-005 Formatter Isolation
Formatter 不得持有 Sink 状态。

Sink 不得持有 Formatter 状态。

保持：

Formatter
Output

完全解耦
BC-006 Fatal Logging
Fatal Log

必须：

生成日志
写入全部 Sink
Flush
终止进程

顺序不可改变。

BC-007 No Exception Escape
Logger API

不得向调用方传播异常。

所有异常：

内部捕获
内部处理
BC-008 Thread Safety
多个线程可同时提交日志。

V1：

保证正确性

不保证无锁
BC-009 Deterministic Formatting

同样输入：

Timestamp
Level
ThreadId
Message

必须产生：

完全相同输出
BC-010 Flush Guarantee

调用：

Logger::Flush()

必须保证：

所有 Sink 已完成 Flush

Concurrency Model (V1 FINAL):

1. Logger::Log is lock-free at interface level.

2. Logger does not guarantee ordering across threads.

3. Logger does not perform synchronization internally.

4. Each Sink implementation is responsible for its own thread safety.

5. No shared locking is allowed in Logger core path.

6. V1 prioritizes throughput over ordering consistency.