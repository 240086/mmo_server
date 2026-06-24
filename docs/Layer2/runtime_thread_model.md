untime Thread Model
Design Goal

The runtime layer provides deterministic and scalable execution for MMO simulation systems.

The runtime must support:

deterministic tick progression
task parallelism
fixed execution ordering
future scene partitioning
future distributed execution

The runtime must never depend on gameplay logic.

Core Philosophy

The runtime owns execution.

Simulation systems own business logic.

Runtime
    ↓
Execution

Simulation
    ↓
Logic

Runtime decides:

when tasks run
where tasks run
synchronization points

Simulation decides:

what tasks do
Thread Roles

Runtime defines three categories of threads.

Runtime Thread

Responsible for:

tick progression
pipeline execution
graph construction
synchronization

Count:

1

Only one runtime thread exists.

This thread owns:

TickScheduler
RuntimePipeline
TaskGraph
Worker Threads

Responsible for:

task execution

Count:

N

Configurable.

Example:

cpu_cores - 1

Worker threads never advance ticks.

Worker threads never modify runtime state.

External Threads

Examples:

Net IO Thread

DB Thread

Gateway Thread

Redis Thread

External threads never access simulation state directly.

Communication occurs through queues or mailboxes.

Runtime Ownership Model

Runtime thread owns:

RuntimeContext

TickScheduler

RuntimePipeline

TaskGraph

Worker threads own:

temporary task execution state

No worker thread owns simulation systems.

Tick Execution Model

Each tick executes:

Tick Start

↓

Build Pipeline

↓

Build TaskGraph

↓

Dispatch Tasks

↓

Barrier

↓

Commit Phase

↓

Tick End

The next tick never starts before the previous tick completes.

Deterministic Execution

Determinism is guaranteed by:

Fixed Tick Order
Tick N

↓

Tick N+1

Never:

Tick N+1 before Tick N completes
Fixed Pipeline Order

Example:

Movement

↓

AOI

↓

Skill

↓

Combat

Order never changes at runtime.

Explicit Dependencies

Task execution order is defined by TaskGraph.

Never by thread scheduling order.

Synchronization Model

Runtime uses barrier synchronization.

Example:

Movement Tasks

↓

Barrier

↓

AOI Tasks

↓

Barrier

↓

Combat Tasks

Workers never synchronize with each other directly.

Synchronization is managed only by runtime.

Data Access Rules
Rule 1

RuntimeContext

single writer
multiple readers

Writer:

Runtime Thread

Readers:

Worker Threads
Rule 2

Task Local Data

Owned exclusively by task execution thread.

No synchronization required.

Rule 3

Shared Simulation Data

Must be partitioned before parallel execution.

Example:

Scene Partition A

Scene Partition B

Scene Partition C

Each task owns its partition.

Future Evolution

V2

Work Stealing

V3

Fiber Scheduler

V4

Cross Scene Parallelism

V5

Distributed Task Graph
