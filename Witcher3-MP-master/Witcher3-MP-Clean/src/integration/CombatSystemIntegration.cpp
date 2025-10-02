#include "integration/CombatSystemIntegration.h"
#include <algorithm>
#include <sstream>

namespace TW3Integration
{
    // Static member initialization
    CombatSystemIntegration* CombatSystemIntegration::s_instance = nullptr;

    CombatSystemIntegration::CombatSystemIntegration()
        : m_redkitBridge(nullptr), m_witcherScriptBridge(nullptr), m_combatOptimizer(nullptr),
          m_initialized(false), m_processing(false), m_running(false),
          m_shouldStop(false), m_processedActions(0), m_droppedActions(0),
          m_processingInterval(16), m_maxProcessingTime(10), m_maxQueueSize(1000)
    {
        m_lastProcessTime = std::chrono::steady_clock::now();
    }

    CombatSystemIntegration::~CombatSystemIntegration()
    {
        Shutdown();
    }

    CombatSystemIntegration& CombatSystemIntegration::GetInstance()
    {
        if (!s_instance)
        {
            s_instance = new CombatSystemIntegration();
        }
        return *s_instance;
    }

    void CombatSystemIntegration::DestroyInstance()
    {
        if (s_instance)
        {
            delete s_instance;
            s_instance = nullptr;
        }
    }

    bool CombatSystemIntegration::Initialize(uint32_t processingInterval, uint32_t maxProcessingTime, uint32_t maxQueueSize)
    {
        if (m_initialized)
        {
            return true;
        }

        m_processingInterval = processingInterval;
        m_maxProcessingTime = maxProcessingTime;
        m_maxQueueSize = maxQueueSize;

        LOG_INFO("Initializing CombatSystemIntegration...");

        // Initialize core systems
        if (!InitializeSystems())
        {
            LOG_ERROR("Failed to initialize core systems");
            return false;
        }

        m_initialized = true;
        LOG_INFO("CombatSystemIntegration initialized successfully");
        
        return true;
    }

    bool CombatSystemIntegration::Shutdown()
    {
        if (!m_initialized)
        {
            return true;
        }

        LOG_INFO("Shutting down CombatSystemIntegration...");

        // Stop processing
        StopProcessing();

        // Shutdown systems
        ShutdownSystems();

        m_initialized = false;
        LOG_INFO("CombatSystemIntegration shutdown complete");
        
        return true;
    }

    bool CombatSystemIntegration::InitializeSystems()
    {
        // Initialize REDkitBridge
        m_redkitBridge = &REDkitBridge::GetInstance();
        if (!m_redkitBridge->Initialize())
        {
            LOG_ERROR("Failed to initialize REDkitBridge");
            return false;
        }

        // Initialize WitcherScriptBridge
        m_witcherScriptBridge = &WitcherScriptBridge::GetInstance();
        if (!m_witcherScriptBridge->Initialize())
        {
            LOG_ERROR("Failed to initialize WitcherScriptBridge");
            return false;
        }

        // Initialize CombatOptimizer
        m_combatOptimizer = &TW3Optimization::CombatOptimizer::GetInstance();
        if (!m_combatOptimizer->Initialize(m_maxQueueSize, 50, m_maxProcessingTime))
        {
            LOG_ERROR("Failed to initialize CombatOptimizer");
            return false;
        }

        return true;
    }

    bool CombatSystemIntegration::ShutdownSystems()
    {
        bool success = true;

        if (m_combatOptimizer)
        {
            if (!m_combatOptimizer->Shutdown())
            {
                LOG_ERROR("Failed to shutdown CombatOptimizer");
                success = false;
            }
        }

        if (m_witcherScriptBridge)
        {
            if (!m_witcherScriptBridge->Shutdown())
            {
                LOG_ERROR("Failed to shutdown WitcherScriptBridge");
                success = false;
            }
        }

        if (m_redkitBridge)
        {
            if (!m_redkitBridge->Shutdown())
            {
                LOG_ERROR("Failed to shutdown REDkitBridge");
                success = false;
            }
        }

        return success;
    }

