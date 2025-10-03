#include "optimization/NextGenServer.h"
#include "utils/Logger.h"
#include <iostream>
#include <thread>
#include <chrono>

int main()
{
    std::cout << "Starting Next-Gen Server..." << std::endl;
    
    Optimization::NextGenServer server(60002);
    
    if (server.Start())
    {
        std::cout << "Next-Gen Server started successfully!" << std::endl;
        std::cout << "Server running on port 60002. Press Ctrl+C to stop." << std::endl;
        
        // Mantener el servidor corriendo
        while (true)
        {
            server.Update();
            std::this_thread::sleep_for(std::chrono::milliseconds(100));
        }
    }
    else
    {
        std::cout << "Failed to start Next-Gen Server!" << std::endl;
    }
    
    return 0;
}