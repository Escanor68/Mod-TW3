#pragma once

#include "Common.h"
#include "game/Entities/Player/Player.h"
#include <vector>
#include <map>
#include <queue>
#include <chrono>
#include <functional>

namespace Optimization
{
    // Movement prediction states
    enum class PredictionState
    {
        Idle = 0,
        Predicting = 1,
        Reconciling = 2,
        Corrected = 3
    };

    // Movement input data
    struct MovementInput
    {
        uint32_t inputId;
        float timestamp;
        Vector4F position;
        Vector4F velocity;
        Vector4F acceleration;
        float rotation;
        bool isMoving;
        bool isRunning;
        bool isJumping;
        bool isCrouching;
        
        MovementInput() : inputId(0), timestamp(0.0f), rotation(0.0f), 
                         isMoving(false), isRunning(false), isJumping(false), isCrouching(false) {}
    };

    // Predicted movement state
    struct PredictedState
    {
        uint32_t inputId;
        float timestamp;
        Vector4F position;
        Vector4F velocity;
        Vector4F acceleration;
        float rotation;
        bool isValid;
        
        PredictedState() : inputId(0), timestamp(0.0f), rotation(0.0f), isValid(false) {}
    };

    // Server reconciliation data
    struct ReconciliationData
    {
        uint32_t inputId;
        float timestamp;
        Vector4F serverPosition;
        Vector4F clientPosition;
        float positionError;
        bool needsCorrection;
        
        ReconciliationData() : inputId(0), timestamp(0.0f), positionError(0.0f), needsCorrection(false) {}
    };

    // Movement prediction configuration
    struct PredictionConfig
    {
        float maxPredictionTime = 0.5f;        // Maximum time to predict ahead
        float reconciliationThreshold = 0.1f;   // Position error threshold for reconciliation
        float interpolationSpeed = 10.0f;       // Speed of position interpolation
        float maxInputHistory = 1.0f;          // Maximum time to keep input history
        uint32_t maxPredictedStates = 64;      // Maximum number of predicted states
        bool enableSmoothing = true;           // Enable position smoothing
        bool enableReconciliation = true;      // Enable server reconciliation
        float smoothingFactor = 0.1f;          // Position smoothing factor
    };

    // Movement prediction system
    class MovementPrediction
    {
    public:
        MovementPrediction();
        ~MovementPrediction();

        // Initialize prediction system
        bool Initialize(const PredictionConfig& config = PredictionConfig());
        void Shutdown();

        // Input handling
        void AddInput(const MovementInput& input);
        void ProcessInputs(float deltaTime);
        void ClearInputs();

        // Prediction methods
        PredictedState PredictMovement(const MovementInput& input, float deltaTime);
        std::vector<PredictedState> PredictMovementSequence(const std::vector<MovementInput>& inputs, float deltaTime);
        
        // Server reconciliation
        void ReconcileWithServer(uint32_t inputId, const Vector4F& serverPosition, float timestamp);
        void ReconcileWithServer(const ReconciliationData& reconciliationData);
        bool NeedsReconciliation(uint32_t inputId) const;
        
        // State management
        void SetCurrentState(const Vector4F& position, const Vector4F& velocity, float rotation);
        Vector4F GetCurrentPosition() const;
        Vector4F GetCurrentVelocity() const;
        float GetCurrentRotation() const;
        
        // Prediction queries
        Vector4F GetPredictedPosition(float timeAhead) const;
        Vector4F GetPredictedPosition(uint32_t inputId) const;
        bool IsStatePredicted(uint32_t inputId) const;
        
        // Configuration
        void SetConfig(const PredictionConfig& config);
        PredictionConfig GetConfig() const;
        
        // Statistics
        struct PredictionStats
        {
            uint32_t totalPredictions = 0;
            uint32_t totalReconciliations = 0;
            uint32_t successfulPredictions = 0;
            uint32_t failedPredictions = 0;
            float averagePredictionError = 0.0f;
            float maxPredictionError = 0.0f;
            float totalPredictionTime = 0.0f;
            
            void Reset()
            {
                totalPredictions = 0;
                totalReconciliations = 0;
                successfulPredictions = 0;
                failedPredictions = 0;
                averagePredictionError = 0.0f;
                maxPredictionError = 0.0f;
                totalPredictionTime = 0.0f;
            }
        };
        
        PredictionStats GetStats() const;
        void ResetStats();
        void PrintStats() const;

        // Callbacks
        using PositionUpdateCallback = std::function<void(const Vector4F&, float)>;
        using ReconciliationCallback = std::function<void(uint32_t, const Vector4F&, const Vector4F&)>;
        
        void SetPositionUpdateCallback(PositionUpdateCallback callback);
        void SetReconciliationCallback(ReconciliationCallback callback);

