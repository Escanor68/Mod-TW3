#include "optimization/CombatOptimizer.h"
#include "integration/REDkitBridge.h"
#include "integration/WitcherScriptBridge.h"
#include <algorithm>
#include <chrono>
#include <thread>
#include <sstream>

namespace TW3Optimization
{
    // Static member initialization
    CombatOptimizer* CombatOptimizer::s_instance = nullptr;
    CombatPerformanceMonitor* CombatPerformanceMonitor::s_instance = nullptr;

    // CombatOptimizer Implementation
    CombatOptimizer::CombatOptimizer()
        : m_processing(false), m_initialized(false), m_maxQueueSize(1000), 
          m_batchSize(50), m_maxProcessingTime(16), m_actionIdCounter(1)
    {
        m_lastProcessTime = std::chrono::steady_clock::now();
    }

    CombatOptimizer::~CombatOptimizer()
    {
        Shutdown();
    }

    CombatOptimizer& CombatOptimizer::GetInstance()
    {
        if (!s_instance)
        {
            s_instance = new CombatOptimizer();
        }
        return *s_instance;
    }

    void CombatOptimizer::DestroyInstance()
    {
        if (s_instance)
        {
            delete s_instance;
            s_instance = nullptr;
        }
    }

    bool CombatOptimizer::Initialize(uint32_t maxQueueSize, uint32_t batchSize, uint32_t maxProcessingTime)
    {
        if (m_initialized)
        {
            return true;
        }

        m_maxQueueSize = maxQueueSize;
        m_batchSize = batchSize;
        m_maxProcessingTime = maxProcessingTime;
        
        // Reserve space for queues
        m_criticalQueue.reserve(maxQueueSize / 4);
        m_highQueue.reserve(maxQueueSize / 4);
        m_mediumQueue.reserve(maxQueueSize / 2);
        m_lowQueue.reserve(maxQueueSize / 4);
        
        m_initialized = true;
        LOG_INFO("CombatOptimizer initialized with maxQueueSize=" + std::to_string(maxQueueSize) + 
                ", batchSize=" + std::to_string(batchSize) + 
                ", maxProcessingTime=" + std::to_string(maxProcessingTime) + "ms");
        
        return true;
    }

    bool CombatOptimizer::Shutdown()
    {
        if (!m_initialized)
        {
            return true;
        }

        m_processing = false;
        
        // Clear all queues
        ClearAllQueues();
        
        m_initialized = false;
        LOG_INFO("CombatOptimizer shutdown");
        
        return true;
    }

    bool CombatOptimizer::AddAction(const CombatAction& action)
    {
        if (!m_initialized)
        {
            LOG_ERROR("CombatOptimizer not initialized");
            return false;
        }

        std::lock_guard<std::mutex> lock(m_queueMutex);
        
        // Check if we should drop this action
        if (ShouldDropAction(action.priority))
        {
            m_stats.droppedActions++;
            return false;
        }

        // Add to appropriate queue
        switch (action.priority)
        {
            case CombatPriority::Critical:
                if (m_criticalQueue.size() < m_maxQueueSize / 4)
                {
                    m_criticalQueue.push_back(action);
                }
                else
                {
                    m_stats.droppedActions++;
                    return false;
                }
                break;
                
            case CombatPriority::High:
                if (m_highQueue.size() < m_maxQueueSize / 4)
                {
                    m_highQueue.push_back(action);
                }
                else
                {
                    m_stats.droppedActions++;
                    return false;
                }
                break;
                
            case CombatPriority::Medium:
                if (m_mediumQueue.size() < m_maxQueueSize / 2)
                {
                    m_mediumQueue.push_back(action);
                }
                else
                {
                    m_stats.droppedActions++;
                    return false;
                }
                break;
                
            case CombatPriority::Low:
                if (m_lowQueue.size() < m_maxQueueSize / 4)
                {
                    m_lowQueue.push_back(action);
                }
                else
                {
                    m_stats.droppedActions++;
                    return false;
                }
                break;
        }

        m_stats.totalActions++;
        return true;
    }

