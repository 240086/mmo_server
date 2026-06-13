// /home/xj/projects/mmo_server/server_core/infrastructure/EventBus/include/mmo/infrastructure/EventBus/EventBus.hpp
#pragma once

#include <atomic>
#include <functional>
#include <memory>
#include <mutex>
#include <shared_mutex>
#include <unordered_map>
#include <vector>

#include <mmo/infrastructure/EventBus/IEventBus.hpp>

namespace mmo::infrastructure::event_bus
{
    class EventBus final : public IEventBus
    {
    public:
        EventBus() = default;

        ~EventBus() override = default;

        EventBus(const EventBus &) = delete;
        EventBus &operator=(const EventBus &) = delete;

        EventBus(EventBus &&) noexcept = delete;
        EventBus &operator=(EventBus &&) noexcept = delete;

    public:
        template <typename TEvent, typename F>
        SubscriptionToken Subscribe(F &&handler)
        {
            auto wrapper =
                [h = std::forward<F>(handler)](const void *eventRaw)
            {
                h(*static_cast<const TEvent *>(eventRaw));
            };

            return SubscribeInternal(
                GetEventTypeId<TEvent>(),
                std::function<void(const void *)>(
                    std::move(wrapper)));
        }

        template <typename TEvent>
        void Publish(const TEvent &event)
        {
            PublishInternal(
                GetEventTypeId<TEvent>(),
                &event);
        }

    public:
        SubscriptionToken SubscribeInternal(
            EventTypeId type,
            std::function<void(const void *)> &&handler) override;

        void Unsubscribe(
            const SubscriptionToken &token) override;

        void PublishInternal(
            EventTypeId type,
            const void *eventRaw) override;

        void Clear() override;

    private:
        struct HandlerEntry
        {
            std::uint64_t id;

            std::function<void(const void *)> fn;

            std::atomic<bool> active{true};
        };

        using HandlerPtr =
            std::shared_ptr<HandlerEntry>;

        using HandlerList =
            std::vector<HandlerPtr>;

        struct Snapshot
        {
            HandlerList handlers;
        };

    private:
        void RebuildSnapshot(EventTypeId type);

    private:
        std::unordered_map<
            EventTypeId,
            HandlerList>
            handlers_;

        std::unordered_map<
            EventTypeId,
            std::shared_ptr<Snapshot>>
            snapshots_;

        std::atomic<std::uint64_t> nextId_{1};

        std::mutex writeMutex_;

        mutable std::shared_mutex topologyMutex_;
    };
}