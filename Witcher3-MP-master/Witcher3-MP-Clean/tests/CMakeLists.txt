# Tests configuration for Witcher3-MP-NextGen
cmake_minimum_required(VERSION 3.16)

# Find Catch2
find_package(Catch2 3 REQUIRED)

# Test sources
set(TEST_SOURCES
    test_bridges.cpp
    test_combat_system.cpp
    test_compression.cpp
    test_witcherscript.cpp
)

# Source files needed for tests
set(PROJECT_SOURCES
    ${CMAKE_SOURCE_DIR}/src/integration/REDkitBridge.cpp
    ${CMAKE_SOURCE_DIR}/src/integration/WitcherScriptBridge.cpp
    ${CMAKE_SOURCE_DIR}/src/integration/AssetLoader.cpp
    ${CMAKE_SOURCE_DIR}/src/integration/CombatSystemIntegration.cpp
    ${CMAKE_SOURCE_DIR}/src/optimization/CombatOptimizer.cpp
    ${CMAKE_SOURCE_DIR}/src/optimization/DataCompression.cpp
    ${CMAKE_SOURCE_DIR}/src/utils/Logger.cpp
    ${CMAKE_SOURCE_DIR}/src/utils/ConfigManager.cpp
    ${CMAKE_SOURCE_DIR}/src/game/Entities/Player/Player.cpp
    ${CMAKE_SOURCE_DIR}/src/game/Entities/Npc/Npc.cpp
)

# Create test executable
add_executable(Witcher3-MP-Tests ${TEST_SOURCES} ${PROJECT_SOURCES})

# Link libraries
target_link_libraries(Witcher3-MP-Tests 
    PRIVATE Catch2::Catch2WithMain
    ZLIB::ZLIB
    ${LZ4_LIBRARIES}
)

# Include directories
target_include_directories(Witcher3-MP-Tests PRIVATE
    ${CMAKE_SOURCE_DIR}/include
    ${CMAKE_SOURCE_DIR}/../Witcher3-MP/include
    ${LZ4_INCLUDE_DIRS}
)

# Add test to CTest
include(CTest)
include(Catch)
catch_discover_tests(Witcher3-MP-Tests)
