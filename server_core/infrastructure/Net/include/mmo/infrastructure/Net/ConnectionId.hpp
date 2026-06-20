// /home/xj/projects/mmo_server/server_core/infrastructure/Net/include/mmo/infrastructure/Net/ConnectionId.hpp
#pragma once

#include <cstdint>

namespace mmo::infrastructure::net
{
    using ConnectionId = std::uint64_t;

    inline constexpr ConnectionId InvalidConnectionId = 0;
}