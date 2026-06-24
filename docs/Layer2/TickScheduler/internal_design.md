# Internal Design

## Core Philosophy

TickScheduler is the root runtime clock.

TickScheduler does not execute gameplay logic.

TickScheduler only advances runtime state.

------------------------------------------------

# Execution Loop

Runtime Thread

while(running)
{
    WaitNextTick();

    UpdateRuntimeContext();

    ExecuteRuntimePipeline();

    AdvanceTick();
}

------------------------------------------------

# Timing Model

Clock Source:

Layer1 Timer

SteadyClock

Scheduling Strategy:

sleep_until(next_tick_time)

Fixed timestep:

Configurable

Default:

50ms

------------------------------------------------

# Tick Metadata

RuntimeContext

Stores:

TickId

FrameIndex

DeltaTime

RuntimeMode

WorkerCount

------------------------------------------------

# Overrun Policy

V1:

Log warning

Continue execution

No catch-up

No rollback

------------------------------------------------

# Complexity

Per Tick:

O(1)

Memory:

O(1)

------------------------------------------------

# Future Evolution

V2:

Catch-Up Policy

V3:

Multi Runtime Domain

V4:

Distributed Tick Synchronization
