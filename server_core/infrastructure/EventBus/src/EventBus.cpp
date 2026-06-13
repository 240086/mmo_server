#include <algorithm>

#include <mmo/infrastructure/EventBus/EventBus.hpp>

namespace mmo::infrastructure::event_bus
{

    SubscriptionToken EventBus::SubscribeInternal(
        EventTypeId type,
        std::function<void(const void *)> &&handler)
    {
        std::lock_guard<std::mutex> lock(writeMutex_);

        const std::uint64_t id =
            nextId_.fetch_add(
                1,
                std::memory_order_relaxed);

        auto entry =
            std::make_shared<HandlerEntry>();

        entry->id = id;
        entry->fn = std::move(handler);

        handlers_[type].push_back(entry);

        RebuildSnapshot(type);

        return SubscriptionToken{
            id,
            type};
    }

    void EventBus::Unsubscribe(
        const SubscriptionToken &token)
    {
        if (!token.Valid())
        {
            return;
        }

        std::lock_guard<std::mutex> lock(writeMutex_);

        auto mapIt =
            handlers_.find(token.type);

        if (mapIt == handlers_.end())
        {
            return;
        }

        auto &handlers = mapIt->second;

        for (auto &handler : handlers)
        {
            if (handler->id == token.id)
            {
                handler->active.store(
                    false,
                    std::memory_order_release);

                break;
            }
        }

        RebuildSnapshot(token.type);
    }

    void EventBus::PublishInternal(
        EventTypeId type,
        const void *eventRaw)
    {
        std::shared_ptr<Snapshot> snapshot;

        {
            std::shared_lock<std::shared_mutex> lock(
                topologyMutex_);

            auto it =
                snapshots_.find(type);

            if (it == snapshots_.end())
            {
                return;
            }

            snapshot = it->second;
        }

        if (!snapshot)
        {
            return;
        }

        for (const auto &handler : snapshot->handlers)
        {
            if (!handler->active.load(
                    std::memory_order_acquire))
            {
                continue;
            }

            handler->fn(eventRaw);
        }
    }

    void EventBus::Clear()
    {
        std::lock_guard<std::mutex> lock(
            writeMutex_);

        std::unique_lock<std::shared_mutex> topologyLock(
            topologyMutex_);

        handlers_.clear();
        snapshots_.clear();

        nextId_.store(
            1,
            std::memory_order_relaxed);
    }

    void EventBus::RebuildSnapshot(
        EventTypeId type)
    {
        auto mapIt =
            handlers_.find(type);

        if (mapIt == handlers_.end())
        {
            return;
        }

        auto snapshot =
            std::make_shared<Snapshot>();

        snapshot->handlers.reserve(
            mapIt->second.size());

        for (auto &handler : mapIt->second)
        {
            if (handler->active.load(
                    std::memory_order_acquire))
            {
                snapshot->handlers.push_back(handler);
            }
        }

        std::unique_lock<std::shared_mutex> topologyLock(
            topologyMutex_);

        snapshots_[type] =
            std::move(snapshot);
    }

}