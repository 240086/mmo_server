# Internal Design

v1:
Boost.Asio

Single Acceptor

Single IO Thread

Length Prefix Packet

Connection Table

Inbound Queue

Outbound Queue

No Session

No RPC

No Gameplay

No Actor

## Core Philosophy

Network transport and gameplay logic
must remain completely separated.

Transport Layer

    Socket
      ↓
    Connection
      ↓
    Packet Assembly
      ↓
    Receive Queue

Runtime Layer

    Receive Queue
      ↓
    Runtime Pipeline

---

# Receive Flow

Socket

    recv()

↓

Receive Buffer

↓

Packet Decoder

↓

Packet Queue

---

# Send Flow

Runtime

↓

Send Queue

↓

Packet Encoder

↓

Socket

# Send Queue

MPSC

Multiple producers:
    Runtime
    RPC
    Gateway

Single consumer:
    IO thread

# Receive Queue

MPSCQueue<ReceivedPacket>

---

# Storage Model

Connection Table

unordered_map<ConnectionId, Connection>

Lookup Complexity

O(1)

---

# Packet Assembly

V1 packet format

[Length][PacketId][Payload]

Length:

uint32_t

Payload:

variable length

PacketId:
uint16_t
LoginReq
MoveReq
AttackReq
ChatReq

---

# Determinism

Packet ordering is preserved per connection.

Cross-connection ordering is undefined.

---

# Scalability

Current:

single io thread

Future:

multi-reactor

future:

io_uring backend

future:

distributed gateway clusters

Current Backend:

Boost.Asio Reactor

Future Backends:

epoll
io_uring

# Connection Lifecycle

Created
    ↓
Active
    ↓
Closing
    ↓
Closed

Rules:

ConnectionId is immutable
Closed connections are never reused during lifetime
Disconnect events are delivered at most once

using ConnectionId = uint64_t;

Runtime systems may only use ConnectionId.

Raw sockets are Net internal implementation details.
