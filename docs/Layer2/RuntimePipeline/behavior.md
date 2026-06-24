Pipeline phase order is immutable.

Phase execution order:

Input

↓

Simulation

↓

PostSimulation

↓

Output

Pipeline must never reorder phases.

Pipeline must never skip phases.

Pipeline must never execute phases in parallel.

Pipeline does not execute gameplay logic.

Pipeline does not own task execution.
