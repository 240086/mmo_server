// /home/xj/projects/mmo_server/server_core/infrastructure/EventBus/include/mmo/infrastructure/EventBus/SubscriptionToken.hpp
#pragma once

#include <compare>
#include <cstdint>

#include <mmo/infrastructure/EventBus/EventBusTypes.hpp>

namespace mmo::infrastructure::event_bus
{
    struct SubscriptionToken
    {
        std::uint64_t id{0};

        EventTypeId type{0};

        [[nodiscard]]
        bool Valid() const noexcept
        {
            return id != 0;
        }

        void Reset() noexcept
        {
            id = 0;
            type = 0;
        }

        auto operator<=>(const SubscriptionToken &) const = default;
    };
}