    bool CombatSystemIntegration::ExecuteAttack(uint32_t playerId, uint32_t attackType, uint32_t weaponType, const Vector4F& targetPosition)
    {
        if (!m_initialized)
        {
            LOG_ERROR("CombatSystemIntegration not initialized");
            return false;
        }

        // Add attack action to optimizer
        bool result = m_combatOptimizer->AddAttackAction(playerId, attackType, weaponType, targetPosition);
        
        if (result)
        {
            LOG_DEBUG("Added attack action for player " + std::to_string(playerId));
        }
        else
        {
            m_droppedActions++;
            LOG_WARNING("Failed to add attack action for player " + std::to_string(playerId) + " - queue full");
        }

        return result;
    }

    bool CombatSystemIntegration::ExecuteDefense(uint32_t playerId, uint32_t defenseType, uint32_t incomingAttackId)
    {
        if (!m_initialized)
        {
            LOG_ERROR("CombatSystemIntegration not initialized");
            return false;
        }

        // Add defense action to optimizer
        bool result = m_combatOptimizer->AddDefenseAction(playerId, defenseType, incomingAttackId);
        
        if (result)
        {
            LOG_DEBUG("Added defense action for player " + std::to_string(playerId));
        }
        else
        {
            m_droppedActions++;
            LOG_WARNING("Failed to add defense action for player " + std::to_string(playerId) + " - queue full");
        }

        return result;
    }

    bool CombatSystemIntegration::CastSign(uint32_t playerId, uint32_t signType, const Vector4F& targetPosition)
    {
        if (!m_initialized)
        {
            LOG_ERROR("CombatSystemIntegration not initialized");
            return false;
        }

        // Add sign action to optimizer
        bool result = m_combatOptimizer->AddSignAction(playerId, signType, targetPosition);
        
        if (result)
        {
            LOG_DEBUG("Added sign action for player " + std::to_string(playerId));
        }
        else
        {
            m_droppedActions++;
            LOG_WARNING("Failed to add sign action for player " + std::to_string(playerId) + " - queue full");
        }

        return result;
    }

    bool CombatSystemIntegration::PlayAnimation(uint32_t playerId, uint32_t animationType, bool isAttack)
    {
        if (!m_initialized)
        {
            LOG_ERROR("CombatSystemIntegration not initialized");
            return false;
        }

        // Add animation action to optimizer
        bool result = m_combatOptimizer->AddAnimationAction(playerId, animationType, isAttack);
        
        if (result)
        {
            LOG_DEBUG("Added animation action for player " + std::to_string(playerId));
        }
        else
        {
            m_droppedActions++;
            LOG_WARNING("Failed to add animation action for player " + std::to_string(playerId) + " - queue full");
        }

        return result;
    }

    bool CombatSystemIntegration::UpdatePlayerPosition(uint32_t playerId, const Vector4F& newPosition, const Vector4F& velocity)
    {
        if (!m_initialized)
        {
            LOG_ERROR("CombatSystemIntegration not initialized");
            return false;
        }

        // Add movement action to optimizer
        bool result = m_combatOptimizer->AddMovementAction(playerId, newPosition, velocity);
        
        if (result)
        {
            LOG_DEBUG("Added movement action for player " + std::to_string(playerId));
        }
        else
        {
            m_droppedActions++;
            LOG_WARNING("Failed to add movement action for player " + std::to_string(playerId) + " - queue full");
        }

        return result;
    }

