# RuntimePipeline Interface Contract
Design Goal

RuntimePipeline provides deterministic phase ordering metadata for runtime execution.

RuntimePipeline defines:

Phase Sequence

RuntimePipeline does not perform:

Task Execution
Dependency Resolution
Parallel Dispatch
Synchronization
Scheduling

Those responsibilities belong to:

TaskGraph
JobDispatch
TickScheduler
Core Philosophy

RuntimePipeline is a passive data structure.

RuntimePipeline contains:

Ordered RuntimePhase Collection

RuntimePipeline does not own runtime behavior.

RuntimePipeline does not contain execution logic.

Ownership Contract

Owned by:

TickScheduler

Lifecycle:

Runtime Startup
    ↓
Pipeline Construction
    ↓
Runtime Execution
    ↓
Runtime Shutdown

Pipeline lifetime equals runtime lifetime.

Immutability Contract

Pipeline configuration is fixed after construction.

Forbidden:

AddPhase()
RemovePhase()
InsertPhase()
ReplacePhase()

during runtime execution.

Any runtime mutation is a fatal configuration error.

Visibility Contract

Runtime Thread:

Read

Worker Thread:

No Access Required

Task execution must not depend on pipeline mutation.

V1 Pipeline Definition

Mandatory order:

Input
    ↓
Simulation
    ↓
PostSimulation
    ↓
Output

Phase order must remain deterministic.

Determinism Contract

For every Tick:

Pipeline Sequence
must be identical.

Pipeline must not depend on:

Player Count
Scene Count
Worker Count
Network State
Runtime Load
RuntimePhase Requirements

Each phase must contain:

Valid RuntimePhaseId

Duplicate phases are forbidden.

Missing mandatory phases are forbidden.

Validation Rules

Valid:

Input
Simulation
PostSimulation
Output

Invalid:

Input
Simulation
Simulation
Output

Invalid:

Input
Output

Invalid:

PostSimulation
Simulation
Input
Output
Memory Contract

RuntimePipeline must remain:

Standard Layout
Trivially Copyable
Trivially Destructible

Heap allocation is forbidden.

Virtual dispatch is forbidden.

Ownership graphs are forbidden.

ABI Stability Contract

V1 public structure must remain ABI stable.

Allowed future additions:

PipelineVersion
PipelineFlags
PipelineMetadata

Forbidden:

Virtual Functions
Inheritance Hierarchy
Heap Ownership
Future Evolution

V2:

Custom Pipeline Definitions

Example:

Input
Simulation
Combat
PostSimulation
Output

V3:

Editor Generated Pipeline

V4:

Distributed Pipeline Metadata
