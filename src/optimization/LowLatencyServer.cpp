#include "optimization/LowLatencyServer.h"
#include <iostream>

namespace Optimization
{
    template<typename T>
    LowLatencyServer<T>::LowLatencyServer(uint16_t port)
        : Networking::server_interface<T>(port)
    {
        // Constructor logic
    }

    template<typename T>
    bool LowLatencyServer<T>::OnClientConnect(std::shared_ptr<Networking::connection<T>> client)
    {
        std::cout << "Client connected: " << client->GetID() << std::endl;
    return true;
}

    template<typename T>
    void LowLatencyServer<T>::OnClientDisconnect(std::shared_ptr<Networking::connection<T>> client)
    {
        std::cout << "Client disconnected: " << client->GetID() << std::endl;
    }

    template<typename T>
    void LowLatencyServer<T>::OnMessageReceived(std::shared_ptr<Networking::connection<T>> client, Networking::message<T>& msg)
    {
        std::cout << "Message received from client " << client->GetID() << ": " << msg << std::endl;
    }

    // Explicit template instantiation for common types
    template class LowLatencyServer<int>;
}