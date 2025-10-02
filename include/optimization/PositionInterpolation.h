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
    // Interpolation types
    enum class InterpolationType
    {
        Linear = 0,
        Cubic = 1,
        Hermite = 2,
        CatmullRom = 3,
        Bezier = 4
    };

    // Interpolation configuration
    struct InterpolationConfig
    {
        InterpolationType type = InterpolationType::Cubic;
        float duration = 0.1f;           // Interpolation duration in seconds
        float smoothing = 0.1f;          // Smoothing factor (0.0 to 1.0)
        bool enableExtrapolation = true; // Enable extrapolation for missing data
        float extrapolationTime = 0.2f;  // Maximum extrapolation time
        bool enableLagCompensation = true; // Enable lag compensation
        float lagCompensationTime = 0.1f;  // Lag compensation time
        bool enableJitterReduction = true; // Enable jitter reduction
        float jitterThreshold = 0.01f;   // Jitter detection threshold
        bool enableAdaptiveInterpolation = true; // Adaptive interpolation based on network conditions
    };

    // Position snapshot
    struct PositionSnapshot
    {
        uint32_t playerId;
        Vector4F position;
        Vector4F velocity;
        Vector4F acceleration;
        float rotation;
        std::chrono::high_resolution_clock::time_point timestamp;
        uint32_t sequenceNumber;
        bool isValid;
        
        PositionSnapshot() : playerId(0), rotation(0.0f), sequenceNumber(0), isValid(false) {}
    };

    // Interpolated position
    struct InterpolatedPosition
    {
        Vector4F position;
        Vector4F velocity;
        Vector4F acceleration;
        float rotation;
        float confidence;        // Confidence level (0.0 to 1.0)
        bool isExtrapolated;     // Whether this is extrapolated data
        bool isJitterCorrected;  // Whether jitter correction was applied
        
        InterpolatedPosition() : rotation(0.0f), confidence(1.0f), 
                                isExtrapolated(false), isJitterCorrected(false) {}
    };

    // Interpolation statistics
    struct InterpolationStats
    {
        uint32_t totalInterpolations = 0;
        uint32_t extrapolations = 0;
        uint32_t jitterCorrections = 0;
        uint32_t lagCompensations = 0;
        float averageInterpolationTime = 0.0f;
        float maxInterpolationTime = 0.0f;
        float averageJitter = 0.0f;
        float maxJitter = 0.0f;
        float averageLag = 0.0f;
        float maxLag = 0.0f;
        
        void Reset()
        {
            totalInterpolations = 0;
            extrapolations = 0;
            jitterCorrections = 0;
            lagCompensations = 0;
            averageInterpolationTime = 0.0f;
            maxInterpolationTime = 0.0f;
            averageJitter = 0.0f;
            maxJitter = 0.0f;
            averageLag = 0.0f;
            maxLag = 0.0f;
        }
    };

    // Position interpolation system
    class PositionInterpolation
    {
    public:
        PositionInterpolation();
        ~PositionInterpolation();

        // Initialize interpolation system
        bool Initialize(const InterpolationConfig& config = InterpolationConfig());
        void Shutdown();

        // Position management
        void AddPositionSnapshot(const PositionSnapshot& snapshot);
        void AddPositionSnapshot(uint32_t playerId, const Vector4F& position, 
                               const Vector4F& velocity, float rotation, uint32_t sequenceNumber);
        
        // Interpolation
        InterpolatedPosition InterpolatePosition(uint32_t playerId, float timeOffset = 0.0f);
        std::vector<InterpolatedPosition> InterpolateAllPositions(float timeOffset = 0.0f);
        InterpolatedPosition ExtrapolatePosition(uint32_t playerId, float timeOffset = 0.0f);
        
        // Position queries
        Vector4F GetCurrentPosition(uint32_t playerId) const;
        Vector4F GetCurrentVelocity(uint32_t playerId) const;
        float GetCurrentRotation(uint32_t playerId) const;
        bool HasPositionData(uint32_t playerId) const;
        
        // Configuration
        void SetConfig(const InterpolationConfig& config);
        InterpolationConfig GetConfig() const;
        void SetInterpolationType(InterpolationType type);
        void SetDuration(float duration);
        void SetSmoothing(float smoothing);
        
        // Network adaptation
        void UpdateNetworkConditions(float latency, float packetLoss, float jitter);
        void AdaptToNetworkConditions();
        
        // Statistics
        InterpolationStats GetStats() const;
        void ResetStats();
        void PrintStats() const;
        
        // Callbacks
        using PositionUpdatedCallback = std::function<void(uint32_t, const InterpolatedPosition&)>;
        using JitterDetectedCallback = std::function<void(uint32_t, float)>;
        
        void SetPositionUpdatedCallback(PositionUpdatedCallback callback);
        void SetJitterDetectedCallback(JitterDetectedCallback callback);

    private:
        // Internal interpolation methods
        InterpolatedPosition InterpolateLinear(const std::vector<PositionSnapshot>& snapshots, float time);
        InterpolatedPosition InterpolateCubic(const std::vector<PositionSnapshot>& snapshots, float time);
        InterpolatedPosition InterpolateHermite(const std::vector<PositionSnapshot>& snapshots, float time);
        InterpolatedPosition InterpolateCatmullRom(const std::vector<PositionSnapshot>& snapshots, float time);
        InterpolatedPosition InterpolateBezier(const std::vector<PositionSnapshot>& snapshots, float time);
        
        // Utility methods
        std::vector<PositionSnapshot> GetSnapshotsForPlayer(uint32_t playerId) const;
        PositionSnapshot FindSnapshotAtTime(const std::vector<PositionSnapshot>& snapshots, float time) const;
        PositionSnapshot FindNearestSnapshot(const std::vector<PositionSnapshot>& snapshots, float time) const;
        
        // Jitter detection and correction
        bool DetectJitter(const PositionSnapshot& current, const PositionSnapshot& previous) const;
        PositionSnapshot CorrectJitter(const PositionSnapshot& snapshot, const PositionSnapshot& previous) const;
        
        // Lag compensation
        PositionSnapshot CompensateForLag(const PositionSnapshot& snapshot, float lagTime) const;
        
        // Extrapolation
        PositionSnapshot ExtrapolateFromSnapshots(const std::vector<PositionSnapshot>& snapshots, float time) const;
        
        // Cleanup
        void CleanupOldSnapshots();
        void CleanupPlayerData(uint32_t playerId);

        // Member variables
        bool m_initialized;
        InterpolationConfig m_config;
        InterpolationStats m_stats;
        
        // Position data
        std::map<uint32_t, std::vector<PositionSnapshot>> m_playerSnapshots;
        std::map<uint32_t, InterpolatedPosition> m_currentPositions;
        
        // Network conditions
        float m_currentLatency;
        float m_currentPacketLoss;
        float m_currentJitter;
        
        // Timing
        std::chrono::high_resolution_clock::time_point m_lastUpdateTime;
        std::chrono::high_resolution_clock::time_point m_lastCleanupTime;
        
        // Callbacks
        PositionUpdatedCallback m_positionUpdatedCallback;
        JitterDetectedCallback m_jitterDetectedCallback;
    };

    // Interpolation utilities
    namespace InterpolationUtils
    {
        // Mathematical interpolation functions
        Vector4F Lerp(const Vector4F& a, const Vector4F& b, float t);
        float Lerp(float a, float b, float t);
        Vector4F Slerp(const Vector4F& a, const Vector4F& b, float t);
        
        // Cubic interpolation
        Vector4F CubicInterpolate(const Vector4F& p0, const Vector4F& p1, 
                                 const Vector4F& p2, const Vector4F& p3, float t);
        float CubicInterpolate(float p0, float p1, float p2, float p3, float t);
        
        // Hermite interpolation
        Vector4F HermiteInterpolate(const Vector4F& p0, const Vector4F& p1, 
                                   const Vector4F& t0, const Vector4F& t1, float t);
        float HermiteInterpolate(float p0, float p1, float t0, float t1, float t);
        
        // Catmull-Rom interpolation
        Vector4F CatmullRomInterpolate(const Vector4F& p0, const Vector4F& p1, 
                                      const Vector4F& p2, const Vector4F& p3, float t);
        float CatmullRomInterpolate(float p0, float p1, float p2, float p3, float t);
        
        // Bezier interpolation
        Vector4F BezierInterpolate(const Vector4F& p0, const Vector4F& p1, 
                                  const Vector4F& p2, const Vector4F& p3, float t);
        float BezierInterpolate(float p0, float p1, float p2, float p3, float t);
        
        // Utility functions
        float Clamp(float value, float min, float max);
        float SmoothStep(float edge0, float edge1, float x);
        float SmootherStep(float edge0, float edge1, float x);
        
        // Distance and angle calculations
        float Distance(const Vector4F& a, const Vector4F& b);
        float AngleDifference(float a, float b);
        Vector4F Normalize(const Vector4F& v);
        
        // Jitter detection
        bool IsJitter(const Vector4F& current, const Vector4F& previous, float threshold);
        float CalculateJitter(const Vector4F& current, const Vector4F& previous);
        
        // Lag compensation
        Vector4F CompensatePosition(const Vector4F& position, const Vector4F& velocity, float lagTime);
        float CompensateRotation(float rotation, float angularVelocity, float lagTime);
    }
}
