#include "optimization/OptimizedNetworkProtocol.h"
#include "utils/Logger.h"
#include <algorithm>
#include <numeric>
#include <cstring>

namespace Optimization
{
    // OptimizedNetworkProtocol implementation
    OptimizedNetworkProtocol::OptimizedNetworkProtocol()
        : m_initialized(false), m_nextPacketId(1), m_nextSequenceNumber(1),
          m_compressionEnabled(true), m_batchingEnabled(true), m_predictionEnabled(true), m_priorityEnabled(true),
          m_compressionLevel(CompressionLevel::Balanced), m_currentLatency(0.0f), m_currentPacketLoss(0.0f), m_currentBandwidth(0.0f)
    {
        m_lastUpdateTime = std::chrono::high_resolution_clock::now();
        m_lastStatsUpdate = m_lastUpdateTime;
        
        LOG_INFO("Optimized network protocol created");
    }

    OptimizedNetworkProtocol::~OptimizedNetworkProtocol()
    {
        Shutdown();
        LOG_INFO("Optimized network protocol destroyed");
    }

    bool OptimizedNetworkProtocol::Initialize()
    {
        if (m_initialized)
        {
            return true;
        }

        LOG_INFO("Initializing optimized network protocol...");

        // Initialize compression system
        m_compression = std::make_unique<DataCompression>();
        if (!m_compression->Initialize())
        {
            LOG_ERROR("Failed to initialize data compression");
            return false;
        }

        // Initialize batching system
        m_batching = std::make_unique<SmartBatching>();
        if (!m_batching->Initialize())
        {
            LOG_ERROR("Failed to initialize smart batching");
            return false;
        }

        // Initialize traffic manager
        m_trafficManager = std::make_unique<NetworkTrafficManager>();
        if (!m_trafficManager->Initialize())
        {
            LOG_ERROR("Failed to initialize network traffic manager");
            return false;
        }

        // Initialize prediction manager
        m_predictionManager = std::make_unique<MovementPredictionManager>();

        m_initialized = true;
        LOG_INFO("Optimized network protocol initialized successfully");
        return true;
    }

    void OptimizedNetworkProtocol::Shutdown()
    {
        if (!m_initialized)
        {
            return;
        }

        LOG_INFO("Shutting down optimized network protocol...");

        // Clear all queues and maps
        while (!m_sendQueue.empty()) m_sendQueue.pop();
        m_pendingPackets.clear();
        m_fragments.clear();

        // Shutdown components
        if (m_compression) m_compression->Shutdown();
        if (m_batching) m_batching->Shutdown();
        if (m_trafficManager) m_trafficManager->Shutdown();

        m_initialized = false;
        LOG_INFO("Optimized network protocol shutdown complete");
    }

    void OptimizedNetworkProtocol::SendMessage(const PrioritizedMessage& message)
    {
        if (!m_initialized)
        {
            return;
        }

        // Process message through traffic manager
        m_trafficManager->ProcessMessage(message);

        LOG_DEBUG("Message queued for sending (ID: " + std::to_string(message.messageId) + 
                 ", type: " + std::to_string(static_cast<int>(message.type)) + ")");
    }

    void OptimizedNetworkProtocol::SendMessages(const std::vector<PrioritizedMessage>& messages)
    {
        if (!m_initialized || messages.empty())
        {
            return;
        }

        for (const auto& message : messages)
        {
            SendMessage(message);
        }

        LOG_DEBUG("Queued " + std::to_string(messages.size()) + " messages for sending");
    }

    std::vector<PrioritizedMessage> OptimizedNetworkProtocol::ProcessReceivedPacket(const NetworkPacket& packet)
    {
        if (!m_initialized)
        {
            return {};
        }

        // Validate packet
        if (!ValidatePacket(packet))
        {
            LOG_WARNING("Received invalid packet (ID: " + std::to_string(packet.header.packetId) + ")");
            return {};
        }

        // Update statistics
        m_stats.packetsReceived++;
        m_stats.bytesReceived += packet.data.size();

        // Extract messages from packet
        std::vector<PrioritizedMessage> messages = ExtractMessages(packet);

        // Process each message
        for (const auto& message : messages)
        {
            if (m_messageReceivedCallback)
            {
                m_messageReceivedCallback(message);
            }
        }

        LOG_DEBUG("Processed packet " + std::to_string(packet.header.packetId) + 
                 " with " + std::to_string(messages.size()) + " messages");

        return messages;
    }

