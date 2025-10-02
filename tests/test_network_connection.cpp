#include <catch2/catch_test_macros.hpp>
#include "networking/net_client.h"
#include "networking/net_server.h"
#include "networking/MessageTypes.h"
#include "networking/NetworkLogger.h"
#include "optimization/DataCompression.h"
#include "optimization/NetworkOptimizer.h"
#include "utils/Logger.h"
#include <thread>
#include <chrono>
#include <iostream>

TEST_CASE("Network Connection Test", "[network]")
{
    // Initialize logging
    auto& logger = Logger::GetInstance();
    logger.SetFileLogging(true, "logs/test_network.log");
    logger.SetConsoleLogging(false);
    logger.SetLogLevel(LogLevel::DEBUG);

    // Initialize network logger
    auto& networkLogger = NetworkLogger::GetInstance();
    networkLogger.Initialize();

    // Initialize compression
    auto& compression = Optimization::DataCompression::GetInstance();
    compression.Initialize();

    SECTION("Server Creation and Startup")
    {
        // Create server on port 7777
        Networking::Witcher3MPServerType server(7777);
        
        // Start server in separate thread
        std::thread serverThread([&server]() {
            server.Start();
            // Keep server running for test
            std::this_thread::sleep_for(std::chrono::seconds(5));
        });

        // Give server time to start
        std::this_thread::sleep_for(std::chrono::milliseconds(100));

        // Verify server is running
        REQUIRE(server.GetConnectedClients() == 0);
        REQUIRE(server.GetMaxClients() == 100);

        // Clean up
        server.Stop();
        if (serverThread.joinable())
        {
            serverThread.join();
        }
    }

    SECTION("Client Connection Test")
    {
        // Create server
        Networking::Witcher3MPServerType server(7777);
        
        // Start server
        std::thread serverThread([&server]() {
            server.Start();
            std::this_thread::sleep_for(std::chrono::seconds(3));
        });

        // Give server time to start
        std::this_thread::sleep_for(std::chrono::milliseconds(100));

        // Create client
        Networking::Witcher3MPClientType client;
        
        // Connect to server
        bool connected = client.Connect("localhost", 7777);
        REQUIRE(connected);

        // Give time for connection to establish
        std::this_thread::sleep_for(std::chrono::milliseconds(100));

        // Verify connection
        REQUIRE(client.IsConnected());

        // Test sending messages
        client.SendPositionUpdate(1.0f, 2.0f, 3.0f, 4.0f);
        client.SendChatMessage("Hello from test client!");

        // Give time for messages to be processed
        std::this_thread::sleep_for(std::chrono::milliseconds(100));

        // Disconnect
        client.Disconnect();
        REQUIRE_FALSE(client.IsConnected());

        // Clean up
        server.Stop();
        if (serverThread.joinable())
        {
            serverThread.join();
        }
    }

    SECTION("Message Types Alignment Test")
    {
        // Test that MessageTypes are properly aligned
        REQUIRE(static_cast<int>(Networking::MessageTypes::ClientConnect) == 1);
        REQUIRE(static_cast<int>(Networking::MessageTypes::ClientDisconnect) == 2);
        REQUIRE(static_cast<int>(Networking::MessageTypes::ClientPing) == 3);
        REQUIRE(static_cast<int>(Networking::MessageTypes::ServerPong) == 4);
        
        REQUIRE(static_cast<int>(Networking::MessageTypes::TC_REQUEST_PLAYERDATA) == 200);
        REQUIRE(static_cast<int>(Networking::MessageTypes::TC_UPDATE_POS) == 201);
        REQUIRE(static_cast<int>(Networking::MessageTypes::TS_SEND_PLAYERDATA) == 202);
        
        REQUIRE(static_cast<int>(Networking::MessageTypes::CompressionEnabled) == 300);
        REQUIRE(static_cast<int>(Networking::MessageTypes::CompressionDisabled) == 301);
    }

    SECTION("Compression Integration Test")
    {
        // Test compression integration
        auto& optimizer = Optimization::NetworkOptimizer::GetInstance();
        
        // Create test data
        std::vector<uint8_t> testData = {1, 2, 3, 4, 5, 6, 7, 8, 9, 10};
        
        // Test compression
        auto compressed = optimizer.ApplyCompression(testData);
        REQUIRE_FALSE(compressed.empty());
        
        // Test decompression
        auto decompressed = optimizer.ApplyDecompression(compressed);
        REQUIRE(decompressed.size() == testData.size());
        
        // Verify data integrity
        for (size_t i = 0; i < testData.size(); ++i)
        {
            REQUIRE(decompressed[i] == testData[i]);
        }
    }

    SECTION("Network Logger Test")
    {
        // Test network logging
        networkLogger.LogConnection("test_client", "connected", "localhost:7777");
        networkLogger.LogMessage("TC_UPDATE_POS", "SENT", 16, "test_client");
        networkLogger.LogLatency("test_client", 25.5f);
        networkLogger.LogPacketLoss("test_client", 0.1f);
        networkLogger.LogCompression("TC_UPDATE_POS", 100, 80, 0.8f);
        networkLogger.LogNetworkEvent("test_event", "Test network event", "test_client");
        
        // Flush logs
        networkLogger.FlushLogs();
        
        // Verify statistics
        auto stats = networkLogger.GetStatistics();
        REQUIRE(stats.packetsSent >= 0);
        REQUIRE(stats.packetsReceived >= 0);
    }

    SECTION("Localhost Connection Test")
    {
        // Create server on localhost:7777
        Networking::Witcher3MPServerType server(7777);
        
        // Start server
        std::thread serverThread([&server]() {
            server.Start();
            std::this_thread::sleep_for(std::chrono::seconds(2));
        });

        // Give server time to start
        std::this_thread::sleep_for(std::chrono::milliseconds(100));

        // Create two clients
        Networking::Witcher3MPClientType client1;
        Networking::Witcher3MPClientType client2;

        // Connect both clients
        bool client1Connected = client1.Connect("localhost", 7777);
        bool client2Connected = client2.Connect("localhost", 7777);
        
        REQUIRE(client1Connected);
        REQUIRE(client2Connected);

        // Give time for connections to establish
        std::this_thread::sleep_for(std::chrono::milliseconds(100));

        // Verify both clients are connected
        REQUIRE(client1.IsConnected());
        REQUIRE(client2.IsConnected());

        // Test communication between clients
        client1.SendChatMessage("Hello from client 1!");
        client2.SendChatMessage("Hello from client 2!");

        // Test position updates
        client1.SendPositionUpdate(1.0f, 2.0f, 3.0f, 4.0f);
        client2.SendPositionUpdate(5.0f, 6.0f, 7.0f, 8.0f);

        // Give time for messages to be processed
        std::this_thread::sleep_for(std::chrono::milliseconds(100));

        // Disconnect clients
        client1.Disconnect();
        client2.Disconnect();

        // Clean up
        server.Stop();
        if (serverThread.joinable())
        {
            serverThread.join();
        }
    }

    SECTION("Compression Performance Test")
    {
        // Test compression performance
        auto& optimizer = Optimization::NetworkOptimizer::GetInstance();
        
        // Create large test data
        std::vector<uint8_t> largeData(10000);
        for (size_t i = 0; i < largeData.size(); ++i)
        {
            largeData[i] = static_cast<uint8_t>(i % 256);
        }
        
        // Measure compression time
        auto start = std::chrono::high_resolution_clock::now();
        auto compressed = optimizer.ApplyCompression(largeData);
        auto end = std::chrono::high_resolution_clock::now();
        
        auto compressionTime = std::chrono::duration_cast<std::chrono::microseconds>(end - start).count();
        
        // Compression should be fast (< 1ms for 10KB)
        REQUIRE(compressionTime < 1000);
        
        // Test decompression
        start = std::chrono::high_resolution_clock::now();
        auto decompressed = optimizer.ApplyDecompression(compressed);
        end = std::chrono::high_resolution_clock::now();
        
        auto decompressionTime = std::chrono::duration_cast<std::chrono::microseconds>(end - start).count();
        
        // Decompression should be fast (< 500μs)
        REQUIRE(decompressionTime < 500);
        
        // Verify data integrity
        REQUIRE(decompressed.size() == largeData.size());
        for (size_t i = 0; i < largeData.size(); ++i)
        {
            REQUIRE(decompressed[i] == largeData[i]);
        }
    }

    // Clean up
    networkLogger.Shutdown();
    compression.Shutdown();
    logger.DestroyInstance();
}
