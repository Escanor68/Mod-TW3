#include "networking/LANDiscovery.h"
#include "utils/Logger.h"
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <cstring>
#include <chrono>
#include <sstream>

#ifdef _WIN32
#include <winsock2.h>
#include <ws2tcpip.h>
#pragma comment(lib, "ws2_32.lib")
#else
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#endif

namespace TW3Networking
{
    LANDiscovery::LANDiscovery()
        : m_isDiscovering(false)
        , m_shouldStop(false)
        , m_discoveryPort(60001)
        , m_broadcastInterval(1000)
        , m_discoveryTimeout(5000)
        , m_discoverySocket(-1)
    {
    }

    LANDiscovery::~LANDiscovery()
    {
        Shutdown();
    }

    bool LANDiscovery::Initialize()
    {
        LOG_INFO("Initializing LAN Discovery system");

#ifdef _WIN32
        WSADATA wsaData;
        if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0)
        {
            LOG_ERROR("WSAStartup failed");
            return false;
        }
#endif

        // Create UDP socket for discovery
        m_discoverySocket = socket(AF_INET, SOCK_DGRAM, 0);
        if (m_discoverySocket < 0)
        {
            LOG_ERROR("Failed to create discovery socket");
            return false;
        }

        // Set socket options
        int broadcast = 1;
        if (setsockopt(m_discoverySocket, SOL_SOCKET, SO_BROADCAST, 
                      (char*)&broadcast, sizeof(broadcast)) < 0)
        {
            LOG_ERROR("Failed to set socket broadcast option");
            close(m_discoverySocket);
            m_discoverySocket = -1;
            return false;
        }

