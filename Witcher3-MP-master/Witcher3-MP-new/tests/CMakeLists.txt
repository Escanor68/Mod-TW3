# Tests configuration for Witcher3-MP-NextGen
cmake_minimum_required(VERSION 3.16)

# Find Catch2
find_package(Catch2 3 REQUIRED)

# Test sources
set(TEST_SOURCES
    test_main.cpp
    test_tw3_integration.cpp
    test_asset_loading.cpp
    test_inventory.cpp
    test_quest_system.cpp
    test_network_optimization.cpp
)

# Create test executable
add_executable(Witcher3-MP-Tests ${TEST_SOURCES})

# Link libraries
target_link_libraries(Witcher3-MP-Tests 
    PRIVATE Catch2::Catch2WithMain
)

# Include directories
target_include_directories(Witcher3-MP-Tests PRIVATE
    ${CMAKE_SOURCE_DIR}/include
    ${CMAKE_SOURCE_DIR}/../Witcher3-MP/include
)

# Add test to CTest
include(CTest)
include(Catch)
catch_discover_tests(Witcher3-MP-Tests)
