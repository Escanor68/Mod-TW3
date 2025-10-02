#include "networking/net_message.h"
#include "networking/net_tsqueue.h"
#include "networking/net_connection.h"
#include "networking/net_client.h"
#include "networking/net_server.h"
#include "networking/PacketTypes.h"

#include "Common.h"
#include <vector>
#include <map>
#include <sstream>
#include <fstream>

#include "game/Entities/Player/Player.h"
#include "game/Entities/Npc/Npc.h"
#include "database/ResourceNames.h"

// New systems for version management and configuration
#include "version/VersionManager.h"
#include "version/DynamicVersionManager.h"
#include "utils/ConfigManager.h"
#include "utils/Logger.h"

// TW3 Next-Gen integration
#include "integration/TW3ModInterface.h"

uint32 highestPlayerID = 1;
std::vector<Player*> PlayerList;

uint32 highestNpcID = 1;
std::vector<Npc*> NpcList;

class Witcher3MPServer : public Networking::server_interface<Networking::MessageTypes>
{
public:
	Witcher3MPServer(uint16_t nPort) : Networking::server_interface<Networking::MessageTypes>(nPort)
	{}

protected:
	virtual bool OnClientConnect(std::shared_ptr<Networking::connection<Networking::MessageTypes>> client)
	{
		Networking::message<Networking::MessageTypes> msg;
		msg.header.id = Networking::MessageTypes::TC_REQUEST_PLAYERDATA;
		MessageClient(client, msg);
		return true;
	}

	virtual void OnClientDisconnect(std::shared_ptr<Networking::connection<Networking::MessageTypes>> client)
	{
		std::cout << "Client disconnected [" << client->GetID() << "]\n";

		for (uint8 i = 0; i < PlayerList.size(); ++i)
		{
			Player* ply = PlayerList[i];

			if (ply != nullptr)
			{
				if (ply->ownerClient == client)
				{
					PlayerList[i] = nullptr;	// I will delete the element from the vector next update

					Networking::message<Networking::MessageTypes> msg; // temporary solution: sending a position update to cords 0,0,0 cause I am lazy to create an entity destroy message xD
					msg.header.id = Networking::MessageTypes::TC_UPDATE_POS;
					uint32 id = ply->GetID();
					Vector4F pos;
					uint8 movetype = 1;
					msg << id << pos << movetype;
					for (auto i : PlayerList)
					{
						if (i == nullptr)
							continue;
						MessageClient(i->ownerClient, msg);
					}

					std::cout << "Kicking Player: " << id << std::endl;
					delete PlayerList[i];
				}
			}
		}
	}

