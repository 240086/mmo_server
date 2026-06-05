<!-- /home/xj/projects/mmo_server/docs/Layer1/packet/behavior.md -->
# Design Philosophy

Packet is the canonical binary transport unit
within the MMO runtime.

Packet is transport-oriented.

Packet is not gameplay-oriented.

# Ownership Contract

PacketBuffer is the sole owner of packet memory.

PacketView is a non-owning borrowed view.

PacketReader borrows PacketView.

PacketWriter borrows PacketBuffer.

Ownership is never transferred implicitly.

Move operations preserve ownership.

Copy operations duplicate payload explicitly.

# Read Contract

PacketReader never mutates data.

PacketReader never reads beyond payload boundary.

# Write Contract

PacketWriter never writes beyond capacity.

PacketWriter advances write position only after successful write.

# Runtime Contract

Packet does not perform networking.

Packet does not perform serialization policy decisions.

Packet only exposes binary transport primitives.

# Invalid Usage

PacketView must not outlive PacketBuffer.

Concurrent writes are forbidden.

Concurrent reads and writes are forbidden.

# Lifetime Contract

PacketView must not outlive PacketBuffer.

PacketReader must not outlive PacketView.

PacketWriter must not outlive PacketBuffer.

Destroying the owner invalidates all dependent views.

No reference stability is guaranteed after Resize().

# Serialization Contract

Packet only stores bytes.

Packet does not interpret payload semantics.

Serialization layer defines:

- field order
- string encoding
- container encoding
- schema evolution

Packet remains serialization agnostic.
