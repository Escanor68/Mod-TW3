#include "optimization/PositionInterpolation.h"
#include "utils/Logger.h"
#include <algorithm>
#include <cmath>

namespace Optimization
{
    // PositionInterpolation implementation
    PositionInterpolation::PositionInterpolation()
        : m_initialized(false), m_currentLatency(0.0f), m_currentPacketLoss(0.0f), m_currentJitter(0.0f)
    {
        m_lastUpdateTime = std::chrono::high_resolution_clock::now();
        m_lastCleanupTime = m_lastUpdateTime;
        
        LOG_INFO("Position interpolation system created");
    }

    PositionInterpolation::~PositionInterpolation()
    {
        Shutdown();
        LOG_INFO("Position interpolation system destroyed");
    }

    bool PositionInterpolation::Initialize(const InterpolationConfig& config)
    {
        if (m_initialized)
        {
            return true;
        }

        LOG_INFO("Initializing position interpolation system...");

        m_config = config;
        m_initialized = true;
        
        LOG_INFO("Position interpolation system initialized (type: " + 
                std::to_string(static_cast<int>(config.type)) + 
                ", duration: " + std::to_string(config.duration) + "s)");
        return true;
    }

    void PositionInterpolation::Shutdown()
    {
        if (!m_initialized)
        {
            return;
        }

        LOG_INFO("Shutting down position interpolation system...");
        
        // Clear all data
        m_playerSnapshots.clear();
        m_currentPositions.clear();
        
        m_initialized = false;
        LOG_INFO("Position interpolation system shutdown complete");
    }

    void PositionInterpolation::AddPositionSnapshot(const PositionSnapshot& snapshot)
    {
        if (!m_initialized || !snapshot.isValid)
        {
            return;
        }

        // Add snapshot to player's history
        m_playerSnapshots[snapshot.playerId].push_back(snapshot);
        
        // Sort by timestamp
        std::sort(m_playerSnapshots[snapshot.playerId].begin(), 
                 m_playerSnapshots[snapshot.playerId].end(),
                 [](const PositionSnapshot& a, const PositionSnapshot& b) {
                     return a.timestamp < b.timestamp;
                 });

        // Clean up old snapshots
        CleanupOldSnapshots();

        LOG_DEBUG("Added position snapshot for player " + std::to_string(snapshot.playerId) + 
                 " (sequence: " + std::to_string(snapshot.sequenceNumber) + ")");
    }

    void PositionInterpolation::AddPositionSnapshot(uint32_t playerId, const Vector4F& position, 
                                                   const Vector4F& velocity, float rotation, uint32_t sequenceNumber)
    {
        PositionSnapshot snapshot;
        snapshot.playerId = playerId;
        snapshot.position = position;
        snapshot.velocity = velocity;
        snapshot.rotation = rotation;
        snapshot.sequenceNumber = sequenceNumber;
        snapshot.timestamp = std::chrono::high_resolution_clock::now();
        snapshot.isValid = true;
        
        AddPositionSnapshot(snapshot);
    }

