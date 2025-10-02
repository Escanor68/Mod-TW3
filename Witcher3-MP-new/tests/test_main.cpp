#define CATCH_CONFIG_MAIN
#include <catch2/catch_test_macros.hpp>
#include <catch2/catch_session.hpp>

// Main test runner for Witcher3-MP-NextGen
int main(int argc, char* argv[])
{
    Catch::Session session;
    
    int returnCode = session.applyCommandLine(argc, argv);
    if (returnCode != 0)
        return returnCode;
    
    return session.run();
}
