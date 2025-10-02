#pragma once
#include "Common.h"
#include "networking/net_server.h"
#include "version/NextGenVersionManager.h"

namespace Optimization
{
    class NextGenServer : public Networking::server_interface<MessageType>
    {
    public:
        NextGenServer(uint16_t port);
        virtual ~NextGenServer() = default;

    protected:
        virtual bool OnClientConnect(std::shared_ptr<Networking::connection<MessageType>> client) override;
        virtual void OnClientDisconnect(std::shared_ptr<Networking::connection<MessageType>> client) override;
        virtual void OnMessageReceived(std::shared_ptr<Networking::connection<MessageType>> client, Networking::message<MessageType>& msg) override;

    private:
        std::unique_ptr<Version::NextGenVersionManager> m_versionManager;
    };
}