    std::vector<NetworkPacket> OptimizedNetworkProtocol::GetPacketsToSend()
    {
        if (!m_initialized)
        {
            return {};
        }

        std::vector<NetworkPacket> packets;

        // Get messages from traffic manager
        std::vector<PrioritizedMessage> messages = m_trafficManager->GetMessagesToSend(MAX_MESSAGES_PER_PACKET);
        
        if (messages.empty())
        {
            return packets;
        }

        // Create packets from messages
        packets = CreatePackets(messages);

        // Add to send queue
        for (const auto& packet : packets)
        {
            m_sendQueue.push(packet);
            m_pendingPackets[packet.header.packetId] = packet;
        }

        // Update statistics
        for (const auto& packet : packets)
        {
            m_stats.packetsSent++;
            m_stats.bytesSent += packet.data.size();
        }

        LOG_DEBUG("Created " + std::to_string(packets.size()) + " packets for sending");

        return packets;
    }

    void OptimizedNetworkProtocol::MarkPacketSent(uint32_t packetId)
    {
        auto it = m_pendingPackets.find(packetId);
        if (it != m_pendingPackets.end())
        {
            it->second.sendTime = std::chrono::high_resolution_clock::now();
            
            if (m_packetSentCallback)
            {
                m_packetSentCallback(it->second);
            }
        }
    }

    void OptimizedNetworkProtocol::MarkPacketAcknowledged(uint32_t packetId)
    {
        auto it = m_pendingPackets.find(packetId);
        if (it != m_pendingPackets.end())
        {
            it->second.ackTime = std::chrono::high_resolution_clock::now();
            it->second.isAcknowledged = true;
            
            // Calculate latency
            auto latency = std::chrono::duration<float>(it->second.ackTime - it->second.sendTime).count() * 1000.0f; // Convert to ms
            m_stats.averageLatency = (m_stats.averageLatency + latency) / 2.0f;
            m_stats.maxLatency = std::max(m_stats.maxLatency, latency);
            
            // Remove from pending packets
            m_pendingPackets.erase(it);
        }
    }

    void OptimizedNetworkProtocol::MarkPacketLost(uint32_t packetId)
    {
        auto it = m_pendingPackets.find(packetId);
        if (it != m_pendingPackets.end())
        {
            m_stats.packetsLost++;
            m_stats.packetLossRate = static_cast<float>(m_stats.packetsLost) / static_cast<float>(m_stats.packetsSent);
            
            if (m_packetLostCallback)
            {
                m_packetLostCallback(packetId);
            }
            
            // Remove from pending packets
            m_pendingPackets.erase(it);
        }
    }

    void OptimizedNetworkProtocol::EnableCompression(bool enable)
    {
        m_compressionEnabled = enable;
        LOG_INFO("Compression " + (enable ? "enabled" : "disabled"));
    }

    void OptimizedNetworkProtocol::EnableBatching(bool enable)
    {
        m_batchingEnabled = enable;
        LOG_INFO("Batching " + (enable ? "enabled" : "disabled"));
    }

    void OptimizedNetworkProtocol::EnablePrediction(bool enable)
    {
        m_predictionEnabled = enable;
        LOG_INFO("Prediction " + (enable ? "enabled" : "disabled"));
    }

    void OptimizedNetworkProtocol::EnablePrioritySystem(bool enable)
    {
        m_priorityEnabled = enable;
        LOG_INFO("Priority system " + (enable ? "enabled" : "disabled"));
    }

    void OptimizedNetworkProtocol::SetCompressionLevel(CompressionLevel level)
    {
        m_compressionLevel = level;
        if (m_compression)
        {
            m_compression->SetDefaultLevel(level);
        }
    }

    void OptimizedNetworkProtocol::SetBatchConfig(const BatchConfig& config)
    {
        m_batchConfig = config;
        if (m_batching)
        {
            m_batching->SetConfig(config);
        }
    }

    void OptimizedNetworkProtocol::SetPriorityConfig(const std::map<Networking::MessageTypes, MessageClassification>& classifications)
    {
        if (m_trafficManager)
        {
            m_trafficManager->SetConfig(classifications);
        }
    }

