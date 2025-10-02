#include "networking/net_server.h"
#include "networking/MessageTypes.h"
#include "utils/Logger.h"
#include "optimization/NetworkOptimizer.h"
#include <iostream>
#include <chrono>

namespace Networking
{
    // Network server implementation for Witcher3-MP
    template<typename T>
    class Witcher3MPServer : public server_interface<T>
    {
    public:
        Witcher3MPServer(uint16_t port)
            : server_interface<T>(port), m_maxClients(100), m_compressionEnabled(true)
        {
            LOG_INFO_CAT(LogCategory::NETWORK, "Witcher3MPServer created on port " + std::to_string(port));
        }

        virtual ~Witcher3MPServer()
        {
            Stop();
            LOG_INFO_CAT(LogCategory::NETWORK, "Witcher3MPServer destroyed");
        }

        bool Start() override
        {
            LOG_INFO_CAT(LogCategory::NETWORK, "Starting Witcher3-MP server...");
            return server_interface<T>::Start();
        }

        void Stop() override
        {
            LOG_INFO_CAT(LogCategory::NETWORK, "Stopping server...");
            server_interface<T>::Stop();
        }

        void Update()
        {
            server_interface<T>::Update();
        }

        // Server statistics
        size_t GetConnectedClients() const { return server_interface<T>::m_deqConnections.size(); }
        size_t GetMaxClients() const { return m_maxClients; }

        // Compression control
        void SetCompressionEnabled(bool enabled)
        {
            m_compressionEnabled = enabled;
            LOG_INFO_CAT(LogCategory::NETWORK, "Compression " + (enabled ? "enabled" : "disabled"));
        }

        bool IsCompressionEnabled() const { return m_compressionEnabled; }

    protected:
        bool OnClientConnect(std::shared_ptr<connection<T>> client) override
        {
            if (server_interface<T>::m_deqConnections.size() >= m_maxClients)
            {
                LOG_WARNING_CAT(LogCategory::NETWORK, "Server full, rejecting connection");
                return false;
            }

            LOG_INFO_CAT(LogCategory::NETWORK, "Client connected, ID: " + std::to_string(client->GetID()));
            return true;
        }

        void OnClientDisconnect(std::shared_ptr<connection<T>> client) override
        {
            if (client)
            {
                LOG_INFO_CAT(LogCategory::NETWORK, "Client disconnected, ID: " + std::to_string(client->GetID()));
            }
        }

        void OnMessageReceived(std::shared_ptr<connection<T>> client, message<T>& msg) override
        {
            if (!client)
                return;

            MessageTypes msgType = static_cast<MessageTypes>(msg.header.id);
            
            switch (msgType)
            {
                case MessageTypes::ClientPing:
                    ProcessClientPing(client, msg);
                    break;
                case MessageTypes::TC_UPDATE_POS:
                    ProcessPositionUpdate(client, msg);
                    break;
                case MessageTypes::TC_CHAT_MESSAGE:
                    ProcessChatMessage(client, msg);
                    break;
                default:
                    LOG_DEBUG_CAT(LogCategory::NETWORK, "Received message type: " + std::to_string(static_cast<int>(msgType)));
                    break;
            }
        }

    private:
        void ProcessClientPing(std::shared_ptr<connection<T>> client, const message<T>& msg)
        {
            message<T> pongMsg;
            pongMsg.header.id = static_cast<T>(MessageTypes::ServerPong);
            pongMsg << msg.body;
            client->Send(pongMsg);
        }

        void ProcessPositionUpdate(std::shared_ptr<connection<T>> client, const message<T>& msg)
        {
            LOG_DEBUG_CAT(LogCategory::NETWORK, "Processing position update from client " + std::to_string(client->GetID()));
        }

        void ProcessChatMessage(std::shared_ptr<connection<T>> client, const message<T>& msg)
        {
            std::string chatMessage;
            msg >> chatMessage;
            LOG_INFO_CAT(LogCategory::NETWORK, "Client " + std::to_string(client->GetID()) + " chat: " + chatMessage);
        }

        size_t m_maxClients;
        bool m_compressionEnabled;
    };

    // Typedef for easy use
    using Witcher3MPServerType = Witcher3MPServer<MessageTypes>;
}
