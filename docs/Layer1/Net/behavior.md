# Net Behavior Contract

## Design Goal

The network layer provides transport
for upper MMO systems.

The network layer is responsible only for:

- accept
- receive
- send
- disconnect

The network layer must never execute gameplay logic.

---

# Connection Ownership

Connections are owned by NetServer.

Runtime systems may only access connection ids.

Raw socket ownership must never leave Net layer.

---

# Packet Boundary

The network layer uses length-prefixed packets.

Packet framing is deterministic.

Partial packets must never be exposed.

---

# Receive Semantics

Incoming bytes are accumulated internally.

Packets become visible only after complete assembly.

---

# Send Semantics

Outgoing packets are queued.

Partial sends are handled internally.

Upper layers must never observe socket state.

---

# Threading Model

Accept thread:

- accepts connections

IO thread:

- performs recv/send

Runtime thread:

- consumes assembled packets

---

# Failure Philosophy

Network errors are connection-local.

A single bad connection must never affect
other active connections.

---

# Future Evolution

V2 may support:

- epoll
- io_uring
- TLS
- KCP
- WebSocket