    InterpolatedPosition PositionInterpolation::InterpolatePosition(uint32_t playerId, float timeOffset)
    {
        if (!m_initialized)
        {
            return InterpolatedPosition();
        }

        auto startTime = std::chrono::high_resolution_clock::now();

        // Get snapshots for player
        std::vector<PositionSnapshot> snapshots = GetSnapshotsForPlayer(playerId);
        
        if (snapshots.empty())
        {
            return InterpolatedPosition();
        }

        // Calculate target time
        auto now = std::chrono::high_resolution_clock::now();
        float currentTime = std::chrono::duration<float>(now.time_since_epoch()).count();
        float targetTime = currentTime - timeOffset;

        InterpolatedPosition result;
        
        // Perform interpolation based on type
        switch (m_config.type)
        {
            case InterpolationType::Linear:
                result = InterpolateLinear(snapshots, targetTime);
                break;
            case InterpolationType::Cubic:
                result = InterpolateCubic(snapshots, targetTime);
                break;
            case InterpolationType::Hermite:
                result = InterpolateHermite(snapshots, targetTime);
                break;
            case InterpolationType::CatmullRom:
                result = InterpolateCatmullRom(snapshots, targetTime);
                break;
            case InterpolationType::Bezier:
                result = InterpolateBezier(snapshots, targetTime);
                break;
        }

        // Apply smoothing
        if (m_config.smoothing > 0.0f)
        {
            auto it = m_currentPositions.find(playerId);
            if (it != m_currentPositions.end())
            {
                result.position = InterpolationUtils::Lerp(it->second.position, result.position, m_config.smoothing);
                result.velocity = InterpolationUtils::Lerp(it->second.velocity, result.velocity, m_config.smoothing);
                result.rotation = InterpolationUtils::Lerp(it->second.rotation, result.rotation, m_config.smoothing);
            }
        }

        // Update current position
        m_currentPositions[playerId] = result;

        // Update statistics
        auto endTime = std::chrono::high_resolution_clock::now();
        float interpolationTime = std::chrono::duration<float>(endTime - startTime).count() * 1000.0f; // Convert to ms
        
        m_stats.totalInterpolations++;
        m_stats.averageInterpolationTime = (m_stats.averageInterpolationTime + interpolationTime) / 2.0f;
        m_stats.maxInterpolationTime = std::max(m_stats.maxInterpolationTime, interpolationTime);

        // Call callback
        if (m_positionUpdatedCallback)
        {
            m_positionUpdatedCallback(playerId, result);
        }

        return result;
    }

    std::vector<InterpolatedPosition> PositionInterpolation::InterpolateAllPositions(float timeOffset)
    {
        std::vector<InterpolatedPosition> results;
        
        for (const auto& pair : m_playerSnapshots)
        {
            InterpolatedPosition position = InterpolatePosition(pair.first, timeOffset);
            if (position.confidence > 0.0f)
            {
                results.push_back(position);
            }
        }
        
        return results;
    }

    InterpolatedPosition PositionInterpolation::ExtrapolatePosition(uint32_t playerId, float timeOffset)
    {
        if (!m_initialized || !m_config.enableExtrapolation)
        {
            return InterpolatedPosition();
        }

        std::vector<PositionSnapshot> snapshots = GetSnapshotsForPlayer(playerId);
        
        if (snapshots.size() < 2)
        {
            return InterpolatedPosition();
        }

        // Extrapolate from last two snapshots
        const PositionSnapshot& last = snapshots.back();
        const PositionSnapshot& secondLast = snapshots[snapshots.size() - 2];
        
        float timeDiff = std::chrono::duration<float>(last.timestamp - secondLast.timestamp).count();
        float extrapolationFactor = timeOffset / timeDiff;
        
        // Clamp extrapolation time
        extrapolationFactor = std::min(extrapolationFactor, m_config.extrapolationTime / timeDiff);
        
        InterpolatedPosition result;
        result.position = InterpolationUtils::Lerp(last.position, 
                                                 InterpolationUtils::Lerp(last.position, last.position, extrapolationFactor), 
                                                 extrapolationFactor);
        result.velocity = last.velocity;
        result.rotation = InterpolationUtils::Lerp(last.rotation, last.rotation, extrapolationFactor);
        result.confidence = std::max(0.0f, 1.0f - extrapolationFactor);
        result.isExtrapolated = true;
        
        m_stats.extrapolations++;
        
        return result;
    }

    Vector4F PositionInterpolation::GetCurrentPosition(uint32_t playerId) const
    {
        auto it = m_currentPositions.find(playerId);
        if (it != m_currentPositions.end())
        {
            return it->second.position;
        }
        return Vector4F{0.0f, 0.0f, 0.0f, 1.0f};
    }