    bool CombatOptimizer::AddAttackAction(uint32_t playerId, uint32_t attackType, uint32_t weaponType, const Vector4F& targetPosition)
    {
        CombatAction action;
        action.id = GenerateActionId();
        action.playerId = playerId;
        action.type = CombatActionType::Attack;
        action.priority = DeterminePriority(CombatActionType::Attack, playerId);
        action.timestamp = std::chrono::steady_clock::now();
        action.processed = false;
        
        // Serialize attack data
        action.data.resize(sizeof(uint32_t) * 3 + sizeof(Vector4F));
        uint8_t* ptr = action.data.data();
        
        *reinterpret_cast<uint32_t*>(ptr) = attackType;
        ptr += sizeof(uint32_t);
        *reinterpret_cast<uint32_t*>(ptr) = weaponType;
        ptr += sizeof(uint32_t);
        *reinterpret_cast<Vector4F*>(ptr) = targetPosition;
        ptr += sizeof(Vector4F);
        *reinterpret_cast<uint32_t*>(ptr) = 0; // Padding
        
        return AddAction(action);
    }

    bool CombatOptimizer::AddDefenseAction(uint32_t playerId, uint32_t defenseType, uint32_t incomingAttackId)
    {
        CombatAction action;
        action.id = GenerateActionId();
        action.playerId = playerId;
        action.type = CombatActionType::Defense;
        action.priority = DeterminePriority(CombatActionType::Defense, playerId);
        action.timestamp = std::chrono::steady_clock::now();
        action.processed = false;
        
        // Serialize defense data
        action.data.resize(sizeof(uint32_t) * 2);
        uint8_t* ptr = action.data.data();
        
        *reinterpret_cast<uint32_t*>(ptr) = defenseType;
        ptr += sizeof(uint32_t);
        *reinterpret_cast<uint32_t*>(ptr) = incomingAttackId;
        
        return AddAction(action);
    }

    bool CombatOptimizer::AddSignAction(uint32_t playerId, uint32_t signType, const Vector4F& targetPosition)
    {
        CombatAction action;
        action.id = GenerateActionId();
        action.playerId = playerId;
        action.type = CombatActionType::Sign;
        action.priority = DeterminePriority(CombatActionType::Sign, playerId);
        action.timestamp = std::chrono::steady_clock::now();
        action.processed = false;
        
        // Serialize sign data
        action.data.resize(sizeof(uint32_t) + sizeof(Vector4F));
        uint8_t* ptr = action.data.data();
        
        *reinterpret_cast<uint32_t*>(ptr) = signType;
        ptr += sizeof(uint32_t);
        *reinterpret_cast<Vector4F*>(ptr) = targetPosition;
        
        return AddAction(action);
    }

    bool CombatOptimizer::AddMovementAction(uint32_t playerId, const Vector4F& newPosition, const Vector4F& velocity)
    {
        CombatAction action;
        action.id = GenerateActionId();
        action.playerId = playerId;
        action.type = CombatActionType::Movement;
        action.priority = DeterminePriority(CombatActionType::Movement, playerId);
        action.timestamp = std::chrono::steady_clock::now();
        action.processed = false;
        
        // Serialize movement data
        action.data.resize(sizeof(Vector4F) * 2);
        uint8_t* ptr = action.data.data();
        
        *reinterpret_cast<Vector4F*>(ptr) = newPosition;
        ptr += sizeof(Vector4F);
        *reinterpret_cast<Vector4F*>(ptr) = velocity;
        
        return AddAction(action);
    }

    bool CombatOptimizer::AddAnimationAction(uint32_t playerId, uint32_t animationType, bool isAttack)
    {
        CombatAction action;
        action.id = GenerateActionId();
        action.playerId = playerId;
        action.type = CombatActionType::Animation;
        action.priority = DeterminePriority(CombatActionType::Animation, playerId);
        action.timestamp = std::chrono::steady_clock::now();
        action.processed = false;
        
        // Serialize animation data
        action.data.resize(sizeof(uint32_t) + sizeof(bool));
        uint8_t* ptr = action.data.data();
        
        *reinterpret_cast<uint32_t*>(ptr) = animationType;
        ptr += sizeof(uint32_t);
        *reinterpret_cast<bool*>(ptr) = isAttack;
        
        return AddAction(action);
    }