    void OptimizedNetworkProtocol::SetPredictionConfig(const PredictionConfig& config)
    {
        m_predictionConfig = config;
        if (m_predictionManager)
        {
            m_predictionManager->SetGlobalConfig(config);
        }
    }

    void OptimizedNetworkProtocol::UpdateNetworkConditions(float latency, float packetLoss, float bandwidth)
    {
        m_currentLatency = latency;
        m_currentPacketLoss = packetLoss;
        m_currentBandwidth = bandwidth;

        // Update batching based on network conditions
        if (m_batching)
        {
            m_batching->UpdateNetworkConditions(packetLoss, latency);
        }

        LOG_DEBUG("Network conditions updated - Latency: " + std::to_string(latency) + 
                 "ms, Packet Loss: " + std::to_string(packetLoss * 100.0f) + 
                 "%, Bandwidth: " + std::to_string(bandwidth) + " bps");
    }

    void OptimizedNetworkProtocol::OptimizeForLatency()
    {
        // Reduce batching interval for lower latency
        BatchConfig config = m_batchConfig;
        config.intervalMs = 16.0f; // 60 FPS
        config.maxWaitTime = 50.0f;
        SetBatchConfig(config);

        // Disable compression for critical messages
        EnableCompression(false);

        LOG_INFO("Optimized for low latency");
    }

    void OptimizedNetworkProtocol::OptimizeForThroughput()
    {
        // Increase batching for higher throughput
        BatchConfig config = m_batchConfig;
        config.intervalMs = 100.0f;
        config.maxSize = 8192;
        config.maxMessages = 100;
        SetBatchConfig(config);

        // Enable compression
        EnableCompression(true);
        SetCompressionLevel(CompressionLevel::High);

        LOG_INFO("Optimized for high throughput");
    }

    void OptimizedNetworkProtocol::OptimizeForReliability()
    {
        // Enable all reliability features
        EnableCompression(true);
        EnableBatching(true);
        EnablePrioritySystem(true);

        // Set high compression for reliability
        SetCompressionLevel(CompressionLevel::Maximum);

        LOG_INFO("Optimized for high reliability");
    }

    NetworkStats OptimizedNetworkProtocol::GetStats() const
    {
        return m_stats;
    }

    void OptimizedNetworkProtocol::ResetStats()
    {
        m_stats.Reset();
    }

    void OptimizedNetworkProtocol::PrintStats() const
    {
        LOG_INFO("=== Optimized Network Protocol Statistics ===");
        LOG_INFO("Packets sent: " + std::to_string(m_stats.packetsSent));
        LOG_INFO("Packets received: " + std::to_string(m_stats.packetsReceived));
        LOG_INFO("Packets lost: " + std::to_string(m_stats.packetsLost));
        LOG_INFO("Packets retransmitted: " + std::to_string(m_stats.packetsRetransmitted));
        LOG_INFO("Bytes sent: " + std::to_string(m_stats.bytesSent));
        LOG_INFO("Bytes received: " + std::to_string(m_stats.bytesReceived));
        LOG_INFO("Average latency: " + std::to_string(m_stats.averageLatency) + "ms");
        LOG_INFO("Max latency: " + std::to_string(m_stats.maxLatency) + "ms");
        LOG_INFO("Packet loss rate: " + std::to_string(m_stats.packetLossRate * 100.0f) + "%");
        LOG_INFO("Throughput: " + std::to_string(m_stats.throughput) + " bps");
        LOG_INFO("Compression ratio: " + std::to_string(m_stats.compressionRatio) + "%");
        LOG_INFO("Batch efficiency: " + std::to_string(m_stats.batchEfficiency) + "%");
        LOG_INFO("=============================================");
    }

    void OptimizedNetworkProtocol::SetMessageReceivedCallback(MessageReceivedCallback callback)
    {
        m_messageReceivedCallback = callback;
    }

    void OptimizedNetworkProtocol::SetPacketSentCallback(PacketSentCallback callback)
    {
        m_packetSentCallback = callback;
    }

    void OptimizedNetworkProtocol::SetPacketLostCallback(PacketLostCallback callback)
    {
        m_packetLostCallback = callback;
    }