    Vector4F PositionInterpolation::GetCurrentVelocity(uint32_t playerId) const
    {
        auto it = m_currentPositions.find(playerId);
        if (it != m_currentPositions.end())
        {
            return it->second.velocity;
        }
        return Vector4F{0.0f, 0.0f, 0.0f, 0.0f};
    }

    float PositionInterpolation::GetCurrentRotation(uint32_t playerId) const
    {
        auto it = m_currentPositions.find(playerId);
        if (it != m_currentPositions.end())
        {
            return it->second.rotation;
        }
        return 0.0f;
    }

    bool PositionInterpolation::HasPositionData(uint32_t playerId) const
    {
        return m_playerSnapshots.find(playerId) != m_playerSnapshots.end() && 
               !m_playerSnapshots.at(playerId).empty();
    }

    void PositionInterpolation::SetConfig(const InterpolationConfig& config)
    {
        m_config = config;
    }

    InterpolationConfig PositionInterpolation::GetConfig() const
    {
        return m_config;
    }

    void PositionInterpolation::SetInterpolationType(InterpolationType type)
    {
        m_config.type = type;
    }

    void PositionInterpolation::SetDuration(float duration)
    {
        m_config.duration = duration;
    }

    void PositionInterpolation::SetSmoothing(float smoothing)
    {
        m_config.smoothing = InterpolationUtils::Clamp(smoothing, 0.0f, 1.0f);
    }

    void PositionInterpolation::UpdateNetworkConditions(float latency, float packetLoss, float jitter)
    {
        m_currentLatency = latency;
        m_currentPacketLoss = packetLoss;
        m_currentJitter = jitter;
        
        if (m_config.enableAdaptiveInterpolation)
        {
            AdaptToNetworkConditions();
        }
    }

    void PositionInterpolation::AdaptToNetworkConditions()
    {
        // Adapt interpolation based on network conditions
        if (m_currentLatency > 100.0f) // High latency
        {
            // Increase interpolation duration for smoother movement
            m_config.duration = std::min(0.2f, m_config.duration * 1.1f);
        }
        else if (m_currentLatency < 20.0f) // Low latency
        {
            // Decrease interpolation duration for more responsive movement
            m_config.duration = std::max(0.05f, m_config.duration * 0.9f);
        }
        
        if (m_currentJitter > 0.01f) // High jitter
        {
            // Increase smoothing to reduce jitter
            m_config.smoothing = std::min(0.5f, m_config.smoothing * 1.2f);
        }
        else if (m_currentJitter < 0.001f) // Low jitter
        {
            // Decrease smoothing for more responsive movement
            m_config.smoothing = std::max(0.01f, m_config.smoothing * 0.8f);
        }
        
        LOG_DEBUG("Adapted interpolation - Duration: " + std::to_string(m_config.duration) + 
                 "s, Smoothing: " + std::to_string(m_config.smoothing));
    }

    InterpolationStats PositionInterpolation::GetStats() const
    {
        return m_stats;
    }

    void PositionInterpolation::ResetStats()
    {
        m_stats.Reset();
    }

    void PositionInterpolation::PrintStats() const
    {
        LOG_INFO("=== Position Interpolation Statistics ===");
        LOG_INFO("Total interpolations: " + std::to_string(m_stats.totalInterpolations));
        LOG_INFO("Extrapolations: " + std::to_string(m_stats.extrapolations));
        LOG_INFO("Jitter corrections: " + std::to_string(m_stats.jitterCorrections));
        LOG_INFO("Lag compensations: " + std::to_string(m_stats.lagCompensations));
        LOG_INFO("Average interpolation time: " + std::to_string(m_stats.averageInterpolationTime) + "ms");
        LOG_INFO("Max interpolation time: " + std::to_string(m_stats.maxInterpolationTime) + "ms");
        LOG_INFO("Average jitter: " + std::to_string(m_stats.averageJitter));
        LOG_INFO("Max jitter: " + std::to_string(m_stats.maxJitter));
        LOG_INFO("Average lag: " + std::to_string(m_stats.averageLag) + "ms");
        LOG_INFO("Max lag: " + std::to_string(m_stats.maxLag) + "ms");
        LOG_INFO("========================================");
    }

