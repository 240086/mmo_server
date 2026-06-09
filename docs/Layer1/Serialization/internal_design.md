Serialization V1 Internal Design

V1
Writer

BinaryWriter
    ↓
PacketWriter
    ↓
PacketBuffer

Reader

BinaryReader
    ↓
PacketReader
    ↓
PacketView

V2应该变成
IBufferWriter
     ↑
PacketWriter
     ↑
BinaryWriter

即：

class IBufferWriter
{
public:
    virtual bool WriteBytes(...) = 0;
};

------------------------------------------------

Primitive Serialization

uint32_t

raw copy

float

raw copy

double

raw copy

------------------------------------------------

String Serialization

uint32 length
char[length]

------------------------------------------------

Thread Model

V1

Single Thread

V2

Thread Safe Context

------------------------------------------------

Memory Model

No Ownership

Borrow Only

------------------------------------------------

Complexity

Primitive

O(1)

String

O(n)

------------------------------------------------

Endianness

Little Endian Only

V1

Cross Endian

Deferred