    bool CombatSystemIntegration::ExecuteCombatSequence(uint32_t playerId, const std::vector<CombatAction>& actions)
    {
        if (!m_initialized)
        {
            LOG_ERROR("CombatSystemIntegration not initialized");
            return false;
        }

        bool allSuccess = true;
        
        for (const auto& action : actions)
        {
            bool result = false;
            
            switch (action.type)
            {
                case CombatAction::Attack:
                {
                    if (action.data.size() >= sizeof(uint32_t) * 3 + sizeof(Vector4F))
                    {
                        const uint8_t* ptr = action.data.data();
                        uint32_t attackType = *reinterpret_cast<const uint32_t*>(ptr);
                        ptr += sizeof(uint32_t);
                        uint32_t weaponType = *reinterpret_cast<const uint32_t*>(ptr);
                        ptr += sizeof(uint32_t);
                        Vector4F targetPosition = *reinterpret_cast<const Vector4F*>(ptr);
                        
                        result = ExecuteAttack(playerId, attackType, weaponType, targetPosition);
                    }
                    break;
                }
                
                case CombatAction::Defense:
                {
                    if (action.data.size() >= sizeof(uint32_t) * 2)
                    {
                        const uint8_t* ptr = action.data.data();
                        uint32_t defenseType = *reinterpret_cast<const uint32_t*>(ptr);
                        ptr += sizeof(uint32_t);
                        uint32_t incomingAttackId = *reinterpret_cast<const uint32_t*>(ptr);
                        
                        result = ExecuteDefense(playerId, defenseType, incomingAttackId);
                    }
                    break;
                }
                
                case CombatAction::Sign:
                {
                    if (action.data.size() >= sizeof(uint32_t) + sizeof(Vector4F))
                    {
                        const uint8_t* ptr = action.data.data();
                        uint32_t signType = *reinterpret_cast<const uint32_t*>(ptr);
                        ptr += sizeof(uint32_t);
                        Vector4F targetPosition = *reinterpret_cast<const Vector4F*>(ptr);
                        
                        result = CastSign(playerId, signType, targetPosition);
                    }
                    break;
                }
                
                case CombatAction::Animation:
                {
                    if (action.data.size() >= sizeof(uint32_t) + sizeof(bool))
                    {
                        const uint8_t* ptr = action.data.data();
                        uint32_t animationType = *reinterpret_cast<const uint32_t*>(ptr);
                        ptr += sizeof(uint32_t);
                        bool isAttack = *reinterpret_cast<const bool*>(ptr);
                        
                        result = PlayAnimation(playerId, animationType, isAttack);
                    }
                    break;
                }
                
                case CombatAction::Movement:
                {
                    if (action.data.size() >= sizeof(Vector4F) * 2)
                    {
                        const uint8_t* ptr = action.data.data();
                        Vector4F newPosition = *reinterpret_cast<const Vector4F*>(ptr);
                        ptr += sizeof(Vector4F);
                        Vector4F velocity = *reinterpret_cast<const Vector4F*>(ptr);
                        
                        result = UpdatePlayerPosition(playerId, newPosition, velocity);
                    }
                    break;
                }
            }
            
            if (!result)
            {
                allSuccess = false;
            }
        }
        
        return allSuccess;
    }

