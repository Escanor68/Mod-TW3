#include "optimization/NextGenServer.h"
#include "utils/Logger.h"
#include <iostream>

int main()
{
    std::cout << "Starting Next-Gen Server..." << std::endl;
    
    Optimization::NextGenServer server(60002);
    
    if (server.Start())
    {
        std::cout << "Next-Gen Server started successfully!" << std::endl;
        server.Update();
    }
    else
    {
        std::cout << "Failed to start Next-Gen Server!" << std::endl;
    }
    
    return 0;
}