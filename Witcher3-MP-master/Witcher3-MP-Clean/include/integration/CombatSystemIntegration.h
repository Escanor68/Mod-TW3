#pragma once

#include "optimization/CombatOptimizer.h"
#include "integration/REDkitBridge.h"
#include "integration/WitcherScriptBridge.h"
#include "game/Entities/Player.h"
#include "utils/Logger.h"
#include <memory>
#include <thread>
#include <atomic>
#include <chrono>

namespace TW3Integration
{
    // Combat system integration class
    class CombatSystemIntegration
    {
    private:
        static CombatSystemIntegration* s_instance;
        
        // Core systems
        REDkitBridge* m_redkitBridge;
        WitcherScriptBridge* m_witcherScriptBridge;
        TW3Optimization::CombatOptimizer* m_combatOptimizer;
        
        // Processing state
        std::atomic<bool> m_initialized;
        std::atomic<bool> m_processing;
        std::atomic<bool> m_running;
        
        // Processing thread
        std::thread m_processingThread;
        std::atomic<bool> m_shouldStop;
        
        // Performance monitoring
        std::chrono::steady_clock::time_point m_lastProcessTime;
        uint64_t m_processedActions;
        uint64_t m_droppedActions;
        
        // Configuration
        uint32_t m_processingInterval; // milliseconds
        uint32_t m_maxProcessingTime;  // milliseconds
        uint32_t m_maxQueueSize;
        
        CombatSystemIntegration();
        ~CombatSystemIntegration();
        
        // Processing methods
        void ProcessingLoop();
        bool ProcessCombatActions();
        void UpdatePerformanceMetrics();
        
        // Integration methods
        bool InitializeSystems();
        bool ShutdownSystems();
        bool ValidateSystemHealth();
        
    public:
        // Singleton pattern
        static CombatSystemIntegration& GetInstance();
        static void DestroyInstance();
        
        // Initialization and shutdown
        bool Initialize(uint32_t processingInterval = 16, uint32_t maxProcessingTime = 10, uint32_t maxQueueSize = 1000);
        bool Shutdown();
        
        // Combat action management
        bool ExecuteAttack(uint32_t playerId, uint32_t attackType, uint32_t weaponType, const Vector4F& targetPosition);
        bool ExecuteDefense(uint32_t playerId, uint32_t defenseType, uint32_t incomingAttackId);
        bool CastSign(uint32_t playerId, uint32_t signType, const Vector4F& targetPosition);
        bool PlayAnimation(uint32_t playerId, uint32_t animationType, bool isAttack);
        bool UpdatePlayerPosition(uint32_t playerId, const Vector4F& newPosition, const Vector4F& velocity);
        
        // Batch operations
        bool ExecuteCombatSequence(uint32_t playerId, const std::vector<CombatAction>& actions);
        bool ProcessPlayerInput(uint32_t playerId, const std::vector<uint8_t>& inputData);
        
        // System management
        bool StartProcessing();
        bool StopProcessing();
        bool PauseProcessing();
        bool ResumeProcessing();
        
        // Performance monitoring
        float GetProcessingLoad() const;
        uint64_t GetProcessedActions() const;
        uint64_t GetDroppedActions() const;
        uint64_t GetAverageProcessingTime() const;
        bool IsOverloaded() const;
        
        // Statistics
        void PrintStatistics() const;
        void ResetStatistics();
        std::string GetStatusReport() const;
        
        // Configuration
        void SetProcessingInterval(uint32_t interval);
        void SetMaxProcessingTime(uint32_t maxTime);
        void SetMaxQueueSize(uint32_t maxSize);
        
        // Health checks
        bool IsHealthy() const;
        bool IsInitialized() const;
        bool IsProcessing() const;
        
        // Utility methods
        std::string GetSystemInfo() const;
        void ForceProcessActions();
    };

    // Combat action structure for batch operations
    struct CombatAction
    {
        enum Type
        {
            Attack,
            Defense,
            Sign,
            Animation,
            Movement
        };
        
        Type type;
        uint32_t playerId;
        std::vector<uint8_t> data;
        uint32_t priority;
        std::chrono::steady_clock::time_point timestamp;
        
        CombatAction() : type(Attack), playerId(0), priority(0), 
                        timestamp(std::chrono::steady_clock::now()) {}
    };

    // Combat system configuration
    struct CombatSystemConfig
    {
        uint32_t processingInterval;
        uint32_t maxProcessingTime;
        uint32_t maxQueueSize;
        uint32_t batchSize;
        bool enablePerformanceMonitoring;
        bool enableStatistics;
        bool enableDebugLogging;
        
        CombatSystemConfig() : processingInterval(16), maxProcessingTime(10), 
                             maxQueueSize(1000), batchSize(50),
                             enablePerformanceMonitoring(true),
                             enableStatistics(true), enableDebugLogging(false) {}
    };

    // Combat system factory
    class CombatSystemFactory
    {
    public:
        static std::unique_ptr<CombatSystemIntegration> CreateCombatSystem(const CombatSystemConfig& config);
        static CombatSystemConfig GetDefaultConfig();
        static CombatSystemConfig GetHighPerformanceConfig();
        static CombatSystemConfig GetLowLatencyConfig();
    };

    // Combat system validator
    class CombatSystemValidator
    {
    public:
        static bool ValidateConfiguration(const CombatSystemConfig& config);
        static bool ValidateSystemHealth(const CombatSystemIntegration& system);
        static std::vector<std::string> GetValidationErrors(const CombatSystemConfig& config);
        static std::vector<std::string> GetHealthIssues(const CombatSystemIntegration& system);
    };
}
