<!-- /home/xj/projects/mmo_server/docs/Layer1/packet/internal_design.md -->
# Packet Backend

PacketBuffer uses contiguous memory.

Current backend:

std::vector<std::byte>

Future backend:

memory_pool_allocation

# Header Layout

PacketHeader is fixed size.

Header fields:

opcode
payload_size
sequence
flags

# Reader Design

PacketReader is cursor-based.

Read operations advance cursor.

Bounds checking is mandatory.

# Writer Design

PacketWriter is cursor-based.

Write operations advance cursor.

Capacity validation is mandatory.

# PacketView

PacketView is lightweight.

PacketView owns no memory.

PacketView supports zero-copy inspection.

# Endian Contract

V1:

All packet fields are encoded as little-endian.

Cross-endian communication is not supported.

Future versions may introduce explicit endian conversion utilities.

PacketReader and PacketWriter do not perform automatic conversion.

# Future RPC Compatibility

PacketHeader is transport stable.

PacketHeader layout must remain backward compatible.

New metadata fields must be appended.

Existing fields must never change meaning.

Packet payload remains opaque to transport layer.