    bool CombatSystemIntegration::ProcessPlayerInput(uint32_t playerId, const std::vector<uint8_t>& inputData)
    {
        if (!m_initialized)
        {
            LOG_ERROR("CombatSystemIntegration not initialized");
            return false;
        }

        // Parse input data and create appropriate actions
        // This is a simplified implementation
        if (inputData.size() < sizeof(uint32_t))
        {
            return false;
        }

        uint32_t actionType = *reinterpret_cast<const uint32_t*>(inputData.data());
        
        switch (actionType)
        {
            case 0: // Attack
                if (inputData.size() >= sizeof(uint32_t) * 3 + sizeof(Vector4F))
                {
                    const uint8_t* ptr = inputData.data() + sizeof(uint32_t);
                    uint32_t attackType = *reinterpret_cast<const uint32_t*>(ptr);
                    ptr += sizeof(uint32_t);
                    uint32_t weaponType = *reinterpret_cast<const uint32_t*>(ptr);
                    ptr += sizeof(uint32_t);
                    Vector4F targetPosition = *reinterpret_cast<const Vector4F*>(ptr);
                    
                    return ExecuteAttack(playerId, attackType, weaponType, targetPosition);
                }
                break;
                
            case 1: // Defense
                if (inputData.size() >= sizeof(uint32_t) * 3)
                {
                    const uint8_t* ptr = inputData.data() + sizeof(uint32_t);
                    uint32_t defenseType = *reinterpret_cast<const uint32_t*>(ptr);
                    ptr += sizeof(uint32_t);
                    uint32_t incomingAttackId = *reinterpret_cast<const uint32_t*>(ptr);
                    
                    return ExecuteDefense(playerId, defenseType, incomingAttackId);
                }
                break;
                
            case 2: // Sign
                if (inputData.size() >= sizeof(uint32_t) * 2 + sizeof(Vector4F))
                {
                    const uint8_t* ptr = inputData.data() + sizeof(uint32_t);
                    uint32_t signType = *reinterpret_cast<const uint32_t*>(ptr);
                    ptr += sizeof(uint32_t);
                    Vector4F targetPosition = *reinterpret_cast<const Vector4F*>(ptr);
                    
                    return CastSign(playerId, signType, targetPosition);
                }
                break;
        }
        
        return false;
    }

    bool CombatSystemIntegration::StartProcessing()
    {
        if (!m_initialized)
        {
            LOG_ERROR("CombatSystemIntegration not initialized");
            return false;
        }

        if (m_running)
        {
            LOG_WARNING("Processing already running");
            return true;
        }

        m_shouldStop = false;
        m_running = true;
        
        // Start processing thread
        m_processingThread = std::thread(&CombatSystemIntegration::ProcessingLoop, this);
        
        LOG_INFO("Combat processing started");
        return true;
    }

    bool CombatSystemIntegration::StopProcessing()
    {
        if (!m_running)
        {
            return true;
        }

        m_shouldStop = true;
        m_running = false;
        
        // Wait for processing thread to finish
        if (m_processingThread.joinable())
        {
            m_processingThread.join();
        }
        
        LOG_INFO("Combat processing stopped");
        return true;
    }

    bool CombatSystemIntegration::PauseProcessing()
    {
        if (!m_running)
        {
            return false;
        }

        m_processing = false;
        LOG_INFO("Combat processing paused");
        return true;
    }

    bool CombatSystemIntegration::ResumeProcessing()
    {
        if (!m_running)
        {
            return false;
        }

        m_processing = true;
        LOG_INFO("Combat processing resumed");
        return true;
    }

    void CombatSystemIntegration::ProcessingLoop()
    {
        LOG_INFO("Combat processing loop started");
        
        while (!m_shouldStop)
        {
            if (m_processing)
            {
                ProcessCombatActions();
            }
            
            // Sleep for processing interval
            std::this_thread::sleep_for(std::chrono::milliseconds(m_processingInterval));
        }
        
        LOG_INFO("Combat processing loop stopped");
    }

    bool CombatSystemIntegration::ProcessCombatActions()
    {
        if (!m_combatOptimizer)
        {
            return false;
        }

        auto startTime = std::chrono::steady_clock::now();
        
        // Process actions through optimizer
        bool result = m_combatOptimizer->ProcessActions();
        
        auto endTime = std::chrono::steady_clock::now();
        auto processingTime = std::chrono::duration_cast<std::chrono::milliseconds>(endTime - startTime).count();
        
        // Update performance metrics
        UpdatePerformanceMetrics();
        
        if (result)
        {
            m_processedActions++;
        }
        
        return result;
    }

