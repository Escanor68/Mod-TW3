#include "optimization/DynamicLowLatencyServer.h"
#include <iostream>

int main()
{
    std::cout << "Starting Dynamic Low Latency Server..." << std::endl;
    
    Optimization::DynamicLowLatencyServer<int> server(60001);
    
    if (server.Start())
    {
        std::cout << "Dynamic Low Latency Server started successfully!" << std::endl;
        server.Update();
    }
    else
    {
        std::cout << "Failed to start Dynamic Low Latency Server!" << std::endl;
    }
    
    return 0;
}