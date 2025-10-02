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

# Include directories
include_directories(include)
include_directories(dep/asio)

# Source files
set(CORE_SOURCES
    src/core/Main.cpp
    src/core/LowLatencyMain.cpp
    src/core/DynamicMain.cpp
)

set(NETWORKING_SOURCES
    src/networking/net_server.cpp
    src/networking/net_client.cpp
    src/networking/net_connection.cpp
    src/networking/net_message.cpp
    src/networking/net_tsqueue.cpp
)

set(OPTIMIZATION_SOURCES
    src/optimization/NetworkOptimizer.cpp
    src/optimization/LowLatencyServer.cpp
    src/optimization/DynamicLowLatencyServer.cpp
    src/optimization/DynamicNetworkOptimizer.cpp
)

set(VERSION_SOURCES
    src/version/VersionManager.cpp
    src/version/DynamicVersionManager.cpp
)

set(UTILS_SOURCES
    src/utils/Logger.cpp
    src/utils/ConfigManager.cpp
)

set(GAME_SOURCES
    src/game/Entities/Player/Player.cpp
    src/game/Entities/Npc/Npc.cpp
)

# Header files
set(HEADERS
    include/core/Main.h
    include/networking/net_server.h
    include/networking/net_client.h
    include/networking/net_connection.h
    include/networking/net_message.h
    include/networking/net_tsqueue.h
    include/networking/PacketTypes.h
    include/optimization/NetworkOptimizer.h
    include/optimization/LowLatencyServer.h
    include/optimization/DynamicLowLatencyServer.h
    include/optimization/DynamicNetworkOptimizer.h
    include/version/VersionManager.h
    include/version/DynamicVersionManager.h
    include/utils/Logger.h
    include/utils/ConfigManager.h
    include/utils/Common.h
    include/game/Entities/Player/Player.h
    include/game/Entities/Npc/Npc.h
)

# All sources
set(ALL_SOURCES
    ${CORE_SOURCES}
    ${NETWORKING_SOURCES}
    ${OPTIMIZATION_SOURCES}
    ${VERSION_SOURCES}
    ${UTILS_SOURCES}
    ${GAME_SOURCES}
)

# Create executables
add_executable(Witcher3-MP-Standard
    src/core/Main.cpp
    ${NETWORKING_SOURCES}
    ${UTILS_SOURCES}
    ${GAME_SOURCES}
)

add_executable(Witcher3-MP-LowLatency
    src/core/LowLatencyMain.cpp
    ${NETWORKING_SOURCES}
    ${OPTIMIZATION_SOURCES}
    ${UTILS_SOURCES}
    ${GAME_SOURCES}
)

add_executable(Witcher3-MP-Dynamic
    src/core/DynamicMain.cpp
    ${NETWORKING_SOURCES}
    ${OPTIMIZATION_SOURCES}
    ${VERSION_SOURCES}
    ${UTILS_SOURCES}
    ${GAME_SOURCES}
)

# Link libraries
if(WIN32)
    target_link_libraries(Witcher3-MP-Standard ws2_32)
    target_link_libraries(Witcher3-MP-LowLatency ws2_32)
    target_link_libraries(Witcher3-MP-Dynamic ws2_32)
endif()

# Copy configuration files
add_custom_command(TARGET Witcher3-MP-Standard POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_directory
    ${CMAKE_SOURCE_DIR}/config
    $<TARGET_FILE_DIR:Witcher3-MP-Standard>/config
)

add_custom_command(TARGET Witcher3-MP-LowLatency POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_directory
    ${CMAKE_SOURCE_DIR}/config
    $<TARGET_FILE_DIR:Witcher3-MP-LowLatency>/config
)

add_custom_command(TARGET Witcher3-MP-Dynamic POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_directory
    ${CMAKE_SOURCE_DIR}/config
    $<TARGET_FILE_DIR:Witcher3-MP-Dynamic>/config
)

# Install targets
install(TARGETS Witcher3-MP-Standard Witcher3-MP-LowLatency Witcher3-MP-Dynamic
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
set(CPACK_PACKAGE_DESCRIPTION "The Witcher 3 Multiplayer Server")
set(CPACK_PACKAGE_VENDOR "Witcher3-MP Team")

include(CPack)
