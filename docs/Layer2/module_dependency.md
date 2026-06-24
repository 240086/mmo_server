Module Dependency Rules
Design Goal

Layer2 modules must maintain strict dependency direction.

No cyclic dependency is allowed.

Execution modules depend only on lower-level runtime services.

Dependency Graph
RuntimeContext
       ↑

TickScheduler
       │

RuntimePipeline
       │

RuntimePhase
       │

TaskGraph
       │

JobDispatch

ActorMailbox:

Independent

Optional Service
RuntimeContext

Responsibilities:

runtime state
tick metadata
execution metadata

Dependencies:

None

Must remain dependency-free.

RuntimeContext is the root module.

JobDispatch

Responsibilities:

worker management
task execution

Dependencies:

RuntimeContext

Must never depend on:

TaskGraph
RuntimePipeline
TickScheduler
TaskGraph

Responsibilities:

task storage
dependency tracking
ready task discovery

Dependencies:

RuntimeContext
JobDispatch

TaskGraph may submit tasks.

TaskGraph must never advance ticks.

RuntimePhase

Responsibilities:

define phase behavior
generate tasks

Dependencies:

RuntimeContext
TaskGraph

Must never depend on TickScheduler.

RuntimePipeline

Responsibilities:

phase ordering
phase execution

Dependencies:

RuntimeContext
RuntimePhase
TaskGraph

Must never depend on JobDispatch internals.

TickScheduler

Responsibilities:

tick progression
timing control
synchronization

Dependencies:

RuntimeContext
RuntimePipeline

Must not know phase details.

Must not know task implementation details.

ActorMailbox

Responsibilities:

asynchronous messaging

Dependencies:

RuntimeContext

No mandatory dependency from:

TickScheduler
RuntimePipeline
TaskGraph
Dependency Constraints

Allowed:

TickScheduler
    ↓
RuntimePipeline

Allowed:

RuntimePipeline
    ↓
RuntimePhase

Allowed:

RuntimePhase
    ↓
TaskGraph

Allowed:

TaskGraph
    ↓
JobDispatch

Forbidden:

JobDispatch
    ↓
TickScheduler

Forbidden:

TaskGraph
    ↓
TickScheduler

Forbidden:

RuntimeContext
    ↓
Any Module
