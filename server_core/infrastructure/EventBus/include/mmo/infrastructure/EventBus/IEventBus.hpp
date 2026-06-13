// /home/xj/projects/mmo_server/server_core/infrastructure/EventBus/include/mmo/infrastructure/EventBus/IEventBus.hpp
#pragma once

#include <functional>
#include <mmo/infrastructure/EventBus/EventBusTypes.hpp>
#include <mmo/infrastructure/EventBus/SubscriptionToken.hpp>

namespace mmo::infrastructure::event_bus
{
    // 彻底剥离了对 IEvent 虚接口的强绑定，支持任意轻量级纯数据结构(POD)事件
    class IEventBus
    {
    public:
        virtual ~IEventBus() = default;

        // 统一采用底层单层类型擦除的纯指针契约
        virtual SubscriptionToken SubscribeInternal(
            EventTypeId type,
            std::function<void(const void *)> &&handler) = 0;

        virtual void Unsubscribe(const SubscriptionToken &token) = 0;

        virtual void PublishInternal(EventTypeId type, const void *eventRaw) = 0;

        virtual void Clear() = 0;
    };
}