    void PositionInterpolation::SetPositionUpdatedCallback(PositionUpdatedCallback callback)
    {
        m_positionUpdatedCallback = callback;
    }

    void PositionInterpolation::SetJitterDetectedCallback(JitterDetectedCallback callback)
    {
        m_jitterDetectedCallback = callback;
    }

    // Internal interpolation methods
    InterpolatedPosition PositionInterpolation::InterpolateLinear(const std::vector<PositionSnapshot>& snapshots, float time)
    {
        if (snapshots.size() < 2)
        {
            return InterpolatedPosition();
        }

        // Find the two snapshots to interpolate between
        PositionSnapshot before = FindSnapshotAtTime(snapshots, time);
        PositionSnapshot after = FindNearestSnapshot(snapshots, time);
        
        if (!before.isValid || !after.isValid)
        {
            return InterpolatedPosition();
        }

        float timeDiff = std::chrono::duration<float>(after.timestamp - before.timestamp).count();
        if (timeDiff <= 0.0f)
        {
            return InterpolatedPosition();
        }

        float t = (time - std::chrono::duration<float>(before.timestamp.time_since_epoch()).count()) / timeDiff;
        t = InterpolationUtils::Clamp(t, 0.0f, 1.0f);

        InterpolatedPosition result;
        result.position = InterpolationUtils::Lerp(before.position, after.position, t);
        result.velocity = InterpolationUtils::Lerp(before.velocity, after.velocity, t);
        result.rotation = InterpolationUtils::Lerp(before.rotation, after.rotation, t);
        result.confidence = 1.0f - std::abs(t - 0.5f) * 2.0f; // Higher confidence near the middle
        
        return result;
    }

    InterpolatedPosition PositionInterpolation::InterpolateCubic(const std::vector<PositionSnapshot>& snapshots, float time)
    {
        if (snapshots.size() < 4)
        {
            return InterpolateLinear(snapshots, time);
        }

        // Find four snapshots for cubic interpolation
        PositionSnapshot p0, p1, p2, p3;
        // This is a simplified version - in practice, you'd find the appropriate snapshots
        
        InterpolatedPosition result;
        result.position = InterpolationUtils::CubicInterpolate(p0.position, p1.position, p2.position, p3.position, 0.5f);
        result.velocity = InterpolationUtils::CubicInterpolate(p0.velocity, p1.velocity, p2.velocity, p3.velocity, 0.5f);
        result.rotation = InterpolationUtils::CubicInterpolate(p0.rotation, p1.rotation, p2.rotation, p3.rotation, 0.5f);
        result.confidence = 0.8f;
        
        return result;
    }

    InterpolatedPosition PositionInterpolation::InterpolateHermite(const std::vector<PositionSnapshot>& snapshots, float time)
    {
        if (snapshots.size() < 2)
        {
            return InterpolatedPosition();
        }

        // Simplified Hermite interpolation
        PositionSnapshot p0 = snapshots[0];
        PositionSnapshot p1 = snapshots[1];
        
        InterpolatedPosition result;
        result.position = InterpolationUtils::HermiteInterpolate(p0.position, p1.position, p0.velocity, p1.velocity, 0.5f);
        result.velocity = InterpolationUtils::Lerp(p0.velocity, p1.velocity, 0.5f);
        result.rotation = InterpolationUtils::HermiteInterpolate(p0.rotation, p1.rotation, 0.0f, 0.0f, 0.5f);
        result.confidence = 0.9f;
        
        return result;
    }

