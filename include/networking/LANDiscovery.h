#pragma once

#include "Common.h"
#include <vector>
#include <string>
#include <thread>
#include <atomic>
#include <mutex>
#include <functional>

namespace TW3Networking
{
    // Server information structure
    struct ServerInfo
    {
        std::string name;
        std::string ip;
        uint16_t port;
        uint8_t playerCount;
        uint8_t maxPlayers;
        uint32_t ping;
        bool isPasswordProtected;
        
        ServerInfo() : port(0), playerCount(0), maxPlayers(0), ping(0), isPasswordProtected(false) {}
    };

    // Callback for server discovery
    using ServerDiscoveredCallback = std::function<void(const ServerInfo&)>;
    using DiscoveryFinishedCallback = std::function<void()>;

    // LAN Discovery class
    class LANDiscovery
    {
    public:
        LANDiscovery();
        ~LANDiscovery();

        // Initialize discovery system
        bool Initialize();
        
        // Shutdown discovery system
        void Shutdown();

        // Start discovering servers on LAN
        void StartDiscovery();
        
        // Stop discovery
        void StopDiscovery();
        
        // Refresh server list
        void RefreshServers();

        // Get discovered servers
        std::vector<ServerInfo> GetDiscoveredServers() const;
        
        // Get server count
        size_t GetServerCount() const;

        // Set callbacks
        void SetServerDiscoveredCallback(ServerDiscoveredCallback callback);
        void SetDiscoveryFinishedCallback(DiscoveryFinishedCallback callback);

        // Check if discovery is running
        bool IsDiscovering() const { return m_isDiscovering; }

    private:
        // Discovery thread function
        void DiscoveryThread();
        
        // Send discovery broadcast
        void SendDiscoveryBroadcast();
        
        // Listen for server responses
        void ListenForResponses();
        
        // Parse server response
        bool ParseServerResponse(const std::string& response, ServerInfo& serverInfo);
        
        // Calculate ping to server
        uint32_t CalculatePing(const std::string& ip, uint16_t port);

        // Member variables
        std::atomic<bool> m_isDiscovering;
        std::atomic<bool> m_shouldStop;
        std::thread m_discoveryThread;
        
        mutable std::mutex m_serversMutex;
        std::vector<ServerInfo> m_discoveredServers;
        
        // Callbacks
        ServerDiscoveredCallback m_serverDiscoveredCallback;
        DiscoveryFinishedCallback m_discoveryFinishedCallback;
        
        // Network settings
        uint16_t m_discoveryPort;
        uint32_t m_broadcastInterval;
        uint32_t m_discoveryTimeout;
        
        // Socket for discovery
        int m_discoverySocket;
    };

    // IP Connection class for direct IP connections
    class IPConnection
    {
    public:
        IPConnection();
        ~IPConnection();

        // Initialize connection system
        bool Initialize();
        
        // Shutdown connection system
        void Shutdown();

        // Connect to server by IP
        bool ConnectToServer(const std::string& ip, uint16_t port, const std::string& username = "Player");
        
        // Disconnect from server
        void Disconnect();
        
        // Check if connected
        bool IsConnected() const { return m_isConnected; }
        
        // Get connection info
        std::string GetServerIP() const { return m_serverIP; }
        uint16_t GetServerPort() const { return m_serverPort; }
        std::string GetUsername() const { return m_username; }

        // Send data to server
        bool SendData(const std::string& data);
        
        // Receive data from server
        std::string ReceiveData();

        // Set connection callbacks
        using ConnectionCallback = std::function<void(bool success, const std::string& message)>;
        using DataReceivedCallback = std::function<void(const std::string& data)>;
        
        void SetConnectionCallback(ConnectionCallback callback);
        void SetDataReceivedCallback(DataReceivedCallback callback);

    private:
        // Connection thread
        void ConnectionThread();
        
        // Send connection request
        bool SendConnectionRequest();
        
        // Handle server response
        bool HandleServerResponse(const std::string& response);

        // Member variables
        std::atomic<bool> m_isConnected;
        std::atomic<bool> m_shouldStop;
        std::thread m_connectionThread;
        
        std::string m_serverIP;
        uint16_t m_serverPort;
        std::string m_username;
        
        int m_connectionSocket;
        
        // Callbacks
        ConnectionCallback m_connectionCallback;
        DataReceivedCallback m_dataReceivedCallback;
        
        // Connection settings
        uint32_t m_connectionTimeout;
        uint32_t m_keepAliveInterval;
    };
}