    // Internal processing methods
    NetworkPacket OptimizedNetworkProtocol::CreatePacket(const std::vector<PrioritizedMessage>& messages)
    {
        NetworkPacket packet;
        
        // Set packet header
        packet.header.packetId = m_nextPacketId++;
        packet.header.sequenceNumber = m_nextSequenceNumber++;
        packet.header.timestamp = static_cast<uint32_t>(std::chrono::duration_cast<std::chrono::milliseconds>(
            std::chrono::high_resolution_clock::now().time_since_epoch()).count());
        packet.header.type = PacketType::Data;
        
        // Serialize messages
        std::vector<uint8_t> messageData;
        for (const auto& message : messages)
        {
            // Message header: ID(4) + Type(4) + DataSize(4) + Data
            uint32_t messageId = message.messageId;
            uint32_t messageType = static_cast<uint32_t>(message.type);
            uint32_t dataSize = static_cast<uint32_t>(message.data.size());
            
            messageData.insert(messageData.end(), reinterpret_cast<uint8_t*>(&messageId), 
                             reinterpret_cast<uint8_t*>(&messageId) + 4);
            messageData.insert(messageData.end(), reinterpret_cast<uint8_t*>(&messageType), 
                             reinterpret_cast<uint8_t*>(&messageType) + 4);
            messageData.insert(messageData.end(), reinterpret_cast<uint8_t*>(&dataSize), 
                             reinterpret_cast<uint8_t*>(&dataSize) + 4);
            messageData.insert(messageData.end(), message.data.begin(), message.data.end());
        }
        
        // Compress if enabled
        if (m_compressionEnabled && m_compression)
        {
            std::vector<uint8_t> compressedData = m_compression->Compress(messageData, 
                                                                         CompressionAlgorithm::LZ4, 
                                                                         m_compressionLevel);
            if (compressedData.size() < messageData.size())
            {
                packet.data = compressedData;
                packet.header.flags |= static_cast<uint16_t>(PacketFlags::Compressed);
            }
            else
            {
                packet.data = messageData;
            }
        }
        else
        {
            packet.data = messageData;
        }
        
        // Set packet size
        packet.header.dataSize = static_cast<uint32_t>(packet.data.size());
        
        // Calculate checksum
        packet.header.checksum = CalculateChecksum(packet.data);
        
        return packet;
    }

    std::vector<NetworkPacket> OptimizedNetworkProtocol::CreatePackets(const std::vector<PrioritizedMessage>& messages)
    {
        std::vector<NetworkPacket> packets;
        
        if (messages.empty())
        {
            return packets;
        }

        // Group messages by priority if enabled
        std::vector<PrioritizedMessage> processedMessages = messages;
        if (m_priorityEnabled)
        {
            PrioritizeMessages(processedMessages);
        }

        // Batch messages if enabled
        if (m_batchingEnabled && m_batching)
        {
            for (const auto& message : processedMessages)
            {
                m_batching->AddMessage(message);
            }
            
            // Get batched messages
            std::vector<BatchedMessage> batches = m_batching->GetReadyBatches();
            for (const auto& batch : batches)
            {
                if (!batch.messages.empty())
                {
                    NetworkPacket packet = CreatePacket(batch.messages);
                    packet.header.flags |= static_cast<uint16_t>(PacketFlags::Batch);
                    packets.push_back(packet);
                }
            }
        }
        else
        {
            // Create individual packets
            for (const auto& message : processedMessages)
            {
                NetworkPacket packet = CreatePacket({message});
                packets.push_back(packet);
            }
        }

        return packets;
    }

