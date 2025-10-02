#pragma once

#include <vector>
#include <unordered_map>
#include <chrono>
#include <mutex>
#include <atomic>
#include "game/Entities/Player.h"
#include "utils/Logger.h"

namespace TW3Optimization
{
    // Combat action types
    enum class CombatActionType
    {
        Attack,
        Defense,
        Sign,
        Movement,
        Animation
    };

    // Combat action priority levels
    enum class CombatPriority
    {
        Critical = 0,    // Immediate processing
        High = 1,        // High priority
        Medium = 2,      // Normal priority
        Low = 3          // Low priority
    };

    // Combat action structure
    struct CombatAction
    {
        uint32_t id;
        uint32_t playerId;
        CombatActionType type;
        CombatPriority priority;
        std::chrono::steady_clock::time_point timestamp;
        std::vector<uint8_t> data;
        bool processed;
        
        CombatAction() : id(0), playerId(0), type(CombatActionType::Attack), 
                        priority(CombatPriority::Medium), processed(false) {}
    };

    // Combat statistics
    struct CombatStats
    {
        std::atomic<uint64_t> totalActions;
        std::atomic<uint64_t> processedActions;
        std::atomic<uint64_t> droppedActions;
        std::atomic<uint64_t> averageProcessingTime;
        std::atomic<uint64_t> peakProcessingTime;
        
        CombatStats() : totalActions(0), processedActions(0), droppedActions(0),
                       averageProcessingTime(0), peakProcessingTime(0) {}
    };

    // Combat optimizer class
    class CombatOptimizer
    {
    private:
        static CombatOptimizer* s_instance;
        
        // Action queues by priority
        std::vector<CombatAction> m_criticalQueue;
        std::vector<CombatAction> m_highQueue;
        std::vector<CombatAction> m_mediumQueue;
        std::vector<CombatAction> m_lowQueue;
        
        // Processing state
        std::atomic<bool> m_processing;
        std::atomic<bool> m_initialized;
        std::mutex m_queueMutex;
        std::mutex m_statsMutex;
        
        // Statistics
        CombatStats m_stats;
        
        // Performance settings
        uint32_t m_maxQueueSize;
        uint32_t m_batchSize;
        uint32_t m_maxProcessingTime;
        uint32_t m_actionIdCounter;
        
        // Performance monitoring
        std::chrono::steady_clock::time_point m_lastProcessTime;
        std::vector<uint64_t> m_processingTimes;
        
        CombatOptimizer();
        ~CombatOptimizer();
        
        // Internal processing methods
        void ProcessActionQueue(std::vector<CombatAction>& queue, CombatPriority priority);
        void ProcessAction(const CombatAction& action);
        void UpdateStatistics(uint64_t processingTime);
        void CleanupOldActions();
        
        // Priority management
        CombatPriority DeterminePriority(CombatActionType type, uint32_t playerId);
        bool ShouldDropAction(CombatPriority priority);
        
    public:
        // Singleton pattern
        static CombatOptimizer& GetInstance();
        static void DestroyInstance();
        
        // Initialization and shutdown
        bool Initialize(uint32_t maxQueueSize = 1000, uint32_t batchSize = 50, uint32_t maxProcessingTime = 16);
        bool Shutdown();
        
        // Action management
        bool AddAction(const CombatAction& action);
        bool AddAttackAction(uint32_t playerId, uint32_t attackType, uint32_t weaponType, const Vector4F& targetPosition);
        bool AddDefenseAction(uint32_t playerId, uint32_t defenseType, uint32_t incomingAttackId);
        bool AddSignAction(uint32_t playerId, uint32_t signType, const Vector4F& targetPosition);
        bool AddMovementAction(uint32_t playerId, const Vector4F& newPosition, const Vector4F& velocity);
        bool AddAnimationAction(uint32_t playerId, uint32_t animationType, bool isAttack);
        
        // Processing
        bool ProcessActions();
        bool ProcessCriticalActions();
        bool ProcessHighPriorityActions();
        bool ProcessMediumPriorityActions();
        bool ProcessLowPriorityActions();
        
        // Queue management
        void ClearQueue(CombatPriority priority);
        void ClearAllQueues();
        size_t GetQueueSize(CombatPriority priority) const;
        size_t GetTotalQueueSize() const;
        
        // Statistics
        CombatStats GetStatistics() const;
        void ResetStatistics();
        void PrintStatistics() const;
        
        // Performance monitoring
        bool IsOverloaded() const;
        float GetProcessingLoad() const;
        uint64_t GetAverageProcessingTime() const;
        uint64_t GetPeakProcessingTime() const;
        
        // Configuration
        void SetMaxQueueSize(uint32_t maxSize);
        void SetBatchSize(uint32_t batchSize);
        void SetMaxProcessingTime(uint32_t maxTime);
        
        // Utility methods
        uint32_t GenerateActionId();
        bool IsInitialized() const;
        std::string GetStatusString() const;
    };

    // Combat action builder for fluent API
    class CombatActionBuilder
    {
    private:
        CombatAction m_action;
        
    public:
        CombatActionBuilder();
        
        CombatActionBuilder& SetPlayerId(uint32_t playerId);
        CombatActionBuilder& SetType(CombatActionType type);
        CombatActionBuilder& SetPriority(CombatPriority priority);
        CombatActionBuilder& SetData(const std::vector<uint8_t>& data);
        CombatActionBuilder& SetTimestamp(std::chrono::steady_clock::time_point timestamp);
        
        CombatAction Build();
        bool Submit();
    };

    // Combat performance monitor
    class CombatPerformanceMonitor
    {
    private:
        static CombatPerformanceMonitor* s_instance;
        
        std::atomic<bool> m_monitoring;
        std::atomic<bool> m_initialized;
        std::mutex m_dataMutex;
        
        // Performance data
        std::vector<uint64_t> m_frameTimes;
        std::vector<uint64_t> m_actionCounts;
        std::vector<uint64_t> m_queueSizes;
        
        // Monitoring settings
        uint32_t m_maxDataPoints;
        uint32_t m_monitoringInterval;
        std::chrono::steady_clock::time_point m_lastMonitorTime;
        
        CombatPerformanceMonitor();
        ~CombatPerformanceMonitor();
        
    public:
        static CombatPerformanceMonitor& GetInstance();
        static void DestroyInstance();
        
        bool Initialize(uint32_t maxDataPoints = 1000, uint32_t monitoringInterval = 100);
        bool Shutdown();
        
        void StartMonitoring();
        void StopMonitoring();
        void UpdateFrame(uint64_t frameTime, uint64_t actionCount, uint64_t queueSize);
        
        std::vector<uint64_t> GetFrameTimes() const;
        std::vector<uint64_t> GetActionCounts() const;
        std::vector<uint64_t> GetQueueSizes() const;
        
        float GetAverageFrameTime() const;
        float GetAverageActionCount() const;
        float GetAverageQueueSize() const;
        
        bool IsMonitoring() const;
        void ResetData();
    };
}
