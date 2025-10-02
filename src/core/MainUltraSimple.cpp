#include <iostream>
#include <string>
#include <thread>
#include <chrono>

int main()
{
    std::cout << "========================================" << std::endl;
    std::cout << "    Witcher3-MP Server Starting..." << std::endl;
    std::cout << "========================================" << std::endl;
    std::cout << std::endl;

    std::cout << "Server is running. Press Ctrl+C to stop." << std::endl;
    std::cout << "This is a test version of Witcher3-MP Server." << std::endl;
    std::cout << std::endl;

    try
    {
        int counter = 0;
        while (true)
        {
            // Simulate server work
            std::this_thread::sleep_for(std::chrono::seconds(1));
            
            // Log periodic status
            if (++counter % 10 == 0)
            {
                std::cout << "Server running - " << counter << " seconds" << std::endl;
            }
        }
    }
    catch (const std::exception& e)
    {
        std::cout << "Server error: " << e.what() << std::endl;
    }

    std::cout << "Server stopped." << std::endl;
    return 0;
}