        LOG_INFO("LAN Discovery system initialized successfully");
        return true;
    }

    void LANDiscovery::Shutdown()
    {
        StopDiscovery();

        if (m_discoverySocket >= 0)
        {
            close(m_discoverySocket);
            m_discoverySocket = -1;
        }

#ifdef _WIN32
        WSACleanup();
#endif

        LOG_INFO("LAN Discovery system shutdown");
    }

    void LANDiscovery::StartDiscovery()
    {
        if (m_isDiscovering)
        {
            LOG_WARNING("Discovery already running");
            return;
        }

        LOG_INFO("Starting LAN server discovery");
        
        m_shouldStop = false;
        m_isDiscovering = true;
        
        // Clear previous results
        {
            std::lock_guard<std::mutex> lock(m_serversMutex);
            m_discoveredServers.clear();
        }

        // Start discovery thread
        m_discoveryThread = std::thread(&LANDiscovery::DiscoveryThread, this);
    }

    void LANDiscovery::StopDiscovery()
    {
        if (!m_isDiscovering)
        {
            return;
        }

        LOG_INFO("Stopping LAN server discovery");
        
        m_shouldStop = true;
        m_isDiscovering = false;

        if (m_discoveryThread.joinable())
        {
            m_discoveryThread.join();
        }

        if (m_discoveryFinishedCallback)
        {
            m_discoveryFinishedCallback();
        }
    }

    void LANDiscovery::RefreshServers()
    {
        LOG_INFO("Refreshing server list");
        StartDiscovery();
    }

    std::vector<ServerInfo> LANDiscovery::GetDiscoveredServers() const
    {
        std::lock_guard<std::mutex> lock(m_serversMutex);
        return m_discoveredServers;
    }

    size_t LANDiscovery::GetServerCount() const
    {
        std::lock_guard<std::mutex> lock(m_serversMutex);
        return m_discoveredServers.size();
    }

    void LANDiscovery::SetServerDiscoveredCallback(ServerDiscoveredCallback callback)
    {
        m_serverDiscoveredCallback = callback;
    }

    void LANDiscovery::SetDiscoveryFinishedCallback(DiscoveryFinishedCallback callback)
    {
        m_discoveryFinishedCallback = callback;
    }

    void LANDiscovery::DiscoveryThread()
    {
        LOG_DEBUG("Discovery thread started");

        auto startTime = std::chrono::steady_clock::now();
        
        while (!m_shouldStop)
        {
            // Send discovery broadcast
            SendDiscoveryBroadcast();
            
            // Listen for responses
            ListenForResponses();
            
            // Check if discovery timeout reached
            auto currentTime = std::chrono::steady_clock::now();
            auto elapsed = std::chrono::duration_cast<std::chrono::milliseconds>(currentTime - startTime);
            
            if (elapsed.count() >= m_discoveryTimeout)
            {
                LOG_INFO("Discovery timeout reached, stopping discovery");
                break;
            }
            
            // Wait before next broadcast
            std::this_thread::sleep_for(std::chrono::milliseconds(m_broadcastInterval));
        }

        m_isDiscovering = false;
        LOG_DEBUG("Discovery thread finished");
    }

    void LANDiscovery::SendDiscoveryBroadcast()
    {
        if (m_discoverySocket < 0)
        {
            return;
        }

        // Create discovery message
        std::string message = "WITCHER3_MP_DISCOVERY_REQUEST";
        
        // Set up broadcast address
        struct sockaddr_in broadcastAddr;
        memset(&broadcastAddr, 0, sizeof(broadcastAddr));
        broadcastAddr.sin_family = AF_INET;
        broadcastAddr.sin_port = htons(m_discoveryPort);
        broadcastAddr.sin_addr.s_addr = INADDR_BROADCAST;

        // Send broadcast
        ssize_t bytesSent = sendto(m_discoverySocket, message.c_str(), message.length(), 0,
                                  (struct sockaddr*)&broadcastAddr, sizeof(broadcastAddr));
        
        if (bytesSent < 0)
        {
            LOG_ERROR("Failed to send discovery broadcast");
        }
        else
        {
            LOG_DEBUG("Discovery broadcast sent");
        }
    }

    void LANDiscovery::ListenForResponses()
    {
        if (m_discoverySocket < 0)
        {
            return;
        }

        // Set socket timeout
        struct timeval timeout;
        timeout.tv_sec = 0;
        timeout.tv_usec = 100000; // 100ms timeout
        
        if (setsockopt(m_discoverySocket, SOL_SOCKET, SO_RCVTIMEO, 
                      (char*)&timeout, sizeof(timeout)) < 0)
        {
            LOG_ERROR("Failed to set socket timeout");
            return;
        }

        char buffer[1024];
        struct sockaddr_in clientAddr;
        socklen_t clientAddrLen = sizeof(clientAddr);

        while (!m_shouldStop)
        {
            ssize_t bytesReceived = recvfrom(m_discoverySocket, buffer, sizeof(buffer) - 1, 0,
                                           (struct sockaddr*)&clientAddr, &clientAddrLen);
            
            if (bytesReceived <= 0)
            {
                // Timeout or error, break from listening loop
                break;
            }

            buffer[bytesReceived] = '\0';
            std::string response(buffer);

            // Parse server response
            ServerInfo serverInfo;
            if (ParseServerResponse(response, serverInfo))
            {
                // Set server IP from client address
                serverInfo.ip = inet_ntoa(clientAddr.sin_addr);
                serverInfo.port = ntohs(clientAddr.sin_port);

                // Calculate ping
                serverInfo.ping = CalculatePing(serverInfo.ip, serverInfo.port);

                // Add to discovered servers
                {
                    std::lock_guard<std::mutex> lock(m_serversMutex);
                    
                    // Check if server already exists
                    bool exists = false;
                    for (const auto& server : m_discoveredServers)
                    {
                        if (server.ip == serverInfo.ip && server.port == serverInfo.port)
                        {
                            exists = true;
                            break;
                        }
                    }
                    
                    if (!exists)
                    {
                        m_discoveredServers.push_back(serverInfo);
                        LOG_INFO("Discovered server: " + serverInfo.name + " at " + 
                                serverInfo.ip + ":" + std::to_string(serverInfo.port));
                        
                        if (m_serverDiscoveredCallback)
                        {
                            m_serverDiscoveredCallback(serverInfo);
                        }
                    }
                }
            }
        }
    }

    bool LANDiscovery::ParseServerResponse(const std::string& response, ServerInfo& serverInfo)
    {
        // Expected format: "WITCHER3_MP_SERVER_RESPONSE|name|port|players|maxPlayers|password"
        if (response.find("WITCHER3_MP_SERVER_RESPONSE") != 0)
        {
            return false;
        }

        std::istringstream iss(response);
        std::string token;
        std::vector<std::string> tokens;
        
        while (std::getline(iss, token, '|'))
        {
            tokens.push_back(token);
        }

        if (tokens.size() < 6)
        {
            LOG_WARNING("Invalid server response format");
            return false;
        }

        try
        {
            serverInfo.name = tokens[1];
            serverInfo.port = static_cast<uint16_t>(std::stoi(tokens[2]));
            serverInfo.playerCount = static_cast<uint8_t>(std::stoi(tokens[3]));
            serverInfo.maxPlayers = static_cast<uint8_t>(std::stoi(tokens[4]));
            serverInfo.isPasswordProtected = (tokens[5] == "true");
            
            return true;
        }
        catch (const std::exception& e)
        {
            LOG_ERROR("Failed to parse server response: " + std::string(e.what()));
            return false;
        }
    }

    uint32_t LANDiscovery::CalculatePing(const std::string& ip, uint16_t port)
    {
        // Simple ping calculation using connection time
        auto startTime = std::chrono::high_resolution_clock::now();
        
        int testSocket = socket(AF_INET, SOCK_STREAM, 0);
        if (testSocket < 0)
        {
            return 999; // High ping value for failed connection
        }

        struct sockaddr_in serverAddr;
        memset(&serverAddr, 0, sizeof(serverAddr));
        serverAddr.sin_family = AF_INET;
        serverAddr.sin_port = htons(port);
        inet_pton(AF_INET, ip.c_str(), &serverAddr.sin_addr);

        // Set connection timeout
        struct timeval timeout;
        timeout.tv_sec = 1;
        timeout.tv_usec = 0;
        setsockopt(testSocket, SOL_SOCKET, SO_RCVTIMEO, (char*)&timeout, sizeof(timeout));

        int result = connect(testSocket, (struct sockaddr*)&serverAddr, sizeof(serverAddr));
        
        auto endTime = std::chrono::high_resolution_clock::now();
        auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(endTime - startTime);
        
        close(testSocket);
        
        if (result < 0)
        {
            return 999; // High ping value for failed connection
        }
        
        return static_cast<uint32_t>(duration.count());
    }

    // IPConnection implementation
    IPConnection::IPConnection()
        : m_isConnected(false)
        , m_shouldStop(false)
        , m_serverPort(0)
        , m_connectionSocket(-1)
        , m_connectionTimeout(5000)
        , m_keepAliveInterval(30000)
    {
    }

    IPConnection::~IPConnection()
    {
        Disconnect();
    }

    bool IPConnection::Initialize()
    {
        LOG_INFO("Initializing IP Connection system");

#ifdef _WIN32
        WSADATA wsaData;
        if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0)
        {
            LOG_ERROR("WSAStartup failed");
            return false;
        }
