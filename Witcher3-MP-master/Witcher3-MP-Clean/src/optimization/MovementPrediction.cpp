#include "optimization/MovementPrediction.h"
#include "utils/Logger.h"
#include <algorithm>
#include <cmath>

namespace Optimization
{
    MovementPrediction::MovementPrediction()
        : m_initialized(false), m_nextInputId(1), m_accumulatedTime(0.0f)
    {
        m_currentPosition = Vector4F{0.0f, 0.0f, 0.0f, 1.0f};
        m_currentVelocity = Vector4F{0.0f, 0.0f, 0.0f, 0.0f};
        m_currentAcceleration = Vector4F{0.0f, 0.0f, 0.0f, 0.0f};
        m_currentRotation = 0.0f;
        
        m_lastUpdateTime = std::chrono::high_resolution_clock::now();
    }

    MovementPrediction::~MovementPrediction()
    {
        Shutdown();
    }

    bool MovementPrediction::Initialize(const PredictionConfig& config)
    {
        if (m_initialized)
        {
            return true;
        }

        LOG_INFO("Initializing movement prediction system...");

        m_config = config;
        m_initialized = true;
        
        LOG_INFO("Movement prediction system initialized");
        return true;
    }

    void MovementPrediction::Shutdown()
    {
        if (!m_initialized)
        {
            return;
        }

        LOG_INFO("Shutting down movement prediction system...");
        
        // Clear all data
        ClearInputs();
        m_predictedStates.clear();
        m_reconciliationData.clear();
        
        // Clear queues
        while (!m_predictionQueue.empty()) m_predictionQueue.pop();
        while (!m_reconciliationQueue.empty()) m_reconciliationQueue.pop();
        
        m_initialized = false;
        LOG_INFO("Movement prediction system shutdown complete");
    }

    void MovementPrediction::AddInput(const MovementInput& input)
    {
        if (!m_initialized)
        {
            return;
        }

        if (!IsInputValid(input))
        {
            LOG_WARNING("Invalid movement input received");
            return;
        }

        // Assign input ID if not set
        MovementInput inputCopy = input;
        if (inputCopy.inputId == 0)
        {
            inputCopy.inputId = m_nextInputId++;
        }

        // Add to input history
        m_inputHistory.push(inputCopy);
        m_inputMap[inputCopy.inputId] = inputCopy;

        // Clean up old inputs
        CleanupOldInputs();

        LOG_DEBUG("Added movement input ID: " + std::to_string(inputCopy.inputId));
    }

    void MovementPrediction::ProcessInputs(float deltaTime)
    {
        if (!m_initialized)
        {
            return;
        }

        m_accumulatedTime += deltaTime;

        // Process all inputs in the queue
        while (!m_inputHistory.empty())
        {
            MovementInput input = m_inputHistory.front();
            m_inputHistory.pop();

            ProcessMovementInput(input, deltaTime);
        }

        // Update current state
        UpdateCurrentState(deltaTime);

        // Process reconciliation queue
        while (!m_reconciliationQueue.empty())
        {
            uint32_t inputId = m_reconciliationQueue.front();
            m_reconciliationQueue.pop();

            auto it = m_reconciliationData.find(inputId);
            if (it != m_reconciliationData.end())
            {
                PerformReconciliation(inputId, it->second.serverPosition);
            }
        }
    }

    void MovementPrediction::ClearInputs()
    {
        while (!m_inputHistory.empty()) m_inputHistory.pop();
        m_inputMap.clear();
        m_predictedStates.clear();
        m_reconciliationData.clear();
        
        while (!m_predictionQueue.empty()) m_predictionQueue.pop();
        while (!m_reconciliationQueue.empty()) m_reconciliationQueue.pop();
    }