    void CombatSystemIntegration::UpdatePerformanceMetrics()
    {
        m_lastProcessTime = std::chrono::steady_clock::now();
        
        // Update dropped actions count
        if (m_combatOptimizer)
        {
            auto stats = m_combatOptimizer->GetStatistics();
            m_droppedActions = stats.droppedActions.load();
        }
    }

    float CombatSystemIntegration::GetProcessingLoad() const
    {
        if (!m_combatOptimizer)
        {
            return 0.0f;
        }
        
        return m_combatOptimizer->GetProcessingLoad();
    }

    uint64_t CombatSystemIntegration::GetProcessedActions() const
    {
        return m_processedActions;
    }

    uint64_t CombatSystemIntegration::GetDroppedActions() const
    {
        return m_droppedActions;
    }

    uint64_t CombatSystemIntegration::GetAverageProcessingTime() const
    {
        if (!m_combatOptimizer)
        {
            return 0;
        }
        
        return m_combatOptimizer->GetAverageProcessingTime();
    }

    bool CombatSystemIntegration::IsOverloaded() const
    {
        if (!m_combatOptimizer)
        {
            return false;
        }
        
        return m_combatOptimizer->IsOverloaded();
    }

    void CombatSystemIntegration::PrintStatistics() const
    {
        LOG_INFO("=== Combat System Integration Statistics ===");
        LOG_INFO("Processed Actions: " + std::to_string(m_processedActions));
        LOG_INFO("Dropped Actions: " + std::to_string(m_droppedActions));
        LOG_INFO("Processing Load: " + std::to_string(GetProcessingLoad() * 100.0f) + "%");
        LOG_INFO("Average Processing Time: " + std::to_string(GetAverageProcessingTime()) + "ms");
        LOG_INFO("Is Overloaded: " + std::string(IsOverloaded() ? "Yes" : "No"));
        
        if (m_combatOptimizer)
        {
            m_combatOptimizer->PrintStatistics();
        }
    }

    void CombatSystemIntegration::ResetStatistics()
    {
        m_processedActions = 0;
        m_droppedActions = 0;
        
        if (m_combatOptimizer)
        {
            m_combatOptimizer->ResetStatistics();
        }
    }

    std::string CombatSystemIntegration::GetStatusReport() const
    {
        std::stringstream ss;
        ss << "Combat System Status:\n";
        ss << "  Initialized: " << (m_initialized ? "Yes" : "No") << "\n";
        ss << "  Running: " << (m_running ? "Yes" : "No") << "\n";
        ss << "  Processing: " << (m_processing ? "Yes" : "No") << "\n";
        ss << "  Processed Actions: " << m_processedActions << "\n";
        ss << "  Dropped Actions: " << m_droppedActions << "\n";
        ss << "  Processing Load: " << (GetProcessingLoad() * 100.0f) << "%\n";
        ss << "  Overloaded: " << (IsOverloaded() ? "Yes" : "No") << "\n";
        
        if (m_combatOptimizer)
        {
            ss << "  " << m_combatOptimizer->GetStatusString() << "\n";
        }
        
        return ss.str();
    }

    void CombatSystemIntegration::SetProcessingInterval(uint32_t interval)
    {
        m_processingInterval = interval;
    }

    void CombatSystemIntegration::SetMaxProcessingTime(uint32_t maxTime)
    {
        m_maxProcessingTime = maxTime;
    }

    void CombatSystemIntegration::SetMaxQueueSize(uint32_t maxSize)
    {
        m_maxQueueSize = maxSize;
    }

    bool CombatSystemIntegration::IsHealthy() const
    {
        if (!m_initialized)
        {
            return false;
        }
        
        if (IsOverloaded())
        {
            return false;
        }
        
        return true;
    }

    bool CombatSystemIntegration::IsInitialized() const
    {
        return m_initialized;
    }

    bool CombatSystemIntegration::IsProcessing() const
    {
        return m_processing;
    }

