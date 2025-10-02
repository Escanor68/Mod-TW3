#include "networking/net_client.h"
#include "networking/MessageTypes.h"
#include "utils/Logger.h"
#include "optimization/NetworkOptimizer.h"
#include <iostream>
#include <chrono>

namespace Networking
{
    // Network client implementation for Witcher3-MP
    template<typename T>
    class Witcher3MPClient : public client_interface<T>
    {
    public:
        Witcher3MPClient()
            : m_connected(false), m_ping(0.0f), m_packetLoss(0.0f), 
              m_compressionEnabled(true), m_lastPingTime(0)
        {
            LOG_INFO_CAT(LogCategory::NETWORK, "Witcher3MPClient created");
        }

        virtual ~Witcher3MPClient()
        {
            Disconnect();
            LOG_INFO_CAT(LogCategory::NETWORK, "Witcher3MPClient destroyed");
        }

        bool Connect(const std::string& host, const uint16_t port) override
        {
            LOG_INFO_CAT(LogCategory::NETWORK, "Attempting to connect to " + host + ":" + std::to_string(port));
            
            if (client_interface<T>::Connect(host, port))
            {
                m_connected = true;
                m_connectionStartTime = std::chrono::high_resolution_clock::now();
                
                // Send initial connection message
                SendConnectionRequest();
                
                LOG_INFO_CAT(LogCategory::NETWORK, "Successfully connected to server");
                return true;
            }
            else
            {
                LOG_ERROR_CAT(LogCategory::NETWORK, "Failed to connect to server");
                return false;
            }
        }

        void Disconnect() override
        {
            if (m_connected)
            {
                LOG_INFO_CAT(LogCategory::NETWORK, "Disconnecting from server");
                
                // Send disconnect message
                SendDisconnectMessage();
                
                client_interface<T>::Disconnect();
                m_connected = false;
                
                LOG_INFO_CAT(LogCategory::NETWORK, "Disconnected from server");
            }
        }

        bool IsConnected() const override
        {
            return m_connected && client_interface<T>::IsConnected();
        }

        void Update()
        {
            if (!IsConnected())
                return;

            // Process incoming messages
            ProcessIncomingMessages();
            
            // Send ping if needed
            SendPingIfNeeded();
            
            // Update network statistics
            UpdateNetworkStats();
        }

        // Witcher3-MP specific methods
        void SendPlayerData(const std::vector<uint8_t>& playerData)
        {
            if (!IsConnected())
                return;

            message<T> msg;
            msg.header.id = static_cast<T>(MessageTypes::TC_REQUEST_PLAYERDATA);
            
            // Compress player data if compression is enabled
            if (m_compressionEnabled)
            {
                auto& optimizer = Optimization::NetworkOptimizer::GetInstance();
                auto compressedData = optimizer.CompressMessage(msg);
                msg = compressedData;
            }
            
            client_interface<T>::MessageServer(msg);
            LOG_DEBUG_CAT(LogCategory::NETWORK, "Sent player data (" + std::to_string(playerData.size()) + " bytes)");
        }

        void SendPositionUpdate(float x, float y, float z, float w)
        {
            if (!IsConnected())
                return;

            message<T> msg;
            msg.header.id = static_cast<T>(MessageTypes::TC_UPDATE_POS);
            
            // Pack position data
            msg << x << y << z << w;
            
            client_interface<T>::MessageServer(msg);
            LOG_DEBUG_CAT(LogCategory::NETWORK, "Sent position update: (" + 
                         std::to_string(x) + ", " + std::to_string(y) + ", " + 
                         std::to_string(z) + ", " + std::to_string(w) + ")");
        }

        void SendChatMessage(const std::string& message)
        {
            if (!IsConnected())
                return;

            message<T> msg;
            msg.header.id = static_cast<T>(MessageTypes::TC_CHAT_MESSAGE);
            
            // Pack chat message (max 100 characters)
            std::string truncatedMessage = message.substr(0, 100);
            msg << truncatedMessage;
            
            client_interface<T>::MessageServer(msg);
            LOG_INFO_CAT(LogCategory::NETWORK, "Sent chat message: " + truncatedMessage);
        }

        void SendCombatAction(uint32_t targetId, float damage, const std::string& actionType)
        {
            if (!IsConnected())
                return;

            message<T> msg;
            msg.header.id = static_cast<T>(MessageTypes::PlayerAttack);
            
            // Pack combat data
            msg << targetId << damage << actionType;
            
            client_interface<T>::MessageServer(msg);
            LOG_INFO_CAT(LogCategory::NETWORK, "Sent combat action: " + actionType + " on target " + std::to_string(targetId));
        }

        // Network statistics
        float GetPing() const { return m_ping; }
        float GetPacketLoss() const { return m_packetLoss; }
        size_t GetBytesSent() const { return m_bytesSent; }
        size_t GetBytesReceived() const { return m_bytesReceived; }
        size_t GetPacketsSent() const { return m_packetsSent; }
        size_t GetPacketsReceived() const { return m_packetsReceived; }

        // Compression control
        void SetCompressionEnabled(bool enabled)
        {
            m_compressionEnabled = enabled;
            LOG_INFO_CAT(LogCategory::NETWORK, "Compression " + (enabled ? "enabled" : "disabled"));
        }

        bool IsCompressionEnabled() const { return m_compressionEnabled; }