    PredictedState MovementPrediction::PredictMovement(const MovementInput& input, float deltaTime)
    {
        if (!m_initialized)
        {
            return PredictedState();
        }

        auto startTime = std::chrono::high_resolution_clock::now();

        PredictedState predictedState;
        predictedState.inputId = input.inputId;
        predictedState.timestamp = input.timestamp;
        
        // Predict position
        predictedState.position = PredictPosition(m_currentPosition, input.velocity, input.acceleration, deltaTime);
        
        // Predict velocity
        predictedState.velocity = PredictVelocity(input.velocity, input.acceleration, deltaTime);
        
        // Predict rotation
        predictedState.rotation = PredictRotation(input.rotation, deltaTime);
        
        predictedState.isValid = true;

        // Store predicted state
        m_predictedStates[input.inputId] = predictedState;
        m_predictionQueue.push(input.inputId);

        auto endTime = std::chrono::high_resolution_clock::now();
        float predictionTime = std::chrono::duration<float>(endTime - startTime).count();
        
        m_stats.totalPredictionTime += predictionTime;
        m_stats.totalPredictions++;

        LOG_DEBUG("Predicted movement for input ID: " + std::to_string(input.inputId));

        return predictedState;
    }

    std::vector<PredictedState> MovementPrediction::PredictMovementSequence(const std::vector<MovementInput>& inputs, float deltaTime)
    {
        std::vector<PredictedState> predictions;
        predictions.reserve(inputs.size());

        for (const auto& input : inputs)
        {
            predictions.push_back(PredictMovement(input, deltaTime));
        }

        return predictions;
    }

    void MovementPrediction::ReconcileWithServer(uint32_t inputId, const Vector4F& serverPosition, float timestamp)
    {
        if (!m_initialized)
        {
            return;
        }

        ReconciliationData reconciliationData;
        reconciliationData.inputId = inputId;
        reconciliationData.timestamp = timestamp;
        reconciliationData.serverPosition = serverPosition;
        
        auto it = m_predictedStates.find(inputId);
        if (it != m_predictedStates.end())
        {
            reconciliationData.clientPosition = it->second.position;
            reconciliationData.positionError = CalculatePositionError(it->second.position, serverPosition);
            reconciliationData.needsCorrection = reconciliationData.positionError > m_config.reconciliationThreshold;
        }

        ReconcileWithServer(reconciliationData);
    }

    void MovementPrediction::ReconcileWithServer(const ReconciliationData& reconciliationData)
    {
        if (!m_initialized)
        {
            return;
        }

        m_reconciliationData[reconciliationData.inputId] = reconciliationData;
        m_reconciliationQueue.push(reconciliationData.inputId);
        
        m_stats.totalReconciliations++;

        LOG_DEBUG("Reconciliation queued for input ID: " + std::to_string(reconciliationData.inputId) + 
                 " (error: " + std::to_string(reconciliationData.positionError) + ")");
    }

    bool MovementPrediction::NeedsReconciliation(uint32_t inputId) const
    {
        auto it = m_reconciliationData.find(inputId);
        return it != m_reconciliationData.end() && it->second.needsCorrection;
    }

    void MovementPrediction::SetCurrentState(const Vector4F& position, const Vector4F& velocity, float rotation)
    {
        m_currentPosition = position;
        m_currentVelocity = velocity;
        m_currentRotation = rotation;
    }

    Vector4F MovementPrediction::GetCurrentPosition() const
    {
        return m_currentPosition;
    }

    Vector4F MovementPrediction::GetCurrentVelocity() const
    {
        return m_currentVelocity;
    }

    float MovementPrediction::GetCurrentRotation() const
    {
        return m_currentRotation;
    }

    Vector4F MovementPrediction::GetPredictedPosition(float timeAhead) const
    {
        // Simple prediction based on current velocity
        Vector4F predictedPos = m_currentPosition;
        predictedPos.x += m_currentVelocity.x * timeAhead;
        predictedPos.y += m_currentVelocity.y * timeAhead;
        predictedPos.z += m_currentVelocity.z * timeAhead;
        return predictedPos;
    }