    InterpolatedPosition PositionInterpolation::InterpolateCatmullRom(const std::vector<PositionSnapshot>& snapshots, float time)
    {
        if (snapshots.size() < 4)
        {
            return InterpolateLinear(snapshots, time);
        }

        // Simplified Catmull-Rom interpolation
        PositionSnapshot p0 = snapshots[0];
        PositionSnapshot p1 = snapshots[1];
        PositionSnapshot p2 = snapshots[2];
        PositionSnapshot p3 = snapshots[3];
        
        InterpolatedPosition result;
        result.position = InterpolationUtils::CatmullRomInterpolate(p0.position, p1.position, p2.position, p3.position, 0.5f);
        result.velocity = InterpolationUtils::Lerp(p1.velocity, p2.velocity, 0.5f);
        result.rotation = InterpolationUtils::CatmullRomInterpolate(p0.rotation, p1.rotation, p2.rotation, p3.rotation, 0.5f);
        result.confidence = 0.85f;
        
        return result;
    }

    InterpolatedPosition PositionInterpolation::InterpolateBezier(const std::vector<PositionSnapshot>& snapshots, float time)
    {
        if (snapshots.size() < 4)
        {
            return InterpolateLinear(snapshots, time);
        }

        // Simplified Bezier interpolation
        PositionSnapshot p0 = snapshots[0];
        PositionSnapshot p1 = snapshots[1];
        PositionSnapshot p2 = snapshots[2];
        PositionSnapshot p3 = snapshots[3];
        
        InterpolatedPosition result;
        result.position = InterpolationUtils::BezierInterpolate(p0.position, p1.position, p2.position, p3.position, 0.5f);
        result.velocity = InterpolationUtils::Lerp(p1.velocity, p2.velocity, 0.5f);
        result.rotation = InterpolationUtils::BezierInterpolate(p0.rotation, p1.rotation, p2.rotation, p3.rotation, 0.5f);
        result.confidence = 0.8f;
        
        return result;
    }

    std::vector<PositionSnapshot> PositionInterpolation::GetSnapshotsForPlayer(uint32_t playerId) const
    {
        auto it = m_playerSnapshots.find(playerId);
        if (it != m_playerSnapshots.end())
        {
            return it->second;
        }
        return std::vector<PositionSnapshot>();
    }

    PositionSnapshot PositionInterpolation::FindSnapshotAtTime(const std::vector<PositionSnapshot>& snapshots, float time) const
    {
        for (const auto& snapshot : snapshots)
        {
            float snapshotTime = std::chrono::duration<float>(snapshot.timestamp.time_since_epoch()).count();
            if (snapshotTime <= time)
            {
                return snapshot;
            }
        }
        return PositionSnapshot();
    }

    PositionSnapshot PositionInterpolation::FindNearestSnapshot(const std::vector<PositionSnapshot>& snapshots, float time) const
    {
        if (snapshots.empty())
        {
            return PositionSnapshot();
        }

        PositionSnapshot nearest = snapshots[0];
        float minDiff = std::abs(std::chrono::duration<float>(snapshots[0].timestamp.time_since_epoch()).count() - time);

        for (const auto& snapshot : snapshots)
        {
            float snapshotTime = std::chrono::duration<float>(snapshot.timestamp.time_since_epoch()).count();
            float diff = std::abs(snapshotTime - time);
            if (diff < minDiff)
            {
                minDiff = diff;
                nearest = snapshot;
            }
        }

        return nearest;
    }

    bool PositionInterpolation::DetectJitter(const PositionSnapshot& current, const PositionSnapshot& previous) const
    {
        if (!m_config.enableJitterReduction)
        {
            return false;
        }

        float jitter = InterpolationUtils::CalculateJitter(current.position, previous.position);
        return jitter > m_config.jitterThreshold;
    }

    PositionSnapshot PositionInterpolation::CorrectJitter(const PositionSnapshot& snapshot, const PositionSnapshot& previous) const
    {
        PositionSnapshot corrected = snapshot;
        
        // Simple jitter correction - smooth the position
        corrected.position = InterpolationUtils::Lerp(previous.position, snapshot.position, 0.5f);
        
        m_stats.jitterCorrections++;
        m_stats.averageJitter = (m_stats.averageJitter + InterpolationUtils::CalculateJitter(snapshot.position, previous.position)) / 2.0f;
        m_stats.maxJitter = std::max(m_stats.maxJitter, InterpolationUtils::CalculateJitter(snapshot.position, previous.position));
        
        if (m_jitterDetectedCallback)
        {
            m_jitterDetectedCallback(snapshot.playerId, InterpolationUtils::CalculateJitter(snapshot.position, previous.position));
        }
        
        return corrected;
    }

