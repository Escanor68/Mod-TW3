#include "optimization/DynamicLowLatencyServer.h"
#include <iostream>

namespace Optimization
{
    template<typename T>
    DynamicLowLatencyServer<T>::DynamicLowLatencyServer(uint16_t port)
        : Networking::server_interface<T>(port)
    {
        // Constructor logic
    }

    template<typename T>
    bool DynamicLowLatencyServer<T>::OnClientConnect(std::shared_ptr<Networking::connection<T>> client)
    {
        std::cout << "Dynamic Client connected: " << client->GetID() << std::endl;
        return true;
    }

    template<typename T>
    void DynamicLowLatencyServer<T>::OnClientDisconnect(std::shared_ptr<Networking::connection<T>> client)
    {
        std::cout << "Dynamic Client disconnected: " << client->GetID() << std::endl;
    }

    template<typename T>
    void DynamicLowLatencyServer<T>::OnMessageReceived(std::shared_ptr<Networking::connection<T>> client, Networking::message<T>& msg)
    {
        std::cout << "Dynamic Message received from client " << client->GetID() << ": " << msg << std::endl;
    }

    // Explicit template instantiation for common types
    template class DynamicLowLatencyServer<int>;
}