    Vector4F MovementPrediction::GetPredictedPosition(uint32_t inputId) const
    {
        auto it = m_predictedStates.find(inputId);
        if (it != m_predictedStates.end())
        {
            return it->second.position;
        }
        return m_currentPosition;
    }

    bool MovementPrediction::IsStatePredicted(uint32_t inputId) const
    {
        return m_predictedStates.find(inputId) != m_predictedStates.end();
    }

    void MovementPrediction::SetConfig(const PredictionConfig& config)
    {
        m_config = config;
    }

    PredictionConfig MovementPrediction::GetConfig() const
    {
        return m_config;
    }

    MovementPrediction::PredictionStats MovementPrediction::GetStats() const
    {
        return m_stats;
    }

    void MovementPrediction::ResetStats()
    {
        m_stats.Reset();
    }

    void MovementPrediction::PrintStats() const
    {
        LOG_INFO("=== Movement Prediction Statistics ===");
        LOG_INFO("Total predictions: " + std::to_string(m_stats.totalPredictions));
        LOG_INFO("Total reconciliations: " + std::to_string(m_stats.totalReconciliations));
        LOG_INFO("Successful predictions: " + std::to_string(m_stats.successfulPredictions));
        LOG_INFO("Failed predictions: " + std::to_string(m_stats.failedPredictions));
        LOG_INFO("Average prediction error: " + std::to_string(m_stats.averagePredictionError));
        LOG_INFO("Max prediction error: " + std::to_string(m_stats.maxPredictionError));
        LOG_INFO("Total prediction time: " + std::to_string(m_stats.totalPredictionTime) + "s");
        LOG_INFO("=====================================");
    }

    void MovementPrediction::SetPositionUpdateCallback(PositionUpdateCallback callback)
    {
        m_positionUpdateCallback = callback;
    }

    void MovementPrediction::SetReconciliationCallback(ReconciliationCallback callback)
    {
        m_reconciliationCallback = callback;
    }

    // Internal prediction methods
    Vector4F MovementPrediction::PredictPosition(const Vector4F& currentPos, const Vector4F& velocity, 
                                                const Vector4F& acceleration, float deltaTime)
    {
        Vector4F predictedPos = currentPos;
        predictedPos.x += velocity.x * deltaTime + 0.5f * acceleration.x * deltaTime * deltaTime;
        predictedPos.y += velocity.y * deltaTime + 0.5f * acceleration.y * deltaTime * deltaTime;
        predictedPos.z += velocity.z * deltaTime + 0.5f * acceleration.z * deltaTime * deltaTime;
        return predictedPos;
    }

    Vector4F MovementPrediction::PredictVelocity(const Vector4F& currentVel, const Vector4F& acceleration, float deltaTime)
    {
        Vector4F predictedVel = currentVel;
        predictedVel.x += acceleration.x * deltaTime;
        predictedVel.y += acceleration.y * deltaTime;
        predictedVel.z += acceleration.z * deltaTime;
        return predictedVel;
    }

    float MovementPrediction::PredictRotation(float currentRotation, float deltaTime)
    {
        // Simple rotation prediction (can be enhanced with angular velocity)
        return currentRotation;
    }

    Vector4F MovementPrediction::InterpolatePosition(const Vector4F& from, const Vector4F& to, float factor)
    {
        Vector4F interpolated;
        interpolated.x = from.x + (to.x - from.x) * factor;
        interpolated.y = from.y + (to.y - from.y) * factor;
        interpolated.z = from.z + (to.z - from.z) * factor;
        interpolated.w = 1.0f;
        return interpolated;
    }

    float MovementPrediction::InterpolateRotation(float from, float to, float factor)
    {
        // Handle angle wrapping
        float diff = to - from;
        if (diff > 180.0f) diff -= 360.0f;
        if (diff < -180.0f) diff += 360.0f;
        return from + diff * factor;
    }