    std::string CombatSystemIntegration::GetSystemInfo() const
    {
        std::stringstream ss;
        ss << "Combat System Integration Info:\n";
        ss << "  Processing Interval: " << m_processingInterval << "ms\n";
        ss << "  Max Processing Time: " << m_maxProcessingTime << "ms\n";
        ss << "  Max Queue Size: " << m_maxQueueSize << "\n";
        ss << "  REDkit Bridge: " << (m_redkitBridge ? "Available" : "Not Available") << "\n";
        ss << "  WitcherScript Bridge: " << (m_witcherScriptBridge ? "Available" : "Not Available") << "\n";
        ss << "  Combat Optimizer: " << (m_combatOptimizer ? "Available" : "Not Available") << "\n";
        
        return ss.str();
    }

    void CombatSystemIntegration::ForceProcessActions()
    {
        if (m_combatOptimizer)
        {
            m_combatOptimizer->ProcessActions();
        }
    }

    // CombatSystemFactory Implementation
    std::unique_ptr<CombatSystemIntegration> CombatSystemFactory::CreateCombatSystem(const CombatSystemConfig& config)
    {
        auto system = std::make_unique<CombatSystemIntegration>();
        
        if (!system->Initialize(config.processingInterval, config.maxProcessingTime, config.maxQueueSize))
        {
            return nullptr;
        }
        
        return system;
    }

    CombatSystemConfig CombatSystemFactory::GetDefaultConfig()
    {
        return CombatSystemConfig();
    }

    CombatSystemConfig CombatSystemFactory::GetHighPerformanceConfig()
    {
        CombatSystemConfig config;
        config.processingInterval = 8;  // 120 FPS
        config.maxProcessingTime = 5;   // 5ms max
        config.maxQueueSize = 2000;     // Larger queue
        config.batchSize = 100;         // Larger batches
        return config;
    }

    CombatSystemConfig CombatSystemFactory::GetLowLatencyConfig()
    {
        CombatSystemConfig config;
        config.processingInterval = 4;  // 250 FPS
        config.maxProcessingTime = 2;   // 2ms max
        config.maxQueueSize = 500;      // Smaller queue
        config.batchSize = 25;          // Smaller batches
        return config;
    }

    // CombatSystemValidator Implementation
    bool CombatSystemValidator::ValidateConfiguration(const CombatSystemConfig& config)
    {
        return config.processingInterval > 0 && 
               config.maxProcessingTime > 0 && 
               config.maxQueueSize > 0 && 
               config.batchSize > 0;
    }

    bool CombatSystemValidator::ValidateSystemHealth(const CombatSystemIntegration& system)
    {
        return system.IsHealthy() && 
               system.IsInitialized() && 
               !system.IsOverloaded();
    }

    std::vector<std::string> CombatSystemValidator::GetValidationErrors(const CombatSystemConfig& config)
    {
        std::vector<std::string> errors;
        
        if (config.processingInterval == 0)
        {
            errors.push_back("Processing interval must be greater than 0");
        }
        
        if (config.maxProcessingTime == 0)
        {
            errors.push_back("Max processing time must be greater than 0");
        }
        
        if (config.maxQueueSize == 0)
        {
            errors.push_back("Max queue size must be greater than 0");
        }
        
        if (config.batchSize == 0)
        {
            errors.push_back("Batch size must be greater than 0");
        }
        
        return errors;
    }

    std::vector<std::string> CombatSystemValidator::GetHealthIssues(const CombatSystemIntegration& system)
    {
        std::vector<std::string> issues;
        
        if (!system.IsInitialized())
        {
            issues.push_back("System not initialized");
        }
        
        if (system.IsOverloaded())
        {
            issues.push_back("System overloaded");
        }
        
        if (system.GetProcessingLoad() > 0.9f)
        {
            issues.push_back("High processing load: " + std::to_string(system.GetProcessingLoad() * 100.0f) + "%");
        }
        
        return issues;
    }
}
