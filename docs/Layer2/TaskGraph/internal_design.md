# Internal Design

## Core Philosophy

TaskGraph is a compact immutable DAG.

TaskGraph stores metadata only.

TaskGraph does not store executable logic.

---

## Storage Model

TaskNode

{
TaskId
PhaseId
}

TaskDependency

{
ParentTaskId
ChildTaskId
}

TaskGraph

{
Nodes
Dependencies
}

---

## Data Layout

Flat Arrays

Nodes

[0]
[1]
[2]
...

Dependencies

[0]
[1]
[2]
...

No linked list.

No tree structure.

No virtual dispatch.

---

## Complexity

Build:

O(V + E)

Topological Validation:

O(V + E)

Read:

O(1)

Memory:

O(V + E)

---

## Graph Validation

Validation occurs once.

Runtime startup only.

Checks:

* duplicate task ids
* invalid phase ids
* missing dependencies
* cycle detection

---

## Cycle Detection

Algorithm:

Kahn Topological Sort

Complexity:

O(V + E)

Failure:

Runtime startup abort.

---

## Runtime Access

Graph becomes immutable after validation.

Worker threads perform read-only access.

No locks required.

---

## Future Evolution

V2:

Graph Metrics

V3:

Dependency Visualization

V4:

Hot Reload Validation