    void MovementPrediction::ProcessMovementInput(const MovementInput& input, float deltaTime)
    {
        // Update current state based on input
        if (input.isMoving)
        {
            m_currentVelocity = input.velocity;
            m_currentAcceleration = input.acceleration;
        }
        
        m_currentRotation = input.rotation;
        
        // Predict movement
        PredictedState predictedState = PredictMovement(input, deltaTime);
        
        // Update current position
        m_currentPosition = predictedState.position;
        
        // Call position update callback
        if (m_positionUpdateCallback)
        {
            m_positionUpdateCallback(m_currentPosition, input.timestamp);
        }
    }

    void MovementPrediction::UpdateCurrentState(float deltaTime)
    {
        // Apply physics to current state
        m_currentPosition.x += m_currentVelocity.x * deltaTime;
        m_currentPosition.y += m_currentVelocity.y * deltaTime;
        m_currentPosition.z += m_currentVelocity.z * deltaTime;
        
        // Apply acceleration
        m_currentVelocity.x += m_currentAcceleration.x * deltaTime;
        m_currentVelocity.y += m_currentAcceleration.y * deltaTime;
        m_currentVelocity.z += m_currentAcceleration.z * deltaTime;
    }

    void MovementPrediction::PerformReconciliation(uint32_t inputId, const Vector4F& serverPosition)
    {
        auto it = m_predictedStates.find(inputId);
        if (it == m_predictedStates.end())
        {
            return;
        }

        Vector4F predictedPosition = it->second.position;
        float positionError = CalculatePositionError(predictedPosition, serverPosition);

        if (positionError > m_config.reconciliationThreshold)
        {
            // Perform correction
            if (m_config.enableSmoothing)
            {
                SmoothCorrection(serverPosition, 0.016f); // 60 FPS
            }
            else
            {
                m_currentPosition = serverPosition;
            }

            // Update statistics
            m_stats.averagePredictionError = (m_stats.averagePredictionError + positionError) / 2.0f;
            m_stats.maxPredictionError = std::max(m_stats.maxPredictionError, positionError);

            // Call reconciliation callback
            if (m_reconciliationCallback)
            {
                m_reconciliationCallback(inputId, predictedPosition, serverPosition);
            }

            LOG_DEBUG("Reconciled position for input ID: " + std::to_string(inputId) + 
                     " (error: " + std::to_string(positionError) + ")");
        }
        else
        {
            m_stats.successfulPredictions++;
        }

        // Remove from predicted states
        m_predictedStates.erase(it);
    }

    void MovementPrediction::SmoothCorrection(const Vector4F& targetPosition, float deltaTime)
    {
        Vector4F currentPos = m_currentPosition;
        Vector4F smoothedPos = InterpolatePosition(currentPos, targetPosition, m_config.smoothingFactor * deltaTime);
        m_currentPosition = smoothedPos;
    }

    float MovementPrediction::CalculatePositionError(const Vector4F& predicted, const Vector4F& actual) const
    {
        float dx = predicted.x - actual.x;
        float dy = predicted.y - actual.y;
        float dz = predicted.z - actual.z;
        return std::sqrt(dx * dx + dy * dy + dz * dz);
    }

    bool MovementPrediction::IsInputValid(const MovementInput& input) const
    {
        return MovementUtils::ValidateMovementInput(input);
    }

    void MovementPrediction::CleanupOldInputs()
    {
        float currentTime = static_cast<float>(std::chrono::duration_cast<std::chrono::milliseconds>(
            std::chrono::high_resolution_clock::now().time_since_epoch()).count()) / 1000.0f;

        // Remove old inputs from map
        auto it = m_inputMap.begin();
        while (it != m_inputMap.end())
        {
            if (currentTime - it->second.timestamp > m_config.maxInputHistory)
            {
                it = m_inputMap.erase(it);
            }
            else
            {
                ++it;
            }
        }
    }

    void MovementPrediction::CleanupOldStates()
    {
        // Remove old predicted states
        if (m_predictedStates.size() > m_config.maxPredictedStates)
        {
            auto it = m_predictedStates.begin();
            for (size_t i = 0; i < m_predictedStates.size() - m_config.maxPredictedStates; ++i)
            {
                it = m_predictedStates.erase(it);
            }
        }
    }

