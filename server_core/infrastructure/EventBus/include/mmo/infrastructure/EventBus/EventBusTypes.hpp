// /home/xj/projects/mmo_server/server_core/infrastructure/EventBus/include/mmo/infrastructure/EventBus/EventBusTypes.hpp
#pragma once

#include <atomic>
#include <cstdint>

namespace mmo::infrastructure::event_bus
{
    using EventTypeId = std::uint32_t;

    namespace detail
    {
        inline EventTypeId GenerateNextTypeId() noexcept
        {
            static std::atomic<EventTypeId> counter{1};

            return counter.fetch_add(
                1,
                std::memory_order_relaxed);
        }
    }

    template <typename TEvent>
    inline EventTypeId GetEventTypeId() noexcept
    {
        static const EventTypeId id =
            detail::GenerateNextTypeId();

        return id;
    }
}