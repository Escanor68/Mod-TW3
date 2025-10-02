#include "optimization/LowLatencyServer.h"
#include <iostream>

int main()
{
    std::cout << "Starting Low Latency Server..." << std::endl;
    
    Optimization::LowLatencyServer<int> server(60000);
    
    if (server.Start())
    {
        std::cout << "Low Latency Server started successfully!" << std::endl;
        server.Update();
    }
    else
    {
        std::cout << "Failed to start Low Latency Server!" << std::endl;
    }
    
    return 0;
}