    // Movement prediction utilities
    namespace MovementUtils
    {
        bool ValidateMovementInput(const MovementInput& input)
        {
            return ValidatePosition(input.position) && ValidateVelocity(input.velocity);
        }

        bool ValidatePosition(const Vector4F& position)
        {
            return std::isfinite(position.x) && std::isfinite(position.y) && std::isfinite(position.z);
        }

        bool ValidateVelocity(const Vector4F& velocity)
        {
            return std::isfinite(velocity.x) && std::isfinite(velocity.y) && std::isfinite(velocity.z);
        }

        Vector4F CalculateMovement(const Vector4F& position, const Vector4F& velocity, 
                                  const Vector4F& acceleration, float deltaTime)
        {
            Vector4F newPosition = position;
            newPosition.x += velocity.x * deltaTime + 0.5f * acceleration.x * deltaTime * deltaTime;
            newPosition.y += velocity.y * deltaTime + 0.5f * acceleration.y * deltaTime * deltaTime;
            newPosition.z += velocity.z * deltaTime + 0.5f * acceleration.z * deltaTime * deltaTime;
            return newPosition;
        }

        Vector4F CalculateFriction(const Vector4F& velocity, float friction, float deltaTime)
        {
            Vector4F frictionForce = velocity;
            frictionForce.x *= -friction;
            frictionForce.y *= -friction;
            frictionForce.z *= -friction;
            return frictionForce;
        }

        Vector4F CalculateGravity(const Vector4F& velocity, float gravity, float deltaTime)
        {
            Vector4F gravityForce = velocity;
            gravityForce.y -= gravity * deltaTime;
            return gravityForce;
        }

        bool PredictCollision(const Vector4F& position, const Vector4F& velocity, 
                             const Vector4F& target, float radius)
        {
            Vector4F direction = target;
            direction.x -= position.x;
            direction.y -= position.y;
            direction.z -= position.z;
            
            float distance = std::sqrt(direction.x * direction.x + direction.y * direction.y + direction.z * direction.z);
            return distance <= radius;
        }

        Vector4F PredictCollisionPoint(const Vector4F& position, const Vector4F& velocity, 
                                      const Vector4F& target, float radius)
        {
            // Simple collision point prediction
            return target;
        }

        Vector4F SmoothPosition(const Vector4F& current, const Vector4F& target, float factor)
        {
            return LerpPosition(current, target, factor);
        }

        float SmoothRotation(float current, float target, float factor)
        {
            return LerpRotation(current, target, factor);
        }

        Vector4F LerpPosition(const Vector4F& from, const Vector4F& to, float t)
        {
            Vector4F result;
            result.x = from.x + (to.x - from.x) * t;
            result.y = from.y + (to.y - from.y) * t;
            result.z = from.z + (to.z - from.z) * t;
            result.w = 1.0f;
            return result;
        }

        float LerpRotation(float from, float to, float t)
        {
            float diff = to - from;
            if (diff > 180.0f) diff -= 360.0f;
            if (diff < -180.0f) diff += 360.0f;
            return from + diff * t;
        }

        Vector4F SlerpPosition(const Vector4F& from, const Vector4F& to, float t)
        {
            // Simple spherical interpolation (can be enhanced)
            return LerpPosition(from, to, t);
        }
    }

    // Movement prediction manager implementation
    MovementPredictionManager::MovementPredictionManager()
    {
        LOG_INFO("Movement prediction manager created");
    }

    MovementPredictionManager::~MovementPredictionManager()
    {
        Cleanup();
        LOG_INFO("Movement prediction manager destroyed");
    }

    void MovementPredictionManager::AddPlayer(uint32_t playerId, const PredictionConfig& config)
    {
        if (HasPlayer(playerId))
        {
            LOG_WARNING("Player " + std::to_string(playerId) + " already exists in prediction manager");
            return;
        }

        auto prediction = std::make_unique<MovementPrediction>();
        prediction->Initialize(config);
        m_playerPredictions[playerId] = std::move(prediction);

        LOG_INFO("Added player " + std::to_string(playerId) + " to prediction manager");
    }