    private:
        // Internal prediction methods
        Vector4F PredictPosition(const Vector4F& currentPos, const Vector4F& velocity, 
                                const Vector4F& acceleration, float deltaTime);
        Vector4F PredictVelocity(const Vector4F& currentVel, const Vector4F& acceleration, float deltaTime);
        float PredictRotation(float currentRotation, float deltaTime);
        
        // State interpolation
        Vector4F InterpolatePosition(const Vector4F& from, const Vector4F& to, float factor);
        float InterpolateRotation(float from, float to, float factor);
        
        // Input processing
        void ProcessMovementInput(const MovementInput& input, float deltaTime);
        void UpdateCurrentState(float deltaTime);
        
        // Reconciliation
        void PerformReconciliation(uint32_t inputId, const Vector4F& serverPosition);
        void SmoothCorrection(const Vector4F& targetPosition, float deltaTime);
        
        // Utility methods
        float CalculatePositionError(const Vector4F& predicted, const Vector4F& actual) const;
        bool IsInputValid(const MovementInput& input) const;
        void CleanupOldInputs();
        void CleanupOldStates();

        // Member variables
        bool m_initialized;
        PredictionConfig m_config;
        PredictionStats m_stats;
        
        // Current state
        Vector4F m_currentPosition;
        Vector4F m_currentVelocity;
        Vector4F m_currentAcceleration;
        float m_currentRotation;
        
        // Input history
        std::queue<MovementInput> m_inputHistory;
        std::map<uint32_t, MovementInput> m_inputMap;
        uint32_t m_nextInputId;
        
        // Predicted states
        std::map<uint32_t, PredictedState> m_predictedStates;
        std::queue<uint32_t> m_predictionQueue;
        
        // Reconciliation
        std::map<uint32_t, ReconciliationData> m_reconciliationData;
        std::queue<uint32_t> m_reconciliationQueue;
        
        // Callbacks
        PositionUpdateCallback m_positionUpdateCallback;
        ReconciliationCallback m_reconciliationCallback;
        
        // Timing
        std::chrono::high_resolution_clock::time_point m_lastUpdateTime;
        float m_accumulatedTime;
    };

    // Movement prediction utilities
    namespace MovementUtils
    {
        // Input validation
        bool ValidateMovementInput(const MovementInput& input);
        bool ValidatePosition(const Vector4F& position);
        bool ValidateVelocity(const Vector4F& velocity);
        
        // Movement calculations
        Vector4F CalculateMovement(const Vector4F& position, const Vector4F& velocity, 
                                  const Vector4F& acceleration, float deltaTime);
        Vector4F CalculateFriction(const Vector4F& velocity, float friction, float deltaTime);
        Vector4F CalculateGravity(const Vector4F& velocity, float gravity, float deltaTime);
        
        // Collision prediction
        bool PredictCollision(const Vector4F& position, const Vector4F& velocity, 
                             const Vector4F& target, float radius);
        Vector4F PredictCollisionPoint(const Vector4F& position, const Vector4F& velocity, 
                                      const Vector4F& target, float radius);
        
        // Smoothing
        Vector4F SmoothPosition(const Vector4F& current, const Vector4F& target, float factor);
        float SmoothRotation(float current, float target, float factor);
        
        // Interpolation
        Vector4F LerpPosition(const Vector4F& from, const Vector4F& to, float t);
        float LerpRotation(float from, float to, float t);
        Vector4F SlerpPosition(const Vector4F& from, const Vector4F& to, float t);
    }

    // Movement prediction manager (for multiple players)
    class MovementPredictionManager
    {
    public:
        MovementPredictionManager();
        ~MovementPredictionManager();

        // Player management
        void AddPlayer(uint32_t playerId, const PredictionConfig& config = PredictionConfig());
        void RemovePlayer(uint32_t playerId);
        bool HasPlayer(uint32_t playerId) const;
        
        // Input handling
        void AddPlayerInput(uint32_t playerId, const MovementInput& input);
        void ProcessAllInputs(float deltaTime);
        
        // Prediction
        PredictedState GetPlayerPrediction(uint32_t playerId, float timeAhead) const;
        std::vector<PredictedState> GetAllPlayerPredictions(float timeAhead) const;
        
        // Reconciliation
        void ReconcilePlayer(uint32_t playerId, uint32_t inputId, const Vector4F& serverPosition);
        void ReconcileAllPlayers(const std::map<uint32_t, std::pair<uint32_t, Vector4F>>& reconciliationData);
        
        // Configuration
        void SetPlayerConfig(uint32_t playerId, const PredictionConfig& config);
        void SetGlobalConfig(const PredictionConfig& config);
        
        // Statistics
        std::map<uint32_t, MovementPrediction::PredictionStats> GetAllStats() const;
        MovementPrediction::PredictionStats GetPlayerStats(uint32_t playerId) const;
        
        // Cleanup
        void Cleanup();
        void ResetAllStats();

    private:
        std::map<uint32_t, std::unique_ptr<MovementPrediction>> m_playerPredictions;
        PredictionConfig m_globalConfig;
    };
}
