# TaskGraph Behavior Contract

## Design Goal

TaskGraph describes task dependencies.

TaskGraph never executes tasks.

TaskGraph never owns worker threads.

TaskGraph never performs scheduling.

---

## Build Lifecycle

Runtime Startup

↓

Register Tasks

↓

Build Graph

↓

Validate DAG

↓

Freeze Graph

↓

Runtime Execution

---

## Runtime Access

TaskGraph is immutable after build.

Worker threads may observe graph metadata.

No graph mutation is allowed during runtime.

---

## Dependency Rules

Dependencies must form a DAG.

Cycles are forbidden.

Example:

Input

↓

Movement

↓

AOI

↓

Combat

↓

Output

Valid.

Example:

Movement → Combat

Combat → Movement

Invalid.

Runtime startup must fail.

---

## Determinism Rules

Graph topology must remain fixed.

Execution ordering must remain deterministic.

Identical graph input must generate identical topology.

---

## Failure Philosophy

Graph validation failure is fatal.

Cycle detection failure is fatal.

Missing dependency failure is fatal.

Fail Fast.