    void MovementPredictionManager::RemovePlayer(uint32_t playerId)
    {
        auto it = m_playerPredictions.find(playerId);
        if (it != m_playerPredictions.end())
        {
            m_playerPredictions.erase(it);
            LOG_INFO("Removed player " + std::to_string(playerId) + " from prediction manager");
        }
    }

    bool MovementPredictionManager::HasPlayer(uint32_t playerId) const
    {
        return m_playerPredictions.find(playerId) != m_playerPredictions.end();
    }

    void MovementPredictionManager::AddPlayerInput(uint32_t playerId, const MovementInput& input)
    {
        auto it = m_playerPredictions.find(playerId);
        if (it != m_playerPredictions.end())
        {
            it->second->AddInput(input);
        }
    }

    void MovementPredictionManager::ProcessAllInputs(float deltaTime)
    {
        for (auto& pair : m_playerPredictions)
        {
            pair.second->ProcessInputs(deltaTime);
        }
    }

    PredictedState MovementPredictionManager::GetPlayerPrediction(uint32_t playerId, float timeAhead) const
    {
        auto it = m_playerPredictions.find(playerId);
        if (it != m_playerPredictions.end())
        {
            PredictedState state;
            state.position = it->second->GetPredictedPosition(timeAhead);
            state.isValid = true;
            return state;
        }
        return PredictedState();
    }

    std::vector<PredictedState> MovementPredictionManager::GetAllPlayerPredictions(float timeAhead) const
    {
        std::vector<PredictedState> predictions;
        for (const auto& pair : m_playerPredictions)
        {
            predictions.push_back(GetPlayerPrediction(pair.first, timeAhead));
        }
        return predictions;
    }

    void MovementPredictionManager::ReconcilePlayer(uint32_t playerId, uint32_t inputId, const Vector4F& serverPosition)
    {
        auto it = m_playerPredictions.find(playerId);
        if (it != m_playerPredictions.end())
        {
            it->second->ReconcileWithServer(inputId, serverPosition, 0.0f);
        }
    }

    void MovementPredictionManager::ReconcileAllPlayers(const std::map<uint32_t, std::pair<uint32_t, Vector4F>>& reconciliationData)
    {
        for (const auto& pair : reconciliationData)
        {
            ReconcilePlayer(pair.first, pair.second.first, pair.second.second);
        }
    }

    void MovementPredictionManager::SetPlayerConfig(uint32_t playerId, const PredictionConfig& config)
    {
        auto it = m_playerPredictions.find(playerId);
        if (it != m_playerPredictions.end())
        {
            it->second->SetConfig(config);
        }
    }

    void MovementPredictionManager::SetGlobalConfig(const PredictionConfig& config)
    {
        m_globalConfig = config;
        for (auto& pair : m_playerPredictions)
        {
            pair.second->SetConfig(config);
        }
    }

    std::map<uint32_t, MovementPrediction::PredictionStats> MovementPredictionManager::GetAllStats() const
    {
        std::map<uint32_t, MovementPrediction::PredictionStats> stats;
        for (const auto& pair : m_playerPredictions)
        {
            stats[pair.first] = pair.second->GetStats();
        }
        return stats;
    }

    MovementPrediction::PredictionStats MovementPredictionManager::GetPlayerStats(uint32_t playerId) const
    {
        auto it = m_playerPredictions.find(playerId);
        if (it != m_playerPredictions.end())
        {
            return it->second->GetStats();
        }
        return MovementPrediction::PredictionStats();
    }

    void MovementPredictionManager::Cleanup()
    {
        m_playerPredictions.clear();
    }

    void MovementPredictionManager::ResetAllStats()
    {
        for (auto& pair : m_playerPredictions)
        {
            pair.second->ResetStats();
        }
    }
}
