# Witcher3-MP Mod Installation Instructions

## Prerequisites

### Required Software
1. **The Witcher 3: Wild Hunt Next-Gen** (v4.0 or higher)
2. **REDkit** (Official modding tool from CD Projekt RED)
3. **Mod Manager** (Vortex or Witcher 3 Mod Manager)
4. **Script Merger** (for compatibility with other mods)

### System Requirements
- Windows 10/11 (64-bit)
- 8GB RAM minimum, 16GB recommended
- Stable internet connection for multiplayer
- Visual Studio 2019+ (for development)

## Installation Steps

### Step 1: Install REDkit
1. Download REDkit from [Steam Store](https://store.steampowered.com/app/2557220/REDkit/) or [Official Website](https://www.thewitcher.com/redkit)
2. Install REDkit following the official instructions
3. Verify installation by opening REDkit editor

### Step 2: Install Mod Manager
1. Download and install [Vortex Mod Manager](https://www.nexusmods.com/site/mods/1) or [Witcher 3 Mod Manager](https://www.nexusmods.com/witcher3/mods/2100)
2. Configure mod manager for The Witcher 3
3. Enable mod support in game settings

### Step 3: Install Witcher3-MP Mod
1. Copy the `mod` folder to your Witcher 3 installation directory:
   ```
   The Witcher 3 Wild Hunt/mod/
   ```
2. Copy the `Witcher3-MP.exe` server executable to a separate folder
3. Run the mod manager and enable the Witcher3-MP mod

### Step 4: Configure the Mod
1. Edit `mod/config/mp_config.json` to configure server settings
2. Set your preferred server port (default: 60000)
3. Configure maximum players (default: 8)
4. Set synchronization options as needed

### Step 5: Start the Server
1. Run `Witcher3-MP.exe` to start the multiplayer server
2. The server will start on the configured port
3. Check the console for any error messages

### Step 6: Test the Mod
1. Launch The Witcher 3: Wild Hunt Next-Gen
2. Load a save game or start a new game
3. The mod should initialize automatically
4. Check the in-game UI for server status

## Configuration Options

### Server Configuration
- `server.port`: Server port (default: 60000)
- `server.max_players`: Maximum number of players (default: 8)
- `server.name`: Server name (default: "Witcher3-MP Server")
- `server.auto_start`: Auto-start server on game launch (default: false)

### Network Configuration
- `network.tick_rate`: Server tick rate (default: 60)
- `network.compression_enabled`: Enable packet compression (default: true)
- `network.encryption_enabled`: Enable packet encryption (default: false)
- `network.timeout_seconds`: Connection timeout (default: 30)

### Game Synchronization
- `game.sync_players`: Synchronize player positions (default: true)
- `game.sync_npcs`: Synchronize NPCs (default: true)
- `game.sync_items`: Synchronize items (default: true)
- `game.sync_quests`: Synchronize quests (default: false)
- `game.sync_weather`: Synchronize weather (default: true)
- `game.sync_time`: Synchronize time (default: true)
- `game.sync_combat`: Synchronize combat (default: true)
- `game.sync_magic`: Synchronize magic (default: true)

### Optimization Settings
- `optimization.low_latency_mode`: Enable low latency mode (default: false)
- `optimization.dynamic_optimization`: Enable dynamic optimization (default: true)
- `optimization.prediction_enabled`: Enable client prediction (default: true)
- `optimization.interpolation_enabled`: Enable position interpolation (default: true)

## Troubleshooting

### Common Issues

#### Mod Not Loading
- Check that REDkit is properly installed
- Verify mod files are in the correct directory
- Check mod manager load order
- Run Script Merger to resolve conflicts

#### Server Not Starting
- Check if port 60000 is available
- Verify firewall settings
- Check server logs for error messages
- Try changing the server port

#### Connection Issues
- Verify server is running
- Check network connectivity
- Verify port forwarding (if needed)
- Check firewall settings

#### Game Crashes
- Disable other mods temporarily
- Check for script conflicts
- Verify game version compatibility
- Check mod installation

### Log Files
- Server logs: `logs/witcher3_mp.log`
- Game logs: Check game installation directory
- Mod manager logs: Check mod manager directory

## Development

### For Modders
1. Install REDkit and development tools
2. Open the mod project in REDkit
3. Modify scripts in `mod/scripts/game/witcher3_mp/`
4. Test changes in-game
5. Compile and package the mod

### For Server Administrators
1. Configure server settings in `mp_config.json`
2. Set up port forwarding if needed
3. Configure firewall rules
4. Monitor server logs
5. Manage player connections

## Support

### Getting Help
- Check the [GitHub Issues](https://github.com/Witcher3-MP/Witcher3-MP/issues)
- Join the [Discord Server](https://discord.gg/witcher3mp)
- Visit the [Nexus Mods Page](https://www.nexusmods.com/witcher3/mods/witcher3mp)

### Reporting Bugs
1. Check existing issues first
2. Provide detailed error messages
3. Include system specifications
4. Attach relevant log files
5. Describe steps to reproduce

## Legal Notice

This mod is created for educational and entertainment purposes. It is not affiliated with CD Projekt RED. Please respect the game's End User License Agreement (EULA) and use responsibly.

## Changelog

### Version 1.0.0
- Initial release
- Basic multiplayer functionality
- Player synchronization
- NPC management
- Network optimization
- REDkit integration
