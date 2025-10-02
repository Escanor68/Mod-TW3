cmake_minimum_required(VERSION 3.16)
project(Witcher3-MP VERSION 1.0.0 LANGUAGES CXX)

# Set C++ standard
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Set build type
if(NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE Release)
endif()

# Compiler flags
if(MSVC)
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /W3 /EHsc")
    set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} /Od /Zi /RTC1")
    set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} /O2 /Ot /GL")
    add_definitions(-D_SILENCE_CXX17_CODECVT_HEADER_DEPRECATION_WARNING)
else()
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -Wextra -Wpedantic")
    set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -g -O0")
    set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} -O3 -DNDEBUG")
endif()

# Find required packages for TW3 Next-Gen
find_package(ZLIB REQUIRED)
find_package(PkgConfig REQUIRED)
pkg_check_modules(LZ4 REQUIRED liblz4)

# Add Catch2 for testing
include(FetchContent)
FetchContent_Declare(
    Catch2
    GIT_REPOSITORY https://github.com/catchorg/Catch2.git
    GIT_TAG        v3.4.0
)
FetchContent_MakeAvailable(Catch2)

# Include directories
include_directories(include)
include_directories(dep/asio)

# Source files for TW3 Next-Gen integration
set(CORE_SOURCES
    src/core/Main.cpp
    src/integration/TW3ModInterface.cpp
    src/integration/REDkitBridge.cpp
    src/integration/WitcherScriptBridge.cpp
    src/integration/AssetLoader.cpp
    src/integration/CombatSystemIntegration.cpp
    src/integration/LoggerIntegration.cpp
)

set(NETWORKING_SOURCES
    src/networking/net_server.cpp
    src/networking/net_client.cpp
    src/networking/net_connection.cpp
    src/networking/net_message.cpp
    src/networking/net_tsqueue.cpp
    src/networking/NetworkLogger.cpp
)

set(OPTIMIZATION_SOURCES
    src/optimization/DataCompression.cpp
    src/optimization/MovementPrediction.cpp
    src/optimization/MessagePrioritySystem.cpp
    src/optimization/MessagePrioritySystemImpl.cpp
    src/optimization/SmartBatching.cpp
    src/optimization/SmartBatchingImpl.cpp
    src/optimization/OptimizedNetworkProtocol.cpp
    src/optimization/PositionInterpolation.cpp
    src/optimization/NetworkOptimizer.cpp
    src/optimization/NetworkOptimizerImpl.cpp
    src/optimization/DynamicNetworkOptimizer.cpp
    src/optimization/CombatOptimizer.cpp
)

set(GAME_SOURCES
    src/game/Entities/Player/Player.cpp
    src/game/Entities/Npc/Npc.cpp
    src/game/CooperativeQuests.cpp
    src/game/GlobalEconomy.cpp
    src/game/SharedProgression.cpp
    src/game/SharedSaveSystem.cpp
    src/game/SharedStoryMode.cpp
    src/game/ExplorationMode.cpp
    src/game/SyncedMonsterAI.cpp
)

set(UTILS_SOURCES
    src/utils/Logger.cpp
    src/utils/ConfigManager.cpp
    src/utils/ConsoleCommands.cpp
    src/utils/ModConsoleCommands.cpp
    src/database/ResourceNames.cpp
)

set(VERSION_SOURCES
    src/version/VersionManager.cpp
    src/version/VersionManagerImpl.cpp
    src/version/DynamicVersionManager.cpp
    src/version/NextGenVersionManager.cpp
)

# All sources
set(ALL_SOURCES
    ${CORE_SOURCES}
    ${NETWORKING_SOURCES}
    ${OPTIMIZATION_SOURCES}
    ${GAME_SOURCES}
    ${UTILS_SOURCES}
    ${VERSION_SOURCES}
)

# Create main executable
add_executable(Witcher3-MP
    ${ALL_SOURCES}
)

# Link libraries
target_link_libraries(Witcher3-MP 
    PRIVATE 
    ZLIB::ZLIB
    ${LZ4_LIBRARIES}
)

if(WIN32)
    target_link_libraries(Witcher3-MP ws2_32)
endif()

# Include directories for target
target_include_directories(Witcher3-MP PRIVATE
    ${LZ4_INCLUDE_DIRS}
)

# Copy configuration files
add_custom_command(TARGET Witcher3-MP POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_directory
    ${CMAKE_SOURCE_DIR}/config
    $<TARGET_FILE_DIR:Witcher3-MP>/config
)

# Copy mod files
add_custom_command(TARGET Witcher3-MP POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_directory
    ${CMAKE_SOURCE_DIR}/mod
    $<TARGET_FILE_DIR:Witcher3-MP>/mod
)

# Enable testing
enable_testing()
add_subdirectory(tests)

# Add logging system test
add_executable(test_logging_system tests/test_logging_system.cpp)
target_link_libraries(test_logging_system Catch2::Catch2WithMain)
target_include_directories(test_logging_system PRIVATE include)

add_executable(test_network_connection tests/test_network_connection.cpp)
target_link_libraries(test_network_connection Catch2::Catch2WithMain)
target_include_directories(test_network_connection PRIVATE include)

# Install targets
install(TARGETS Witcher3-MP
    RUNTIME DESTINATION bin
)

install(DIRECTORY config/
    DESTINATION share/Witcher3-MP/config
)

install(DIRECTORY docs/
    DESTINATION share/Witcher3-MP/docs
)

# Create package
set(CPACK_PACKAGE_NAME "Witcher3-MP")
set(CPACK_PACKAGE_VERSION "1.0.0")
set(CPACK_PACKAGE_DESCRIPTION "The Witcher 3 Wild Hunt Multiplayer Mod")
set(CPACK_PACKAGE_VENDOR "Witcher3-MP Team")
set(CPACK_PACKAGE_CONTACT "support@witcher3-mp.com")

include(CPack)