	virtual void OnMessageReceived(std::shared_ptr<Networking::connection<Networking::MessageTypes>> client, Networking::message<Networking::MessageTypes>& msg)
	{
		switch (msg.header.id)
		{
			case Networking::MessageTypes::TS_SEND_PLAYERDATA:
			{
				Vector4F recPosition;
				uint8 characterId;
				msg >> characterId >> recPosition;

				uint32 newPlyID = highestPlayerID++;

				Networking::message<Networking::MessageTypes> msg;
				msg.header.id = Networking::MessageTypes::TC_CREATE_PLAYER;
				msg << newPlyID << recPosition << characterId;

				for (auto i : PlayerList)
				{
					if (i->ownerClient != client)
						MessageClient(i->ownerClient, msg);
				}

				if (PlayerList.size())
				{
					Networking::message<Networking::MessageTypes> massCreate;
					massCreate.header.id = Networking::MessageTypes::TC_MASS_CREATE_PLAYER;

					for (auto i : PlayerList)
					{
						uint32 ID = i->GetID();
						Vector4F pos = i->GetPosition();
						
						massCreate << ID << pos << i->characterId;
					}

					MessageClient(client, massCreate);
				}

				Player* newPly = new Player(newPlyID, recPosition, 1000.f, characterId, client);
				PlayerList.push_back(newPly);

				std::cout << "New Player created with ID: " + std::to_string(newPlyID) << std::endl;

				Networking::message<Networking::MessageTypes> setHealth;
				setHealth.header.id = Networking::MessageTypes::TC_SET_ACTOR_HEALTH;
				uint32 setHealth_id = 0;
				bool setHealth_isPlayer = true;
				float setHealth_currhealthvalue = 1000;
				float setHealth_maxhealthvalue = 1000;
				setHealth << setHealth_id << setHealth_isPlayer << setHealth_currhealthvalue << setHealth_maxhealthvalue;
				MessageClient(client, setHealth);
				break;
			}
			case Networking::MessageTypes::TS_NOTIFY_PLAYER_POS_CHANGE:
			{
				Player* affected = nullptr;
				for (Player* i : PlayerList)
					if (i->ownerClient == client)
					{
						affected = i;
						break;
					}

				if (affected != nullptr)
				{
					Vector4F newPos;
					uint8 MoveType;
					msg >> MoveType >> newPos;

					affected->UpdatePosition(newPos);

					Networking::message<Networking::MessageTypes> updatePos;
					updatePos.header.id = Networking::MessageTypes::TC_UPDATE_POS;
					uint32 playerId = affected->GetID();
					updatePos << playerId << newPos << MoveType;

					MessageAllClients(updatePos, affected->ownerClient);
				}
				break;
			}
			case Networking::MessageTypes::TS_HIT_NPC:
			{
				uint32 Id;
				msg >> Id;

				for (auto i : NpcList)
				{
					if (i->GetID() == Id)
					{
						float damage = 50.f;

						if (i->GetHealth() > damage)
						{
							i->SetHealth(i->GetHealth() - damage);

							Networking::message<Networking::MessageTypes> healthMsg;
							healthMsg.header.id = Networking::MessageTypes::TC_SET_ACTOR_HEALTH;

							uint32 healthMsg_id = Id;
							bool healthMsg_isPlayer = false;
							float healthMsg_currhealthvalue = i->GetHealth();
							float healthMsg_maxhealthvalue = i->GetMaxHealth(); 
							healthMsg << healthMsg_id << healthMsg_isPlayer << healthMsg_currhealthvalue << healthMsg_maxhealthvalue;

							for (auto j : PlayerList)
								MessageClient(j->ownerClient, healthMsg);
						}
						else
						{
							i->SetHealth(0.f);

							Networking::message<Networking::MessageTypes> death_msg;
							death_msg.header.id = Networking::MessageTypes::TC_NPC_DEAD;
							death_msg << Id;

							for (auto j : PlayerList)
								MessageClient(j->ownerClient, death_msg);
						}
						break;
					}
				}
				break;
			}
			case Networking::MessageTypes::TS_GOT_HIT:
			{
				for (auto i : PlayerList)
				{
					if (i->ownerClient == client)
					{
						float damage = 100.f;

						if (i->GetHealth() > damage)
						{
							i->SetHealth(i->GetHealth() - damage);

							Networking::message<Networking::MessageTypes> healthMsg;
							healthMsg.header.id = Networking::MessageTypes::TC_SET_ACTOR_HEALTH;

							uint32 healthMsg_id = 0;
							bool healthMsg_isPlayer = true;
							float healthMsg_currhealthvalue = i->GetHealth();
							float healthMsg_maxhealthvalue = i->GetMaxHealth();
							healthMsg << healthMsg_id << healthMsg_isPlayer << healthMsg_currhealthvalue << healthMsg_maxhealthvalue;

							MessageClient(client, healthMsg);
						}
						else
						{
							i->SetHealth(i->GetMaxHealth());

							Networking::message<Networking::MessageTypes> msg;
							msg.header.id = Networking::MessageTypes::TC_PLAYER_DEAD;
							MessageClient(client, msg);
						}
						break;
					}
				}
				break;
			}
			case Networking::MessageTypes::TS_CHAT_MESSAGE:
			{
				for (auto i : PlayerList)
				{
					if (i->ownerClient == client)
					{
						std::string chat_message;
						for (uint8 i = 0; i < 100; ++i)
						{
							if (msg.size())
							{
								char ch;
								msg >> ch;
								chat_message += ch;
							}
						}

						if (chat_message.length() == 0)
							break;

						size_t length = chat_message.length();
						for (size_t i = 0; i < length / 2; i++)
							std::swap(chat_message[i], chat_message[length - i - 1]);

						Networking::message<Networking::MessageTypes> msg;
						msg.header.id = Networking::MessageTypes::TC_CHAT_MESSAGE;
						uint32 playerId = i->GetID();
						msg << playerId;
						for (auto i : chat_message)
							msg << i;

						for(auto i : PlayerList)
							if(i->ownerClient != client)
								MessageClient(i->ownerClient, msg);


						Networking::message<Networking::MessageTypes> msg2;
						msg2.header.id = Networking::MessageTypes::TC_CHAT_MESSAGE;
						uint32 playerId2 = 0;
						msg2 << playerId2;
						for (auto i : chat_message)
							msg2 << i;
						playerId = 0;
						MessageClient(client, msg2);
						break;
					}
				}
				break;
			}
		}
	}
};