    PositionSnapshot PositionInterpolation::CompensateForLag(const PositionSnapshot& snapshot, float lagTime) const
    {
        if (!m_config.enableLagCompensation)
        {
            return snapshot;
        }

        PositionSnapshot compensated = snapshot;
        compensated.position = InterpolationUtils::CompensatePosition(snapshot.position, snapshot.velocity, lagTime);
        compensated.rotation = InterpolationUtils::CompensateRotation(snapshot.rotation, 0.0f, lagTime);
        
        m_stats.lagCompensations++;
        m_stats.averageLag = (m_stats.averageLag + lagTime * 1000.0f) / 2.0f; // Convert to ms
        m_stats.maxLag = std::max(m_stats.maxLag, lagTime * 1000.0f);
        
        return compensated;
    }

    PositionSnapshot PositionInterpolation::ExtrapolateFromSnapshots(const std::vector<PositionSnapshot>& snapshots, float time) const
    {
        if (snapshots.size() < 2)
        {
            return PositionSnapshot();
        }

        const PositionSnapshot& last = snapshots.back();
        const PositionSnapshot& secondLast = snapshots[snapshots.size() - 2];
        
        float timeDiff = std::chrono::duration<float>(last.timestamp - secondLast.timestamp).count();
        float extrapolationTime = time - std::chrono::duration<float>(last.timestamp.time_since_epoch()).count();
        float extrapolationFactor = extrapolationTime / timeDiff;
        
        PositionSnapshot extrapolated = last;
        extrapolated.position = InterpolationUtils::CompensatePosition(last.position, last.velocity, extrapolationTime);
        extrapolated.rotation = InterpolationUtils::CompensateRotation(last.rotation, 0.0f, extrapolationTime);
        
        return extrapolated;
    }

    void PositionInterpolation::CleanupOldSnapshots()
    {
        auto now = std::chrono::high_resolution_clock::now();
        float currentTime = std::chrono::duration<float>(now.time_since_epoch()).count();
        float cutoffTime = currentTime - m_config.duration * 2.0f; // Keep snapshots for 2x duration
        
        for (auto& pair : m_playerSnapshots)
        {
            auto& snapshots = pair.second;
            snapshots.erase(
                std::remove_if(snapshots.begin(), snapshots.end(),
                    [cutoffTime](const PositionSnapshot& snapshot) {
                        float snapshotTime = std::chrono::duration<float>(snapshot.timestamp.time_since_epoch()).count();
                        return snapshotTime < cutoffTime;
                    }),
                snapshots.end()
            );
        }
    }

    void PositionInterpolation::CleanupPlayerData(uint32_t playerId)
    {
        m_playerSnapshots.erase(playerId);
        m_currentPositions.erase(playerId);
    }

    // Interpolation utilities implementation
    namespace InterpolationUtils
    {
        Vector4F Lerp(const Vector4F& a, const Vector4F& b, float t)
        {
            Vector4F result;
            result.x = a.x + (b.x - a.x) * t;
            result.y = a.y + (b.y - a.y) * t;
            result.z = a.z + (b.z - a.z) * t;
            result.w = 1.0f;
            return result;
        }

        float Lerp(float a, float b, float t)
        {
            return a + (b - a) * t;
        }

        Vector4F Slerp(const Vector4F& a, const Vector4F& b, float t)
        {
            // Simple spherical interpolation (can be enhanced)
            return Lerp(a, b, t);
        }

