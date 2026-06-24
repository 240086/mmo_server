# JobDispatch Behavior Contract

## Rule 1

JobDispatch executes only tasks that exist inside TaskGraph.

Creation of new tasks during runtime is prohibited.

---

## Rule 2

TaskGraph topology is immutable.

JobDispatch must never modify topology metadata.

---

## Rule 3

Runtime dependency counters are maintained separately from TaskGraph.

Topology metadata remains read-only.

---

## Rule 4

Task execution order must satisfy all TaskGraph dependencies.

Execution before dependency completion is prohibited.

---

## Rule 5

Ready tasks are inserted into ReadyQueue.

Workers consume tasks only from ReadyQueue.

---

## Rule 6

Task completion releases successor nodes.

Successor dependency counters are decremented atomically.

---

## Rule 7

A successor task becomes runnable only when:

remainingDependencies == 0

---

## Rule 8

Cross-phase execution is prohibited.

JobDispatch executes only tasks belonging to the currently active RuntimePhase.

---

## Rule 9

Phase completion requires:

* ReadyQueue empty
* Active task count equals zero

---

## Rule 10

Barrier synchronization must use:

std::atomic::wait
std::atomic::notify_all

Busy-spin waiting is prohibited.

---

## Rule 11

No dynamic memory allocation is permitted during Tick execution.

---

## Rule 12

No virtual dispatch is permitted on execution hot paths.

---

## Rule 13

Task execution functions must be direct function-pointer calls.

std::function is prohibited.

---

## Rule 14

Worker count is fixed for runtime lifetime.

Dynamic worker creation is prohibited.