    bool CombatOptimizer::ProcessActions()
    {
        if (!m_initialized || m_processing)
        {
            return false;
        }

        m_processing = true;
        auto startTime = std::chrono::steady_clock::now();
        
        // Process actions in priority order
        ProcessActionQueue(m_criticalQueue, CombatPriority::Critical);
        ProcessActionQueue(m_highQueue, CombatPriority::High);
        ProcessActionQueue(m_mediumQueue, CombatPriority::Medium);
        ProcessActionQueue(m_lowQueue, CombatPriority::Low);
        
        // Clean up old actions
        CleanupOldActions();
        
        auto endTime = std::chrono::steady_clock::now();
        auto processingTime = std::chrono::duration_cast<std::chrono::milliseconds>(endTime - startTime).count();
        
        UpdateStatistics(processingTime);
        
        m_processing = false;
        return true;
    }

    void CombatOptimizer::ProcessActionQueue(std::vector<CombatAction>& queue, CombatPriority priority)
    {
        if (queue.empty())
        {
            return;
        }

        size_t batchSize = std::min(static_cast<size_t>(m_batchSize), queue.size());
        
        for (size_t i = 0; i < batchSize; ++i)
        {
            CombatAction& action = queue[i];
            if (!action.processed)
            {
                ProcessAction(action);
                action.processed = true;
            }
        }
        
        // Remove processed actions
        queue.erase(queue.begin(), queue.begin() + batchSize);
    }

    void CombatOptimizer::ProcessAction(const CombatAction& action)
    {
        auto& redkitBridge = REDkitBridge::GetInstance();
        
        switch (action.type)
        {
            case CombatActionType::Attack:
            {
                if (action.data.size() >= sizeof(uint32_t) * 3 + sizeof(Vector4F))
                {
                    const uint8_t* ptr = action.data.data();
                    uint32_t attackType = *reinterpret_cast<const uint32_t*>(ptr);
                    ptr += sizeof(uint32_t);
                    uint32_t weaponType = *reinterpret_cast<const uint32_t*>(ptr);
                    ptr += sizeof(uint32_t);
                    Vector4F targetPosition = *reinterpret_cast<const Vector4F*>(ptr);
                    
                    redkitBridge.ExecuteAttack(action.playerId, attackType, weaponType, targetPosition);
                }
                break;
            }
            
            case CombatActionType::Defense:
            {
                if (action.data.size() >= sizeof(uint32_t) * 2)
                {
                    const uint8_t* ptr = action.data.data();
                    uint32_t defenseType = *reinterpret_cast<const uint32_t*>(ptr);
                    ptr += sizeof(uint32_t);
                    uint32_t incomingAttackId = *reinterpret_cast<const uint32_t*>(ptr);
                    
                    redkitBridge.ExecuteDefense(action.playerId, defenseType, incomingAttackId);
                }
                break;
            }
            
            case CombatActionType::Sign:
            {
                if (action.data.size() >= sizeof(uint32_t) + sizeof(Vector4F))
                {
                    const uint8_t* ptr = action.data.data();
                    uint32_t signType = *reinterpret_cast<const uint32_t*>(ptr);
                    ptr += sizeof(uint32_t);
                    Vector4F targetPosition = *reinterpret_cast<const Vector4F*>(ptr);
                    
                    redkitBridge.CastSign(action.playerId, signType, targetPosition);
                }
                break;
            }
            
            case CombatActionType::Movement:
            {
                if (action.data.size() >= sizeof(Vector4F) * 2)
                {
                    const uint8_t* ptr = action.data.data();
                    Vector4F newPosition = *reinterpret_cast<const Vector4F*>(ptr);
                    ptr += sizeof(Vector4F);
                    Vector4F velocity = *reinterpret_cast<const Vector4F*>(ptr);
                    
                    // Process movement (placeholder)
                    LOG_DEBUG("Processing movement for player " + std::to_string(action.playerId));
                }
                break;
            }
            
            case CombatActionType::Animation:
            {
                if (action.data.size() >= sizeof(uint32_t) + sizeof(bool))
                {
                    const uint8_t* ptr = action.data.data();
                    uint32_t animationType = *reinterpret_cast<const uint32_t*>(ptr);
                    ptr += sizeof(uint32_t);
                    bool isAttack = *reinterpret_cast<const bool*>(ptr);
                    
                    if (isAttack)
                    {
                        redkitBridge.PlayAttackAnimation(action.playerId, animationType);
                    }
                    else
                    {
                        redkitBridge.PlayDefenseAnimation(action.playerId, animationType);
                    }
                }
                break;
            }
        }
        
        m_stats.processedActions++;
    }

