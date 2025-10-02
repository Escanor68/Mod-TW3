#include <iostream>
#include <string>
#include <thread>
#include <chrono>
#include "utils/Logger.h"
#include "networking/NetworkLogger.h"
#include "optimization/DataCompression.h"

int main()
{
    std::cout << "========================================" << std::endl;
    std::cout << "    Witcher3-MP Server Starting..." << std::endl;
    std::cout << "========================================" << std::endl;
    std::cout << std::endl;

    // Initialize logging
    auto& logger = Logger::GetInstance();
    logger.SetFileLogging(true, "logs/witcher3_mp.log");
    logger.SetConsoleLogging(true);
    logger.SetLogLevel(LogLevel::INFO);

    LOG_INFO("Witcher3-MP Server starting...");

    // Initialize network logging
    auto& networkLogger = NetworkLogger::GetInstance();
    networkLogger.Initialize();

    // Initialize compression
    auto& compression = DataCompression::GetInstance();
    compression.Initialize();

    LOG_INFO("All systems initialized successfully");

    // Main server loop
    std::cout << "Server is running. Press Ctrl+C to stop." << std::endl;
    std::cout << "Check logs/witcher3_mp.log for detailed information." << std::endl;
    std::cout << std::endl;

    try
    {
        while (true)
        {
            // Simulate server work
            std::this_thread::sleep_for(std::chrono::seconds(1));
            
            // Log periodic status
            static int counter = 0;
            if (++counter % 10 == 0)
            {
                LOG_INFO("Server running - " + std::to_string(counter) + " seconds");
            }
        }
    }
    catch (const std::exception& e)
    {
        LOG_ERROR("Server error: " + std::string(e.what()));
    }

    // Cleanup
    LOG_INFO("Shutting down server...");
    compression.Shutdown();
    networkLogger.Shutdown();
    logger.DestroyInstance();

    std::cout << "Server stopped." << std::endl;
    return 0;
}