        Vector4F CubicInterpolate(const Vector4F& p0, const Vector4F& p1, 
                                 const Vector4F& p2, const Vector4F& p3, float t)
        {
            float t2 = t * t;
            float t3 = t2 * t;
            
            Vector4F result;
            result.x = 0.5f * ((2.0f * p1.x) + (-p0.x + p2.x) * t + (2.0f * p0.x - 5.0f * p1.x + 4.0f * p2.x - p3.x) * t2 + (-p0.x + 3.0f * p1.x - 3.0f * p2.x + p3.x) * t3);
            result.y = 0.5f * ((2.0f * p1.y) + (-p0.y + p2.y) * t + (2.0f * p0.y - 5.0f * p1.y + 4.0f * p2.y - p3.y) * t2 + (-p0.y + 3.0f * p1.y - 3.0f * p2.y + p3.y) * t3);
            result.z = 0.5f * ((2.0f * p1.z) + (-p0.z + p2.z) * t + (2.0f * p0.z - 5.0f * p1.z + 4.0f * p2.z - p3.z) * t2 + (-p0.z + 3.0f * p1.z - 3.0f * p2.z + p3.z) * t3);
            result.w = 1.0f;
            return result;
        }

        float CubicInterpolate(float p0, float p1, float p2, float p3, float t)
        {
            float t2 = t * t;
            float t3 = t2 * t;
            return 0.5f * ((2.0f * p1) + (-p0 + p2) * t + (2.0f * p0 - 5.0f * p1 + 4.0f * p2 - p3) * t2 + (-p0 + 3.0f * p1 - 3.0f * p2 + p3) * t3);
        }

        Vector4F HermiteInterpolate(const Vector4F& p0, const Vector4F& p1, 
                                   const Vector4F& t0, const Vector4F& t1, float t)
        {
            float t2 = t * t;
            float t3 = t2 * t;
            
            Vector4F result;
            result.x = (2.0f * t3 - 3.0f * t2 + 1.0f) * p0.x + (t3 - 2.0f * t2 + t) * t0.x + (-2.0f * t3 + 3.0f * t2) * p1.x + (t3 - t2) * t1.x;
            result.y = (2.0f * t3 - 3.0f * t2 + 1.0f) * p0.y + (t3 - 2.0f * t2 + t) * t0.y + (-2.0f * t3 + 3.0f * t2) * p1.y + (t3 - t2) * t1.y;
            result.z = (2.0f * t3 - 3.0f * t2 + 1.0f) * p0.z + (t3 - 2.0f * t2 + t) * t0.z + (-2.0f * t3 + 3.0f * t2) * p1.z + (t3 - t2) * t1.z;
            result.w = 1.0f;
            return result;
        }

        float HermiteInterpolate(float p0, float p1, float t0, float t1, float t)
        {
            float t2 = t * t;
            float t3 = t2 * t;
            return (2.0f * t3 - 3.0f * t2 + 1.0f) * p0 + (t3 - 2.0f * t2 + t) * t0 + (-2.0f * t3 + 3.0f * t2) * p1 + (t3 - t2) * t1;
        }

        Vector4F CatmullRomInterpolate(const Vector4F& p0, const Vector4F& p1, 
                                      const Vector4F& p2, const Vector4F& p3, float t)
        {
            float t2 = t * t;
            float t3 = t2 * t;
            
            Vector4F result;
            result.x = 0.5f * ((2.0f * p1.x) + (-p0.x + p2.x) * t + (2.0f * p0.x - 5.0f * p1.x + 4.0f * p2.x - p3.x) * t2 + (-p0.x + 3.0f * p1.x - 3.0f * p2.x + p3.x) * t3);
            result.y = 0.5f * ((2.0f * p1.y) + (-p0.y + p2.y) * t + (2.0f * p0.y - 5.0f * p1.y + 4.0f * p2.y - p3.y) * t2 + (-p0.y + 3.0f * p1.y - 3.0f * p2.y + p3.y) * t3);
            result.z = 0.5f * ((2.0f * p1.z) + (-p0.z + p2.z) * t + (2.0f * p0.z - 5.0f * p1.z + 4.0f * p2.z - p3.z) * t2 + (-p0.z + 3.0f * p1.z - 3.0f * p2.z + p3.z) * t3);
            result.w = 1.0f;
            return result;
        }