Witcher3MPServer* w3server;

std::vector<std::string> commandQueue;

void receive_commands()
{
	while (true)
	{
		std::string command = "";
		std::getline(std::cin, command);

		commandQueue.push_back(command);
	}
}

void handle_commands()
{
	if (!commandQueue.size())
		return;

	for (uint8 i = 0; i < commandQueue.size(); ++i)
	{
		std::string command = commandQueue[i];

		if (command == "")
			continue;

		std::stringstream stream(command);
		std::string segment;
		std::vector<std::string> segments;

		while (std::getline(stream, segment, ' '))
			segments.push_back(segment);

		// spawn: resourcename, toPlayerID
		if (segments[0] == "spawn" && segments.size() == 3)
		{
			if (uint32 ResID = Database::ResourceNames::FindResID(segments[1]))
			{
				uint32 toPlayerID = std::atoi(segments[2].c_str());

				Vector4F SpawnTo;

				for (auto i : PlayerList)
				{
					if (i->GetID() == toPlayerID)
					{
						SpawnTo = i->GetPosition();
						break;
					}
				}

				if (!SpawnTo.null())
				{
					uint32 newNpcID = highestNpcID++;
					float newNpcHealth = 500.f;
					if (ResID == 243)
						newNpcHealth = 3000.f;
					if (ResID == 244)
						newNpcHealth = 5000.f;
					if (ResID == 242)
						newNpcHealth = 10000.f;
					Npc* newNpc = new Npc(newNpcID, ResID, SpawnTo, newNpcHealth);
					NpcList.push_back(newNpc);

					std::cout << "NPC Spawned" << std::endl;

					Networking::message<Networking::MessageTypes> msg;
					msg.header.id = Networking::MessageTypes::TC_CREATE_NPC;
					msg << newNpcID << ResID << SpawnTo << newNpcHealth;
					w3server->MessageAllClients(msg);
				}
			}
		}

		commandQueue[i] = "";
	}

	commandQueue.erase(std::remove(commandQueue.begin(),
		commandQueue.end(), ""), commandQueue.end());
}

