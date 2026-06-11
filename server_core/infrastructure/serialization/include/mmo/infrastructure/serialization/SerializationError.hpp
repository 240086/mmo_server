// /home/xj/projects/mmo_server/server_core/infrastructure/serialization/include/mmo/infrastructure/serialization/SerializationError.hpp

enum class SerializationError
{
    None,

    OutOfBounds,

    InvalidStringLength,

    UnsupportedType,

    BufferOverflow
};