        float CatmullRomInterpolate(float p0, float p1, float p2, float p3, float t)
        {
            float t2 = t * t;
            float t3 = t2 * t;
            return 0.5f * ((2.0f * p1) + (-p0 + p2) * t + (2.0f * p0 - 5.0f * p1 + 4.0f * p2 - p3) * t2 + (-p0 + 3.0f * p1 - 3.0f * p2 + p3) * t3);
        }

        Vector4F BezierInterpolate(const Vector4F& p0, const Vector4F& p1, 
                                  const Vector4F& p2, const Vector4F& p3, float t)
        {
            float u = 1.0f - t;
            float tt = t * t;
            float uu = u * u;
            float uuu = uu * u;
            float ttt = tt * t;
            
            Vector4F result;
            result.x = uuu * p0.x + 3.0f * uu * t * p1.x + 3.0f * u * tt * p2.x + ttt * p3.x;
            result.y = uuu * p0.y + 3.0f * uu * t * p1.y + 3.0f * u * tt * p2.y + ttt * p3.y;
            result.z = uuu * p0.z + 3.0f * uu * t * p1.z + 3.0f * u * tt * p2.z + ttt * p3.z;
            result.w = 1.0f;
            return result;
        }

        float BezierInterpolate(float p0, float p1, float p2, float p3, float t)
        {
            float u = 1.0f - t;
            float tt = t * t;
            float uu = u * u;
            float uuu = uu * u;
            float ttt = tt * t;
            return uuu * p0 + 3.0f * uu * t * p1 + 3.0f * u * tt * p2 + ttt * p3;
        }

        float Clamp(float value, float min, float max)
        {
            return std::max(min, std::min(max, value));
        }

        float SmoothStep(float edge0, float edge1, float x)
        {
            float t = Clamp((x - edge0) / (edge1 - edge0), 0.0f, 1.0f);
            return t * t * (3.0f - 2.0f * t);
        }

        float SmootherStep(float edge0, float edge1, float x)
        {
            float t = Clamp((x - edge0) / (edge1 - edge0), 0.0f, 1.0f);
            return t * t * t * (t * (t * 6.0f - 15.0f) + 10.0f);
        }

        float Distance(const Vector4F& a, const Vector4F& b)
        {
            float dx = a.x - b.x;
            float dy = a.y - b.y;
            float dz = a.z - b.z;
            return std::sqrt(dx * dx + dy * dy + dz * dz);
        }

        float AngleDifference(float a, float b)
        {
            float diff = b - a;
            while (diff > 180.0f) diff -= 360.0f;
            while (diff < -180.0f) diff += 360.0f;
            return diff;
        }

        Vector4F Normalize(const Vector4F& v)
        {
            float length = std::sqrt(v.x * v.x + v.y * v.y + v.z * v.z);
            if (length > 0.0f)
            {
                Vector4F result;
                result.x = v.x / length;
                result.y = v.y / length;
                result.z = v.z / length;
                result.w = 1.0f;
                return result;
            }
            return v;
        }

        bool IsJitter(const Vector4F& current, const Vector4F& previous, float threshold)
        {
            return CalculateJitter(current, previous) > threshold;
        }

        float CalculateJitter(const Vector4F& current, const Vector4F& previous)
        {
            return Distance(current, previous);
        }

        Vector4F CompensatePosition(const Vector4F& position, const Vector4F& velocity, float lagTime)
        {
            Vector4F compensated = position;
            compensated.x += velocity.x * lagTime;
            compensated.y += velocity.y * lagTime;
            compensated.z += velocity.z * lagTime;
            return compensated;
        }

        float CompensateRotation(float rotation, float angularVelocity, float lagTime)
        {
            return rotation + angularVelocity * lagTime;
        }
    }
}
