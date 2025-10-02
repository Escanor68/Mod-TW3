#include "optimization/NextGenServer.h"
#include <iostream>

namespace Optimization
{
NextGenServer::NextGenServer(uint16_t port)
        : server_interface<MessageType>(port)
    {
        m_versionManager = std::make_unique<Version::NextGenVersionManager>("1.0.0");
        std::cout << "NextGenServer initialized on port " << port << std::endl;
    }

    bool NextGenServer::OnClientConnect(std::shared_ptr<Networking::connection<MessageType>> client)
    {
        std::cout << "Client connected to NextGen server: " << client->GetID() << std::endl;
        return true;
    }

    void NextGenServer::OnClientDisconnect(std::shared_ptr<Networking::connection<MessageType>> client)
    {
        std::cout << "Client disconnected from NextGen server: " << client->GetID() << std::endl;
    }

    void NextGenServer::OnMessageReceived(std::shared_ptr<Networking::connection<MessageType>> client, Networking::message<MessageType>& msg)
    {
        std::cout << "Message received by NextGen server from client " << client->GetID() << std::endl;
    }
}