    std::vector<PrioritizedMessage> OptimizedNetworkProtocol::ExtractMessages(const NetworkPacket& packet)
    {
        std::vector<PrioritizedMessage> messages;
        
        if (packet.data.empty())
        {
            return messages;
        }

        std::vector<uint8_t> messageData = packet.data;
        
        // Decompress if needed
        if (packet.header.flags & static_cast<uint16_t>(PacketFlags::Compressed))
        {
            if (m_compression)
            {
                messageData = m_compression->Decompress(packet.data, CompressionAlgorithm::LZ4);
            }
        }

        // Extract messages
        size_t offset = 0;
        while (offset + 12 <= messageData.size()) // ID + Type + DataSize
        {
            PrioritizedMessage message;
            
            // Message ID
            std::memcpy(&message.messageId, &messageData[offset], 4);
            offset += 4;
            
            // Message type
            uint32_t messageType;
            std::memcpy(&messageType, &messageData[offset], 4);
            offset += 4;
            message.type = static_cast<Networking::MessageTypes>(messageType);
            
            // Data size
            uint32_t dataSize;
            std::memcpy(&dataSize, &messageData[offset], 4);
            offset += 4;
            
            if (offset + dataSize > messageData.size())
            {
                break; // Invalid data
            }
            
            // Data
            message.data.assign(messageData.begin() + offset, messageData.begin() + offset + dataSize);
            offset += dataSize;
            
            messages.push_back(message);
        }

        return messages;
    }

    void OptimizedNetworkProtocol::OptimizePacket(NetworkPacket& packet)
    {
        // Optimize packet based on network conditions
        if (m_currentLatency > 100.0f) // High latency
        {
            // Increase packet size to reduce overhead
            if (packet.data.size() < 512)
            {
                // Could batch with other small packets
            }
        }
        else if (m_currentLatency < 20.0f) // Low latency
        {
            // Reduce packet size for faster transmission
            if (packet.data.size() > 1024)
            {
                // Could split large packets
            }
        }
    }

    void OptimizedNetworkProtocol::CompressPacket(NetworkPacket& packet)
    {
        if (!m_compressionEnabled || !m_compression)
        {
            return;
        }

        std::vector<uint8_t> compressedData = m_compression->Compress(packet.data, 
                                                                     CompressionAlgorithm::LZ4, 
                                                                     m_compressionLevel);
        
        if (compressedData.size() < packet.data.size())
        {
            packet.data = compressedData;
            packet.header.dataSize = static_cast<uint32_t>(packet.data.size());
            packet.header.flags |= static_cast<uint16_t>(PacketFlags::Compressed);
        }
    }

    void OptimizedNetworkProtocol::BatchMessages(std::vector<PrioritizedMessage>& messages)
    {
        if (!m_batchingEnabled || !m_batching)
        {
            return;
        }

        // This would be handled by the batching system
        // For now, just sort by priority
        std::sort(messages.begin(), messages.end(), [](const PrioritizedMessage& a, const PrioritizedMessage& b) {
            return static_cast<int>(a.classification.priority) < static_cast<int>(b.classification.priority);
        });
    }

    void OptimizedNetworkProtocol::PrioritizeMessages(std::vector<PrioritizedMessage>& messages)
    {
        if (!m_priorityEnabled)
        {
            return;
        }

        // Sort by priority and urgency
        std::sort(messages.begin(), messages.end(), [](const PrioritizedMessage& a, const PrioritizedMessage& b) {
            if (a.classification.priority != b.classification.priority)
            {
                return static_cast<int>(a.classification.priority) < static_cast<int>(b.classification.priority);
            }
            return static_cast<int>(a.classification.urgency) < static_cast<int>(b.classification.urgency);
        });
    }

    bool OptimizedNetworkProtocol::ValidatePacket(const NetworkPacket& packet) const
    {
        // Check magic number
        if (packet.header.magic != 0xDEADBEEF)
        {
            return false;
        }

        // Check version
        if (packet.header.version != PROTOCOL_VERSION)
        {
            return false;
        }

        // Check data size
        if (packet.header.dataSize != packet.data.size())
        {
            return false;
        }

        // Verify checksum
        if (!VerifyChecksum(packet))
        {
            return false;
        }

        return true;
    }

    uint32_t OptimizedNetworkProtocol::CalculateChecksum(const std::vector<uint8_t>& data) const
    {
        uint32_t checksum = 0;
        for (size_t i = 0; i < data.size(); ++i)
        {
            checksum += data[i] * (i + 1);
        }
        return checksum;
    }

    bool OptimizedNetworkProtocol::VerifyChecksum(const NetworkPacket& packet) const
    {
        uint32_t calculatedChecksum = CalculateChecksum(packet.data);
        return calculatedChecksum == packet.header.checksum;
    }