    private:
        void SendConnectionRequest()
        {
            message<T> msg;
            msg.header.id = static_cast<T>(MessageTypes::ClientConnect);
            
            // Send client info
            std::string clientVersion = "Witcher3-MP-1.0.0";
            msg << clientVersion;
            
            client_interface<T>::MessageServer(msg);
            m_packetsSent++;
        }

        void SendDisconnectMessage()
        {
            message<T> msg;
            msg.header.id = static_cast<T>(MessageTypes::ClientDisconnect);
            client_interface<T>::MessageServer(msg);
            m_packetsSent++;
        }

        void SendPingIfNeeded()
        {
            auto now = std::chrono::high_resolution_clock::now();
            auto timeSinceLastPing = std::chrono::duration_cast<std::chrono::milliseconds>(now - m_lastPingTime).count();
            
            if (timeSinceLastPing >= 1000) // Ping every second
            {
                message<T> msg;
                msg.header.id = static_cast<T>(MessageTypes::ClientPing);
                
                // Send timestamp
                auto timestamp = std::chrono::duration_cast<std::chrono::milliseconds>(now.time_since_epoch()).count();
                msg << timestamp;
                
                client_interface<T>::MessageServer(msg);
                m_lastPingTime = now;
                m_packetsSent++;
                
                LOG_DEBUG_CAT(LogCategory::NETWORK, "Sent ping");
            }
        }

        void ProcessIncomingMessages()
        {
            while (!client_interface<T>::Incoming().empty())
            {
                auto msg = client_interface<T>::Incoming().pop_front();
                ProcessMessage(msg.msg);
                m_packetsReceived++;
            }
        }

        void ProcessMessage(const message<T>& msg)
        {
            MessageTypes msgType = static_cast<MessageTypes>(msg.header.id);
            
            switch (msgType)
            {
                case MessageTypes::ServerPong:
                    ProcessPong(msg);
                    break;
                    
                case MessageTypes::TS_SEND_PLAYERDATA:
                    ProcessPlayerData(msg);
                    break;
                    
                case MessageTypes::TS_NOTIFY_PLAYER_POS_CHANGE:
                    ProcessPositionUpdate(msg);
                    break;
                    
                case MessageTypes::TS_CHAT_MESSAGE:
                    ProcessChatMessage(msg);
                    break;
                    
                case MessageTypes::TS_HIT_NPC:
                    ProcessHitNPC(msg);
                    break;
                    
                case MessageTypes::TS_GOT_HIT:
                    ProcessGotHit(msg);
                    break;
                    
                case MessageTypes::CompressionEnabled:
                    m_compressionEnabled = true;
                    LOG_INFO_CAT(LogCategory::NETWORK, "Server enabled compression");
                    break;
                    
                case MessageTypes::CompressionDisabled:
                    m_compressionEnabled = false;
                    LOG_INFO_CAT(LogCategory::NETWORK, "Server disabled compression");
                    break;
                    
                default:
                    LOG_WARNING_CAT(LogCategory::NETWORK, "Unknown message type: " + std::to_string(static_cast<int>(msgType)));
                    break;
            }
        }

        void ProcessPong(const message<T>& msg)
        {
            auto now = std::chrono::high_resolution_clock::now();
            auto pingTime = std::chrono::duration_cast<std::chrono::milliseconds>(now - m_lastPingTime).count();
            m_ping = static_cast<float>(pingTime);
            
            LOG_DEBUG_CAT(LogCategory::NETWORK, "Received pong, ping: " + std::to_string(m_ping) + "ms");
        }

        void ProcessPlayerData(const message<T>& msg)
        {
            LOG_DEBUG_CAT(LogCategory::NETWORK, "Received player data (" + std::to_string(msg.body.size()) + " bytes)");
            // Process player data here
        }

        void ProcessPositionUpdate(const message<T>& msg)
        {
            float x, y, z, w;
            msg >> w >> z >> y >> x; // Reverse order due to stack behavior
            
            LOG_DEBUG_CAT(LogCategory::NETWORK, "Received position update: (" + 
                         std::to_string(x) + ", " + std::to_string(y) + ", " + 
                         std::to_string(z) + ", " + std::to_string(w) + ")");
        }

        void ProcessChatMessage(const message<T>& msg)
        {
            std::string chatMessage;
            msg >> chatMessage;
            
            LOG_INFO_CAT(LogCategory::NETWORK, "Received chat message: " + chatMessage);
        }

        void ProcessHitNPC(const message<T>& msg)
        {
            uint32_t npcId;
            msg >> npcId;
            
            LOG_INFO_CAT(LogCategory::NETWORK, "Received hit NPC: " + std::to_string(npcId));
        }

        void ProcessGotHit(const message<T>& msg)
        {
            LOG_INFO_CAT(LogCategory::NETWORK, "Received got hit notification");
        }

        void UpdateNetworkStats()
        {
            // Update packet loss calculation
            if (m_packetsSent > 0)
            {
                m_packetLoss = (1.0f - (float)m_packetsReceived / (float)m_packetsSent) * 100.0f;
            }
        }

        // Member variables
        bool m_connected;
        float m_ping;
        float m_packetLoss;
        bool m_compressionEnabled;
        
        std::chrono::high_resolution_clock::time_point m_connectionStartTime;
        std::chrono::high_resolution_clock::time_point m_lastPingTime;
        
        // Statistics
        size_t m_bytesSent = 0;
        size_t m_bytesReceived = 0;
        size_t m_packetsSent = 0;
        size_t m_packetsReceived = 0;
    };

    // Typedef for easy use
    using Witcher3MPClientType = Witcher3MPClient<MessageTypes>;
}