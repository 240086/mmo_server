# TaskGraph Interface Contract

Version: V1

Layer: Layer2 Runtime & Scheduler

Module: TaskGraph

Status: Design Freeze Candidate

---

# 1. Design Goal

TaskGraph provides deterministic task dependency metadata for runtime scheduling.

TaskGraph describes:

* task nodes
* task dependency topology
* execution ordering constraints

TaskGraph does not execute tasks.

TaskGraph does not schedule tasks.

TaskGraph does not contain gameplay logic.

TaskGraph exists solely as a runtime execution topology description.

---

# 2. Architectural Position

Layer Relationship:

RuntimePipeline

↓

RuntimePhase

↓

TaskGraph

↓

JobDispatch

↓

TickScheduler

TaskGraph is the topology metadata provider of Layer2.

TaskGraph is consumed by:

* JobDispatch
* TickScheduler

TaskGraph provides execution dependency information.

TaskGraph does not own execution.

---

# 3. Responsibilities

TaskGraph SHALL:

* store task metadata
* store task dependency metadata
* provide deterministic topology
* provide dependency traversal information
* provide startup-time validation input

TaskGraph SHALL NOT:

* execute tasks
* store system implementations
* store gameplay state
* store scene state
* store entity state
* store actor state
* perform scheduling
* perform synchronization
* perform locking

---

# 4. Dependency Ownership

TaskGraph owns:

* TaskNode metadata
* Dependency topology metadata

TaskGraph does not own:

* worker threads
* task implementations
* execution queues
* runtime context
* phase execution state

Ownership remains external.

---

# 5. Runtime Lifetime

Creation:

Runtime Initialization

↓

TaskGraph Build

↓

TaskGraph Validation

↓

TaskGraph Freeze

↓

Runtime Start

Destruction:

Runtime Shutdown

↓

TaskGraph Destroy

TaskGraph is immutable after runtime startup.

---

# 6. Immutability Contract

After initialization:

TaskGraph must become logically immutable.

Allowed:

Read access

Forbidden:

Node insertion

Node removal

Dependency insertion

Dependency removal

Topology mutation

Runtime mutation is prohibited.

---

# 7. Determinism Contract

TaskGraph must remain deterministic.

Identical input configuration must produce:

* identical node count
* identical node ordering
* identical dependency ordering
* identical topology layout

Runtime execution order must never depend on:

* hash ordering
* pointer addresses
* allocation order
* thread timing

---

# 8. Task Identity Contract

Each task must possess a unique TaskId.

Requirements:

Unique

Stable

Deterministic

TaskId reuse is prohibited inside the same graph.

Invalid task references are fatal.

---

# 9. Dependency Contract

Task dependencies form a Directed Acyclic Graph.

Requirements:

No cycles

No self dependency

No duplicate dependency edges

No invalid task references

Examples:

Valid:

Movement

↓

AOI

↓

Combat

Invalid:

Movement

↓

AOI

↓

Movement

Cycle detection is mandatory during startup validation.

---

# 10. Phase Dependency Contract

TaskGraph must respect RuntimePipeline ordering.

Allowed:

Input

↓

Simulation

↓

PostSimulation

↓

Output

Forward dependencies are allowed.

Forbidden:

Simulation → Input

PostSimulation → Input

Output → Simulation

Output → Input

Reverse phase dependencies are fatal.

Validation must occur during startup.

---

# 11. RuntimePhase Association

Each TaskNode belongs to exactly one RuntimePhase.

A task cannot belong to multiple phases.

A task cannot migrate between phases at runtime.

Phase assignment is immutable.

---

# 12. Storage Contract

TaskGraph is metadata only.

TaskGraph stores:

TaskNode metadata

Dependency metadata

TaskGraph does not store:

std::function

callbacks

systems

actors

entities

scenes

network objects

No executable business code is allowed.

---

# 13. Memory Contract

TaskGraph must remain cache-friendly.

Preferred storage:

Flat contiguous arrays

Allowed:

std::vector during startup

Forbidden:

linked list topology

tree ownership graphs

pointer-heavy structures

virtual inheritance chains

Runtime allocations must not occur after freeze.

---

# 14. Dependency Traversal Contract

Runtime dependency traversal must avoid full graph scans.

TaskGraph must provide:

O(1) access to child dependency ranges.

Linear scans across all dependency edges during task completion are prohibited.

Topology representation must support efficient downstream traversal.

---

# 15. Thread Safety Contract

Build Phase:

Single-threaded

Freeze Phase:

Single-threaded

Runtime Phase:

Read-only

Multiple readers are allowed.

Writers are forbidden.

No locking is required after freeze.

---

# 16. Failure Philosophy

TaskGraph corruption is fatal.

Examples:

Invalid TaskId

Dependency cycle

Reverse phase dependency

Topology corruption

Policy:

Fail Fast

Abort Startup

No runtime recovery is required.

---

# 17. Future Evolution

V2:

TaskGraphBuilder

Topology Validation Report

Graph Diagnostics

V3:

Hot Reload Validation

Graph Visualization Export

V4:

Distributed TaskGraph Metadata

Cross-node Dependency Metadata

These additions must not break V1 ABI.

---

# 18. ABI Stability Requirements

TaskGraph V1 metadata structures must remain:

Standard Layout

Trivially Copyable

Trivially Destructible

No virtual table

No RTTI dependency

ABI stability has higher priority than convenience APIs.

---

# 19. Layer Boundary Rules

TaskGraph must remain a Layer2 control-plane module.

Layer3 Simulation systems may register metadata into TaskGraph.

Layer3 must never execute through TaskGraph.

TaskGraph must never know:

MovementSystem

CombatSystem

SkillSystem

AOISystem

SceneSystem

TaskGraph only understands task topology metadata.

No gameplay awareness is allowed.