    void OptimizedNetworkProtocol::HandleRetransmission()
    {
        auto now = std::chrono::high_resolution_clock::now();
        
        for (auto it = m_pendingPackets.begin(); it != m_pendingPackets.end();)
        {
            const auto& packet = it->second;
            float age = std::chrono::duration<float>(now - packet.sendTime).count() * 1000.0f; // Convert to ms
            
            if (age > 1000.0f && !packet.isAcknowledged) // 1 second timeout
            {
                // Retransmit packet
                m_sendQueue.push(packet);
                m_stats.packetsRetransmitted++;
                it = m_pendingPackets.erase(it);
            }
            else
            {
                ++it;
            }
        }
    }

    void OptimizedNetworkProtocol::ScheduleRetransmission(const NetworkPacket& packet)
    {
        // Schedule retransmission after timeout
        // This would be handled by a timer system in a real implementation
    }

    void OptimizedNetworkProtocol::CancelRetransmission(uint32_t packetId)
    {
        auto it = m_pendingPackets.find(packetId);
        if (it != m_pendingPackets.end())
        {
            m_pendingPackets.erase(it);
        }
    }

    std::vector<NetworkPacket> OptimizedNetworkProtocol::FragmentPacket(const NetworkPacket& packet)
    {
        std::vector<NetworkPacket> fragments;
        
        if (packet.data.size() <= MAX_PACKET_SIZE)
        {
            fragments.push_back(packet);
            return fragments;
        }

        // Split packet into fragments
        size_t fragmentSize = MAX_PACKET_SIZE - sizeof(PacketHeader);
        size_t numFragments = (packet.data.size() + fragmentSize - 1) / fragmentSize;
        
        for (size_t i = 0; i < numFragments; ++i)
        {
            NetworkPacket fragment = packet;
            fragment.header.flags |= static_cast<uint16_t>(PacketFlags::Fragmented);
            
            if (i == numFragments - 1)
            {
                fragment.header.flags |= static_cast<uint16_t>(PacketFlags::LastFragment);
            }
            
            size_t start = i * fragmentSize;
            size_t end = std::min(start + fragmentSize, packet.data.size());
            
            fragment.data.assign(packet.data.begin() + start, packet.data.begin() + end);
            fragment.header.dataSize = static_cast<uint32_t>(fragment.data.size());
            fragment.header.checksum = CalculateChecksum(fragment.data);
            
            fragments.push_back(fragment);
        }

        return fragments;
    }

    NetworkPacket OptimizedNetworkProtocol::DefragmentPacket(const std::vector<NetworkPacket>& fragments)
    {
        if (fragments.empty())
        {
            return NetworkPacket();
        }

        // Sort fragments by sequence number
        std::vector<NetworkPacket> sortedFragments = fragments;
        std::sort(sortedFragments.begin(), sortedFragments.end(), 
                 [](const NetworkPacket& a, const NetworkPacket& b) {
                     return a.header.sequenceNumber < b.header.sequenceNumber;
                 });

        // Combine fragments
        NetworkPacket combined = sortedFragments[0];
        combined.header.flags &= ~static_cast<uint16_t>(PacketFlags::Fragmented);
        combined.header.flags &= ~static_cast<uint16_t>(PacketFlags::LastFragment);
        
        for (size_t i = 1; i < sortedFragments.size(); ++i)
        {
            combined.data.insert(combined.data.end(), 
                               sortedFragments[i].data.begin(), 
                               sortedFragments[i].data.end());
        }
        
        combined.header.dataSize = static_cast<uint32_t>(combined.data.size());
        combined.header.checksum = CalculateChecksum(combined.data);
        
        return combined;
    }

    // Protocol utilities implementation
    namespace ProtocolUtils
    {
        NetworkPacket CreateDataPacket(const std::vector<uint8_t>& data, uint32_t packetId)
        {
            NetworkPacket packet;
            packet.header.packetId = packetId;
            packet.header.type = PacketType::Data;
            packet.header.dataSize = static_cast<uint32_t>(data.size());
            packet.data = data;
            packet.header.checksum = 0; // Will be calculated later
            return packet;
        }

        NetworkPacket CreateAckPacket(uint32_t ackedPacketId, uint32_t packetId)
        {
            NetworkPacket packet;
            packet.header.packetId = packetId;
            packet.header.type = PacketType::Acknowledgment;
            packet.header.dataSize = 4; // Just the acked packet ID
            packet.data.resize(4);
            std::memcpy(packet.data.data(), &ackedPacketId, 4);
            packet.header.checksum = 0; // Will be calculated later
            return packet;
        }