int main()
{
	system("Color 03");
	setlocale(LC_ALL, "");

	// Initialize logging system first
	Logger& logger = Logger::GetInstance();
	logger.SetFileLogging(true, "witcher3mp.log");
	logger.SetConsoleLogging(true);
	logger.SetLogLevel(1); // INFO level

	LOG_INFO("Starting Witcher3-MP Server...");

	std::cout << R"(
 __        ___ _       _               ____                   
 \ \      / (_) |_ ___| |__   ___ _ __/ ___| _   _ _ __   ___ 
  \ \ /\ / /| | __/ __| '_ \ / _ \ '__\___ \| | | | '_ \ / __|
   \ V  V / | | || (__| | | |  __/ |   ___) | |_| | | | | (__ 
    \_/\_/  |_|\__\___|_| |_|\___|_|  |____/ \__, |_| |_|\___|
                                             |___/                
                 2022.02 - 04, Developer: Hyjarion
                                                      v0.0.1  )" << "\n";

	// Initialize configuration manager
	auto& configManager = ConfigManager::GetInstance();
	if (!configManager.LoadConfig())
	{
		LOG_WARNING("Could not load config, using defaults");
		configManager.SaveConfig(); // Save default config
	}

	// Validate configuration
	if (!configManager.ValidateConfig())
	{
		LOG_WARNING("Configuration validation failed, some settings may be incorrect");
	}

	// Print configuration
	configManager.PrintConfig();

	// Initialize version manager
	Version::DynamicVersionManager versionManager("1.0.0");
	if (!versionManager.CheckVersion())
	{
		LOG_ERROR("Failed to check version");
		return 1;
	}

	// Initialize TW3 Next-Gen integration
	auto& tw3Interface = TW3Integration::TW3ModInterface::GetInstance();
	if (!tw3Interface.Initialize())
	{
		LOG_ERROR("Failed to initialize TW3 integration");
		return 1;
	}
	
	// Set up callbacks for game events
	tw3Interface.SetPlayerMoveCallback([](uint32_t playerId, float x, float y, float z, float w) {
		LOG_DEBUG("Player " + std::to_string(playerId) + " moved to (" + 
			std::to_string(x) + ", " + std::to_string(y) + ", " + std::to_string(z) + ")");
	});
	
	tw3Interface.SetPlayerHealthCallback([](uint32_t playerId, float health) {
		LOG_DEBUG("Player " + std::to_string(playerId) + " health: " + std::to_string(health));
	});
	
	tw3Interface.SetModConnected(true);
	LOG_INFO("TW3 Next-Gen integration initialized successfully");

	// Check game compatibility
	if (!versionManager.IsCompatible("1.0.0"))
	{
		LOG_WARNING("Game version compatibility check failed");
		LOG_INFO("Server will continue but may not work properly");
	}

	// Print version information
	LOG_INFO("Current version: " + versionManager.GetVersion());

	// Check if game was recently updated
	if (versionManager.UpdateVersion())
	{
		LOG_WARNING("Game appears to have been recently updated");
		LOG_INFO("Please verify mod compatibility with the new game version");
	}

	// Get port from configuration
	uint16_t port = configManager.GetPort();
	if (port == 0)
	{
		LOG_ERROR("Invalid port configuration: " + std::to_string(port));
		return 1;
	}

	LOG_INFO("Starting server on port: " + std::to_string(port));

	// Create and start server
	w3server = new Witcher3MPServer(port);
	if (!w3server->Start())
	{
		LOG_ERROR("Failed to start server");
		delete w3server;
		return 1;
	}

	LOG_INFO("Server started successfully");

	// Start command processor thread
	std::thread commandProcessor(receive_commands);
	LOG_INFO("Command processor started");

	// Main server loop
	LOG_INFO("Entering main server loop");
	while (true)
	{
		try
		{
			handle_commands();

			// Clean up disconnected players
			if (PlayerList.size())
			{
				PlayerList.erase(std::remove(PlayerList.begin(),
				PlayerList.end(), nullptr), PlayerList.end());
			}

			// Update server
			w3server->Update(-1, false);

			// Check for game updates periodically (every 1000 iterations)
			static int updateCheckCounter = 0;
			if (++updateCheckCounter >= 1000)
			{
				updateCheckCounter = 0;
				if (versionManager.UpdateVersion())
				{
					LOG_WARNING("Game update detected during runtime");
				}
			}
		}
		catch (const std::exception& e)
		{
			LOG_ERROR("Exception in main loop: " + std::string(e.what()));
		}
		catch (...)
		{
			LOG_ERROR("Unknown exception in main loop");
		}
	}

	// Cleanup
	LOG_INFO("Shutting down server...");
	commandProcessor.join();
	delete w3server;
	Logger::DestroyInstance();

	return 0;
}