    void CombatOptimizer::UpdateStatistics(uint64_t processingTime)
    {
        std::lock_guard<std::mutex> lock(m_statsMutex);
        
        m_processingTimes.push_back(processingTime);
        if (m_processingTimes.size() > 100)
        {
            m_processingTimes.erase(m_processingTimes.begin());
        }
        
        // Calculate average processing time
        uint64_t totalTime = 0;
        for (uint64_t time : m_processingTimes)
        {
            totalTime += time;
        }
        m_stats.averageProcessingTime = totalTime / m_processingTimes.size();
        
        // Update peak processing time
        if (processingTime > m_stats.peakProcessingTime)
        {
            m_stats.peakProcessingTime = processingTime;
        }
    }

    void CombatOptimizer::CleanupOldActions()
    {
        auto now = std::chrono::steady_clock::now();
        auto maxAge = std::chrono::milliseconds(5000); // 5 seconds
        
        auto cleanupQueue = [&](std::vector<CombatAction>& queue)
        {
            queue.erase(
                std::remove_if(queue.begin(), queue.end(),
                    [&](const CombatAction& action)
                    {
                        return (now - action.timestamp) > maxAge;
                    }),
                queue.end()
            );
        };
        
        std::lock_guard<std::mutex> lock(m_queueMutex);
        cleanupQueue(m_criticalQueue);
        cleanupQueue(m_highQueue);
        cleanupQueue(m_mediumQueue);
        cleanupQueue(m_lowQueue);
    }

    CombatPriority CombatOptimizer::DeterminePriority(CombatActionType type, uint32_t playerId)
    {
        switch (type)
        {
            case CombatActionType::Attack:
                return CombatPriority::High;
            case CombatActionType::Defense:
                return CombatPriority::Critical;
            case CombatActionType::Sign:
                return CombatPriority::High;
            case CombatActionType::Movement:
                return CombatPriority::Low;
            case CombatActionType::Animation:
                return CombatPriority::Medium;
            default:
                return CombatPriority::Medium;
        }
    }

    bool CombatOptimizer::ShouldDropAction(CombatPriority priority)
    {
        size_t totalSize = GetTotalQueueSize();
        
        // Drop low priority actions if queue is getting full
        if (priority == CombatPriority::Low && totalSize > m_maxQueueSize * 0.8)
        {
            return true;
        }
        
        // Drop medium priority actions if queue is very full
        if (priority == CombatPriority::Medium && totalSize > m_maxQueueSize * 0.9)
        {
            return true;
        }
        
        return false;
    }

    // Getters and utility methods
    size_t CombatOptimizer::GetQueueSize(CombatPriority priority) const
    {
        std::lock_guard<std::mutex> lock(m_queueMutex);
        
        switch (priority)
        {
            case CombatPriority::Critical: return m_criticalQueue.size();
            case CombatPriority::High: return m_highQueue.size();
            case CombatPriority::Medium: return m_mediumQueue.size();
            case CombatPriority::Low: return m_lowQueue.size();
            default: return 0;
        }
    }

    size_t CombatOptimizer::GetTotalQueueSize() const
    {
        std::lock_guard<std::mutex> lock(m_queueMutex);
        return m_criticalQueue.size() + m_highQueue.size() + m_mediumQueue.size() + m_lowQueue.size();
    }

    CombatStats CombatOptimizer::GetStatistics() const
    {
        std::lock_guard<std::mutex> lock(m_statsMutex);
        return m_stats;
    }