        NetworkPacket CreateHeartbeatPacket(uint32_t packetId)
        {
            NetworkPacket packet;
            packet.header.packetId = packetId;
            packet.header.type = PacketType::Heartbeat;
            packet.header.dataSize = 0;
            packet.header.checksum = 0; // Will be calculated later
            return packet;
        }

        std::vector<uint8_t> SerializePacket(const NetworkPacket& packet)
        {
            std::vector<uint8_t> data;
            data.reserve(sizeof(PacketHeader) + packet.data.size());
            
            // Serialize header
            data.insert(data.end(), reinterpret_cast<const uint8_t*>(&packet.header), 
                       reinterpret_cast<const uint8_t*>(&packet.header) + sizeof(PacketHeader));
            
            // Serialize data
            data.insert(data.end(), packet.data.begin(), packet.data.end());
            
            return data;
        }

        NetworkPacket DeserializePacket(const std::vector<uint8_t>& data)
        {
            NetworkPacket packet;
            
            if (data.size() < sizeof(PacketHeader))
            {
                return packet; // Invalid packet
            }
            
            // Deserialize header
            std::memcpy(&packet.header, data.data(), sizeof(PacketHeader));
            
            // Deserialize data
            if (data.size() >= sizeof(PacketHeader) + packet.header.dataSize)
            {
                packet.data.assign(data.begin() + sizeof(PacketHeader), 
                                 data.begin() + sizeof(PacketHeader) + packet.header.dataSize);
            }
            
            return packet;
        }

        bool IsValidPacket(const NetworkPacket& packet)
        {
            return packet.header.magic == 0xDEADBEEF && 
                   packet.header.version == PROTOCOL_VERSION &&
                   packet.header.dataSize == packet.data.size();
        }

        bool IsPacketExpired(const NetworkPacket& packet, float maxAge)
        {
            auto now = std::chrono::high_resolution_clock::now();
            float age = std::chrono::duration<float>(now - packet.sendTime).count() * 1000.0f; // Convert to ms
            return age > maxAge;
        }

        void OptimizePacketSize(NetworkPacket& packet)
        {
            // Optimize packet size based on network conditions
            if (packet.data.size() > MAX_PACKET_SIZE)
            {
                // Packet is too large, needs fragmentation
                // This would be handled by the fragmentation system
            }
        }

        void OptimizePacketOrder(std::vector<NetworkPacket>& packets)
        {
            // Sort packets by priority
            std::sort(packets.begin(), packets.end(), [](const NetworkPacket& a, const NetworkPacket& b) {
                return a.header.sequenceNumber < b.header.sequenceNumber;
            });
        }

        void PrintPacketAnalysis(const NetworkPacket& packet)
        {
            LOG_INFO("=== Packet Analysis ===");
            LOG_INFO("Packet ID: " + std::to_string(packet.header.packetId));
            LOG_INFO("Sequence: " + std::to_string(packet.header.sequenceNumber));
            LOG_INFO("Type: " + std::to_string(static_cast<int>(packet.header.type)));
            LOG_INFO("Flags: " + std::to_string(packet.header.flags));
            LOG_INFO("Data size: " + std::to_string(packet.header.dataSize) + " bytes");
            LOG_INFO("Checksum: " + std::to_string(packet.header.checksum));
            LOG_INFO("=====================");
        }

        void PrintProtocolStatistics(const NetworkStats& stats)
        {
            LOG_INFO("=== Protocol Statistics ===");
            LOG_INFO("Packets sent: " + std::to_string(stats.packetsSent));
            LOG_INFO("Packets received: " + std::to_string(stats.packetsReceived));
            LOG_INFO("Packets lost: " + std::to_string(stats.packetsLost));
            LOG_INFO("Bytes sent: " + std::to_string(stats.bytesSent));
            LOG_INFO("Bytes received: " + std::to_string(stats.bytesReceived));
            LOG_INFO("Average latency: " + std::to_string(stats.averageLatency) + "ms");
            LOG_INFO("Packet loss rate: " + std::to_string(stats.packetLossRate * 100.0f) + "%");
            LOG_INFO("=========================");
        }
    }
}
