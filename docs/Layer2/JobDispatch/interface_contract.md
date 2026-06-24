# JobDispatch Interface Contract

## Runtime State

JobDispatch shall provide:

TaskRuntimeState

Requirements:

* standard layout
* trivially copyable
* trivially destructible
* cache-line isolated

---

## Worker Pool

JobDispatch shall provide:

WorkerPool

Responsibilities:

* start workers
* stop workers
* join workers

Worker ownership belongs exclusively to JobDispatch.

---

## Ready Queue

JobDispatch shall depend on:

MPMCQueue<TaskId>

Requirements:

* bounded
* lock-free
* fixed capacity
* no runtime allocation

---

## Task Function

Definition:

using TaskFunction =
void(*)(RuntimeContext&);

Requirements:

* noexcept preferred
* direct invocation
* no virtual dispatch

---

## Task Registry

JobDispatch shall provide:

TaskFunction registry

Requirements:

* O(1) lookup
* static capacity
* TaskId indexed

---

## Execution API

JobDispatch shall provide:

Initialize()

ExecutePhase()

Shutdown()

Responsibilities:

Initialize:
Build runtime state

ExecutePhase:
Execute all tasks for a phase

Shutdown:
Stop worker threads

---

## Barrier API

JobDispatch shall provide:

WaitForPhaseCompletion()

Requirements:

* atomic wait based
* no busy spinning
* deterministic completion

---

## Determinism

Identical inputs must produce identical scheduling legality.

Dependency rules must never be violated.

---

## Memory Contract

No heap allocation during Tick execution.

No ownership transfer during task execution.

No topology mutation.

---

## ABI Contract

All runtime metadata structures must satisfy:

std::is_standard_layout_v

std::is_trivially_copyable_v

std::is_trivially_destructible_v
