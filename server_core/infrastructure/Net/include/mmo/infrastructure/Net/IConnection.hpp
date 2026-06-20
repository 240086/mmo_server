// /home/xj/projects/mmo_server/server_core/infrastructure/Net/include/mmo/infrastructure/Net/IConnection.hpp
#pragma once

#include <mmo/infrastructure/Net/ConnectionId.hpp>

namespace mmo::infrastructure::net
{
    class IConnection
    {
    public:
        virtual ~IConnection() = default;

        virtual ConnectionId GetId() const noexcept = 0;

        virtual bool IsConnected() const noexcept = 0;

        virtual void Disconnect() = 0;
    };
}