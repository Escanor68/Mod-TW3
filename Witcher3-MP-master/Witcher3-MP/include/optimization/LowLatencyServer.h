#pragma once
#include "Common.h"
#include "networking/net_server.h"

namespace Optimization
{
    template<typename T>
    class LowLatencyServer : public Networking::server_interface<T>
    {
    public:
        LowLatencyServer(uint16_t port);
        virtual ~LowLatencyServer() = default;
        
    protected:
        virtual bool OnClientConnect(std::shared_ptr<Networking::connection<T>> client) override;
        virtual void OnClientDisconnect(std::shared_ptr<Networking::connection<T>> client) override;
        virtual void OnMessageReceived(std::shared_ptr<Networking::connection<T>> client, Networking::message<T>& msg) override;
    };
}