    void CombatOptimizer::ResetStatistics()
    {
        std::lock_guard<std::mutex> lock(m_statsMutex);
        m_stats = CombatStats();
        m_processingTimes.clear();
    }

    void CombatOptimizer::PrintStatistics() const
    {
        CombatStats stats = GetStatistics();
        
        LOG_INFO("=== Combat Optimizer Statistics ===");
        LOG_INFO("Total Actions: " + std::to_string(stats.totalActions.load()));
        LOG_INFO("Processed Actions: " + std::to_string(stats.processedActions.load()));
        LOG_INFO("Dropped Actions: " + std::to_string(stats.droppedActions.load()));
        LOG_INFO("Average Processing Time: " + std::to_string(stats.averageProcessingTime.load()) + "ms");
        LOG_INFO("Peak Processing Time: " + std::to_string(stats.peakProcessingTime.load()) + "ms");
        LOG_INFO("Queue Sizes - Critical: " + std::to_string(GetQueueSize(CombatPriority::Critical)) +
                ", High: " + std::to_string(GetQueueSize(CombatPriority::High)) +
                ", Medium: " + std::to_string(GetQueueSize(CombatPriority::Medium)) +
                ", Low: " + std::to_string(GetQueueSize(CombatPriority::Low)));
    }

    bool CombatOptimizer::IsOverloaded() const
    {
        return GetTotalQueueSize() > m_maxQueueSize * 0.8;
    }

    float CombatOptimizer::GetProcessingLoad() const
    {
        return static_cast<float>(GetTotalQueueSize()) / m_maxQueueSize;
    }

    uint64_t CombatOptimizer::GetAverageProcessingTime() const
    {
        std::lock_guard<std::mutex> lock(m_statsMutex);
        return m_stats.averageProcessingTime.load();
    }

    uint64_t CombatOptimizer::GetPeakProcessingTime() const
    {
        std::lock_guard<std::mutex> lock(m_statsMutex);
        return m_stats.peakProcessingTime.load();
    }

    uint32_t CombatOptimizer::GenerateActionId()
    {
        return m_actionIdCounter++;
    }

    bool CombatOptimizer::IsInitialized() const
    {
        return m_initialized;
    }

    std::string CombatOptimizer::GetStatusString() const
    {
        std::stringstream ss;
        ss << "CombatOptimizer Status: ";
        ss << "Initialized=" << (m_initialized ? "true" : "false");
        ss << ", Processing=" << (m_processing ? "true" : "false");
        ss << ", TotalQueueSize=" << GetTotalQueueSize();
        ss << ", Load=" << (GetProcessingLoad() * 100.0f) << "%";
        return ss.str();
    }

    // CombatActionBuilder Implementation
    CombatActionBuilder::CombatActionBuilder()
    {
        m_action.id = 0;
        m_action.playerId = 0;
        m_action.type = CombatActionType::Attack;
        m_action.priority = CombatPriority::Medium;
        m_action.timestamp = std::chrono::steady_clock::now();
        m_action.processed = false;
    }

    CombatActionBuilder& CombatActionBuilder::SetPlayerId(uint32_t playerId)
    {
        m_action.playerId = playerId;
        return *this;
    }

    CombatActionBuilder& CombatActionBuilder::SetType(CombatActionType type)
    {
        m_action.type = type;
        return *this;
    }

    CombatActionBuilder& CombatActionBuilder::SetPriority(CombatPriority priority)
    {
        m_action.priority = priority;
        return *this;
    }

    CombatActionBuilder& CombatActionBuilder::SetData(const std::vector<uint8_t>& data)
    {
        m_action.data = data;
        return *this;
    }

    CombatActionBuilder& CombatActionBuilder::SetTimestamp(std::chrono::steady_clock::time_point timestamp)
    {
        m_action.timestamp = timestamp;
        return *this;
    }

    CombatAction CombatActionBuilder::Build()
    {
        m_action.id = CombatOptimizer::GetInstance().GenerateActionId();
        return m_action;
    }

    bool CombatActionBuilder::Submit()
    {
        return CombatOptimizer::GetInstance().AddAction(Build());
    }
}