#endif

        LOG_INFO("IP Connection system initialized successfully");
        return true;
    }

    void IPConnection::Shutdown()
    {
        Disconnect();

#ifdef _WIN32
        WSACleanup();
#endif

        LOG_INFO("IP Connection system shutdown");
    }

    bool IPConnection::ConnectToServer(const std::string& ip, uint16_t port, const std::string& username)
    {
        if (m_isConnected)
        {
            LOG_WARNING("Already connected to a server");
            return false;
        }

        LOG_INFO("Connecting to server: " + ip + ":" + std::to_string(port));

        m_serverIP = ip;
        m_serverPort = port;
        m_username = username;
        m_shouldStop = false;

        // Create connection socket
        m_connectionSocket = socket(AF_INET, SOCK_STREAM, 0);
        if (m_connectionSocket < 0)
        {
            LOG_ERROR("Failed to create connection socket");
            if (m_connectionCallback)
            {
                m_connectionCallback(false, "Failed to create socket");
            }
            return false;
        }

        // Set up server address
        struct sockaddr_in serverAddr;
        memset(&serverAddr, 0, sizeof(serverAddr));
        serverAddr.sin_family = AF_INET;
        serverAddr.sin_port = htons(port);
        inet_pton(AF_INET, ip.c_str(), &serverAddr.sin_addr);

        // Attempt connection
        int result = connect(m_connectionSocket, (struct sockaddr*)&serverAddr, sizeof(serverAddr));
        if (result < 0)
        {
            LOG_ERROR("Failed to connect to server");
            close(m_connectionSocket);
            m_connectionSocket = -1;
            
            if (m_connectionCallback)
            {
                m_connectionCallback(false, "Connection failed");
            }
            return false;
        }

        // Send connection request
        if (!SendConnectionRequest())
        {
            LOG_ERROR("Failed to send connection request");
            close(m_connectionSocket);
            m_connectionSocket = -1;
            
            if (m_connectionCallback)
            {
                m_connectionCallback(false, "Failed to send connection request");
            }
            return false;
        }

        // Start connection thread
        m_connectionThread = std::thread(&IPConnection::ConnectionThread, this);
        
        m_isConnected = true;
        LOG_INFO("Successfully connected to server");
        
        if (m_connectionCallback)
        {
            m_connectionCallback(true, "Connected successfully");
        }
        
        return true;
    }

    void IPConnection::Disconnect()
    {
        if (!m_isConnected)
        {
            return;
        }

        LOG_INFO("Disconnecting from server");
        
        m_shouldStop = true;
        m_isConnected = false;

        if (m_connectionSocket >= 0)
        {
            close(m_connectionSocket);
            m_connectionSocket = -1;
        }

        if (m_connectionThread.joinable())
        {
            m_connectionThread.join();
        }

        LOG_INFO("Disconnected from server");
    }

    bool IPConnection::SendData(const std::string& data)
    {
        if (!m_isConnected || m_connectionSocket < 0)
        {
            LOG_WARNING("Not connected to server");
            return false;
        }

        ssize_t bytesSent = send(m_connectionSocket, data.c_str(), data.length(), 0);
        if (bytesSent < 0)
        {
            LOG_ERROR("Failed to send data to server");
            return false;
        }

        LOG_DEBUG("Sent data to server: " + std::to_string(bytesSent) + " bytes");
        return true;
    }

    std::string IPConnection::ReceiveData()
    {
        if (!m_isConnected || m_connectionSocket < 0)
        {
            return "";
        }

        char buffer[4096];
        ssize_t bytesReceived = recv(m_connectionSocket, buffer, sizeof(buffer) - 1, 0);
        
        if (bytesReceived <= 0)
        {
            if (bytesReceived == 0)
            {
                LOG_INFO("Server disconnected");
                Disconnect();
            }
            return "";
        }

        buffer[bytesReceived] = '\0';
        return std::string(buffer);
    }

    void IPConnection::SetConnectionCallback(ConnectionCallback callback)
    {
        m_connectionCallback = callback;
    }

    void IPConnection::SetDataReceivedCallback(DataReceivedCallback callback)
    {
        m_dataReceivedCallback = callback;
    }

    void IPConnection::ConnectionThread()
    {
        LOG_DEBUG("Connection thread started");

        while (!m_shouldStop && m_isConnected)
        {
            std::string data = ReceiveData();
            if (!data.empty() && m_dataReceivedCallback)
            {
                m_dataReceivedCallback(data);
            }
            
            std::this_thread::sleep_for(std::chrono::milliseconds(10));
        }

        LOG_DEBUG("Connection thread finished");
    }

    bool IPConnection::SendConnectionRequest()
    {
        std::string request = "WITCHER3_MP_CONNECT|" + m_username + "|" + std::to_string(m_serverPort);
        return SendData(request);
    }

    bool IPConnection::HandleServerResponse(const std::string& response)
    {
        if (response.find("WITCHER3_MP_CONNECT_OK") == 0)
        {
            LOG_INFO("Server accepted connection");
            return true;
        }
        else if (response.find("WITCHER3_MP_CONNECT_ERROR") == 0)
        {
            LOG_ERROR("Server rejected connection: " + response);
            return false;
        }
        
        return false;
    }
}
