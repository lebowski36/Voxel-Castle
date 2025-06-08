#include "world/hydrological_data.h"
#include <cstring>
#include <sstream>
#include <cmath>
#include <algorithm>

namespace VoxelCastle {
namespace World {

// RiverSegment Implementation

RiverSegment::RiverSegment() {
    SetDefaults();
}

RiverSegment::RiverSegment(float sx, float sz, float ex, float ez, float w, float d, float f) 
    : startX(sx), startZ(sz), endX(ex), endZ(ez), width(w), depth(d), flow(f) {
    size = RiverSize::STREAM;
    elevation = 0.0f;
    std::fill(connectsTo, connectsTo + 4, 255);
}

bool RiverSegment::SerializeToBinary(std::vector<uint8_t>& buffer) const {
    size_t startSize = buffer.size();
    size_t totalSize = sizeof(float) * 7 + // coordinates, width, depth, flow, elevation
                       sizeof(uint8_t) + // size
                       sizeof(connectsTo); // connections
    
    buffer.resize(startSize + totalSize);
    uint8_t* ptr = buffer.data() + startSize;
    
    // Write coordinates and properties
    std::memcpy(ptr, &startX, sizeof(startX)); ptr += sizeof(startX);
    std::memcpy(ptr, &startZ, sizeof(startZ)); ptr += sizeof(startZ);
    std::memcpy(ptr, &endX, sizeof(endX)); ptr += sizeof(endX);
    std::memcpy(ptr, &endZ, sizeof(endZ)); ptr += sizeof(endZ);
    std::memcpy(ptr, &width, sizeof(width)); ptr += sizeof(width);
    std::memcpy(ptr, &depth, sizeof(depth)); ptr += sizeof(depth);
    std::memcpy(ptr, &flow, sizeof(flow)); ptr += sizeof(flow);
    std::memcpy(ptr, &elevation, sizeof(elevation)); ptr += sizeof(elevation);
    
    // Write size
    uint8_t sizeValue = static_cast<uint8_t>(size);
    std::memcpy(ptr, &sizeValue, sizeof(sizeValue)); ptr += sizeof(sizeValue);
    
    // Write connections
    std::memcpy(ptr, connectsTo, sizeof(connectsTo));
    
    return true;
}

bool RiverSegment::DeserializeFromBinary(const std::vector<uint8_t>& buffer, size_t& offset) {
    size_t requiredSize = sizeof(float) * 8 + sizeof(uint8_t) + sizeof(connectsTo);
    if (offset + requiredSize > buffer.size()) {
        return false;
    }
    
    const uint8_t* ptr = buffer.data() + offset;
    
    // Read coordinates and properties
    std::memcpy(&startX, ptr, sizeof(startX)); ptr += sizeof(startX);
    std::memcpy(&startZ, ptr, sizeof(startZ)); ptr += sizeof(startZ);
    std::memcpy(&endX, ptr, sizeof(endX)); ptr += sizeof(endX);
    std::memcpy(&endZ, ptr, sizeof(endZ)); ptr += sizeof(endZ);
    std::memcpy(&width, ptr, sizeof(width)); ptr += sizeof(width);
    std::memcpy(&depth, ptr, sizeof(depth)); ptr += sizeof(depth);
    std::memcpy(&flow, ptr, sizeof(flow)); ptr += sizeof(flow);
    std::memcpy(&elevation, ptr, sizeof(elevation)); ptr += sizeof(elevation);
    
    // Read size
    uint8_t sizeValue;
    std::memcpy(&sizeValue, ptr, sizeof(sizeValue)); ptr += sizeof(sizeValue);
    size = static_cast<RiverSize>(sizeValue);
    
    // Read connections
    std::memcpy(connectsTo, ptr, sizeof(connectsTo));
    
    offset += requiredSize;
    return IsValid();
}

bool RiverSegment::IsValid() const {
    return width >= 0.0f && width <= 1000.0f &&
           depth >= 0.0f && depth <= 100.0f &&
           flow >= 0.0f &&
           static_cast<uint8_t>(size) <= 3;
}

void RiverSegment::SetDefaults() {
    startX = startZ = endX = endZ = 0.0f;
    width = 2.0f;
    depth = 0.5f;
    flow = 1.0f;
    elevation = 0.0f;
    size = RiverSize::STREAM;
    std::fill(connectsTo, connectsTo + 4, 255);
}

float RiverSegment::GetLength() const {
    float dx = endX - startX;
    float dz = endZ - startZ;
    return std::sqrt(dx * dx + dz * dz);
}

std::string RiverSegment::ToString() const {
    std::ostringstream oss;
    oss << "RiverSegment{start=(" << startX << "," << startZ << ")"
        << ", end=(" << endX << "," << endZ << ")"
        << ", width=" << width << "m"
        << ", depth=" << depth << "m"
        << ", flow=" << flow << "m³/s}";
    return oss.str();
}

// WaterBody Implementation

WaterBody::WaterBody() {
    SetDefaults();
}

WaterBody::WaterBody(WaterBodyType t, float x, float z, float a, float v, float d) 
    : type(t), centerX(x), centerZ(z), area(a), volume(v), depth(d) {
    elevation = 0.0f;
    temperature = 15.0f;
    std::fill(connectedRivers, connectedRivers + 8, 255);
}

bool WaterBody::SerializeToBinary(std::vector<uint8_t>& buffer) const {
    size_t startSize = buffer.size();
    size_t totalSize = sizeof(uint8_t) + // type
                       sizeof(float) * 6 + // coordinates, area, volume, depth, elevation, temperature
                       sizeof(connectedRivers); // connections
    
    buffer.resize(startSize + totalSize);
    uint8_t* ptr = buffer.data() + startSize;
    
    // Write type
    uint8_t typeValue = static_cast<uint8_t>(type);
    std::memcpy(ptr, &typeValue, sizeof(typeValue)); ptr += sizeof(typeValue);
    
    // Write properties
    std::memcpy(ptr, &centerX, sizeof(centerX)); ptr += sizeof(centerX);
    std::memcpy(ptr, &centerZ, sizeof(centerZ)); ptr += sizeof(centerZ);
    std::memcpy(ptr, &area, sizeof(area)); ptr += sizeof(area);
    std::memcpy(ptr, &volume, sizeof(volume)); ptr += sizeof(volume);
    std::memcpy(ptr, &depth, sizeof(depth)); ptr += sizeof(depth);
    std::memcpy(ptr, &elevation, sizeof(elevation)); ptr += sizeof(elevation);
    std::memcpy(ptr, &temperature, sizeof(temperature)); ptr += sizeof(temperature);
    
    // Write connections
    std::memcpy(ptr, connectedRivers, sizeof(connectedRivers));
    
    return true;
}

bool WaterBody::DeserializeFromBinary(const std::vector<uint8_t>& buffer, size_t& offset) {
    size_t requiredSize = sizeof(uint8_t) + sizeof(float) * 6 + sizeof(connectedRivers);
    if (offset + requiredSize > buffer.size()) {
        return false;
    }
    
    const uint8_t* ptr = buffer.data() + offset;
    
    // Read type
    uint8_t typeValue;
    std::memcpy(&typeValue, ptr, sizeof(typeValue)); ptr += sizeof(typeValue);
    type = static_cast<WaterBodyType>(typeValue);
    
    // Read properties
    std::memcpy(&centerX, ptr, sizeof(centerX)); ptr += sizeof(centerX);
    std::memcpy(&centerZ, ptr, sizeof(centerZ)); ptr += sizeof(centerZ);
    std::memcpy(&area, ptr, sizeof(area)); ptr += sizeof(area);
    std::memcpy(&volume, ptr, sizeof(volume)); ptr += sizeof(volume);
    std::memcpy(&depth, ptr, sizeof(depth)); ptr += sizeof(depth);
    std::memcpy(&elevation, ptr, sizeof(elevation)); ptr += sizeof(elevation);
    std::memcpy(&temperature, ptr, sizeof(temperature)); ptr += sizeof(temperature);
    
    // Read connections
    std::memcpy(connectedRivers, ptr, sizeof(connectedRivers));
    
    offset += requiredSize;
    return IsValid();
}

bool WaterBody::IsValid() const {
    return static_cast<uint8_t>(type) <= 7 &&
           area >= 0.0f &&
           volume >= 0.0f &&
           depth >= 0.0f && depth <= 1000.0f &&
           temperature >= -50.0f && temperature <= 100.0f;
}

void WaterBody::SetDefaults() {
    type = WaterBodyType::POND;
    centerX = centerZ = 0.0f;
    area = 100.0f;
    volume = 50.0f;
    depth = 2.0f;
    elevation = 0.0f;
    temperature = 15.0f;
    std::fill(connectedRivers, connectedRivers + 8, 255);
}

std::string WaterBody::ToString() const {
    std::ostringstream oss;
    oss << "WaterBody{type=" << WaterBodyTypeToString(type)
        << ", center=(" << centerX << "," << centerZ << ")"
        << ", area=" << area << "m²"
        << ", depth=" << depth << "m}";
    return oss.str();
}

// HydrologicalData Implementation

HydrologicalData::HydrologicalData() {
    SetDefaults();
}

bool HydrologicalData::SerializeToBinary(std::vector<uint8_t>& buffer) const {
    size_t startSize = buffer.size();
    
    // Calculate header size
    size_t headerSize = sizeof(float) * 14 + // drainage, precip, runoff, etc.
                        sizeof(uint8_t) * 3 + // counts and watershed ID
                        sizeof(bool) * 4; // special features
    
    // Calculate data size
    size_t riverDataSize = 0;
    for (uint8_t i = 0; i < riverSegmentCount; ++i) {
        riverDataSize += sizeof(float) * 8 + sizeof(uint8_t) + sizeof(riverSegments[i].connectsTo);
    }
    
    size_t waterBodyDataSize = 0;
    for (uint8_t i = 0; i < waterBodyCount; ++i) {
        waterBodyDataSize += sizeof(uint8_t) + sizeof(float) * 6 + sizeof(waterBodies[i].connectedRivers);
    }
    
    buffer.resize(startSize + headerSize + riverDataSize + waterBodyDataSize);
    uint8_t* ptr = buffer.data() + startSize;
    
    // Write watershed data
    std::memcpy(ptr, &drainageArea, sizeof(drainageArea)); ptr += sizeof(drainageArea);
    std::memcpy(ptr, &precipitationCapture, sizeof(precipitationCapture)); ptr += sizeof(precipitationCapture);
    std::memcpy(ptr, &runoffCoefficient, sizeof(runoffCoefficient)); ptr += sizeof(runoffCoefficient);
    std::memcpy(ptr, &infiltrationRate, sizeof(infiltrationRate)); ptr += sizeof(infiltrationRate);
    
    // Write groundwater data
    std::memcpy(ptr, &groundwaterLevel, sizeof(groundwaterLevel)); ptr += sizeof(groundwaterLevel);
    std::memcpy(ptr, &aquiferThickness, sizeof(aquiferThickness)); ptr += sizeof(aquiferThickness);
    std::memcpy(ptr, &porosity, sizeof(porosity)); ptr += sizeof(porosity);
    std::memcpy(ptr, &permeability, sizeof(permeability)); ptr += sizeof(permeability);
    std::memcpy(ptr, &rechargeRate, sizeof(rechargeRate)); ptr += sizeof(rechargeRate);
    
    // Write flow data
    std::memcpy(ptr, &totalFlow, sizeof(totalFlow)); ptr += sizeof(totalFlow);
    std::memcpy(ptr, &flowDirection, sizeof(flowDirection)); ptr += sizeof(flowDirection);
    
    // Write seasonal data
    std::memcpy(ptr, &springFlow, sizeof(springFlow)); ptr += sizeof(springFlow);
    std::memcpy(ptr, &summerFlow, sizeof(summerFlow)); ptr += sizeof(summerFlow);
    std::memcpy(ptr, &autumnFlow, sizeof(autumnFlow)); ptr += sizeof(autumnFlow);
    std::memcpy(ptr, &winterFlow, sizeof(winterFlow)); ptr += sizeof(winterFlow);
    
    // Write counts and ID
    std::memcpy(ptr, &riverSegmentCount, sizeof(riverSegmentCount)); ptr += sizeof(riverSegmentCount);
    std::memcpy(ptr, &waterBodyCount, sizeof(waterBodyCount)); ptr += sizeof(waterBodyCount);
    std::memcpy(ptr, &watershedID, sizeof(watershedID)); ptr += sizeof(watershedID);
    
    // Write special features
    std::memcpy(ptr, &hasWaterfall, sizeof(hasWaterfall)); ptr += sizeof(hasWaterfall);
    std::memcpy(ptr, &hasSpring, sizeof(hasSpring)); ptr += sizeof(hasSpring);
    std::memcpy(ptr, &hasFloodPlain, sizeof(hasFloodPlain)); ptr += sizeof(hasFloodPlain);
    std::memcpy(ptr, &hasWetlands, sizeof(hasWetlands)); ptr += sizeof(hasWetlands);
    
    // Serialize river segments
    for (uint8_t i = 0; i < riverSegmentCount; ++i) {
        std::vector<uint8_t> segmentBuffer;
        if (!riverSegments[i].SerializeToBinary(segmentBuffer)) {
            return false;
        }
        std::memcpy(ptr, segmentBuffer.data(), segmentBuffer.size());
        ptr += segmentBuffer.size();
    }
    
    // Serialize water bodies
    for (uint8_t i = 0; i < waterBodyCount; ++i) {
        std::vector<uint8_t> bodyBuffer;
        if (!waterBodies[i].SerializeToBinary(bodyBuffer)) {
            return false;
        }
        std::memcpy(ptr, bodyBuffer.data(), bodyBuffer.size());
        ptr += bodyBuffer.size();
    }
    
    return true;
}

bool HydrologicalData::DeserializeFromBinary(const std::vector<uint8_t>& buffer, size_t& offset) {
    size_t headerSize = sizeof(float) * 14 + sizeof(uint8_t) * 3 + sizeof(bool) * 4;
    if (offset + headerSize > buffer.size()) {
        return false;
    }
    
    const uint8_t* ptr = buffer.data() + offset;
    
    // Read watershed data
    std::memcpy(&drainageArea, ptr, sizeof(drainageArea)); ptr += sizeof(drainageArea);
    std::memcpy(&precipitationCapture, ptr, sizeof(precipitationCapture)); ptr += sizeof(precipitationCapture);
    std::memcpy(&runoffCoefficient, ptr, sizeof(runoffCoefficient)); ptr += sizeof(runoffCoefficient);
    std::memcpy(&infiltrationRate, ptr, sizeof(infiltrationRate)); ptr += sizeof(infiltrationRate);
    
    // Read groundwater data
    std::memcpy(&groundwaterLevel, ptr, sizeof(groundwaterLevel)); ptr += sizeof(groundwaterLevel);
    std::memcpy(&aquiferThickness, ptr, sizeof(aquiferThickness)); ptr += sizeof(aquiferThickness);
    std::memcpy(&porosity, ptr, sizeof(porosity)); ptr += sizeof(porosity);
    std::memcpy(&permeability, ptr, sizeof(permeability)); ptr += sizeof(permeability);
    std::memcpy(&rechargeRate, ptr, sizeof(rechargeRate)); ptr += sizeof(rechargeRate);
    
    // Read flow data
    std::memcpy(&totalFlow, ptr, sizeof(totalFlow)); ptr += sizeof(totalFlow);
    std::memcpy(&flowDirection, ptr, sizeof(flowDirection)); ptr += sizeof(flowDirection);
    
    // Read seasonal data
    std::memcpy(&springFlow, ptr, sizeof(springFlow)); ptr += sizeof(springFlow);
    std::memcpy(&summerFlow, ptr, sizeof(summerFlow)); ptr += sizeof(summerFlow);
    std::memcpy(&autumnFlow, ptr, sizeof(autumnFlow)); ptr += sizeof(autumnFlow);
    std::memcpy(&winterFlow, ptr, sizeof(winterFlow)); ptr += sizeof(winterFlow);
    
    // Read counts and ID
    std::memcpy(&riverSegmentCount, ptr, sizeof(riverSegmentCount)); ptr += sizeof(riverSegmentCount);
    std::memcpy(&waterBodyCount, ptr, sizeof(waterBodyCount)); ptr += sizeof(waterBodyCount);
    std::memcpy(&watershedID, ptr, sizeof(watershedID)); ptr += sizeof(watershedID);
    
    // Read special features
    std::memcpy(&hasWaterfall, ptr, sizeof(hasWaterfall)); ptr += sizeof(hasWaterfall);
    std::memcpy(&hasSpring, ptr, sizeof(hasSpring)); ptr += sizeof(hasSpring);
    std::memcpy(&hasFloodPlain, ptr, sizeof(hasFloodPlain)); ptr += sizeof(hasFloodPlain);
    std::memcpy(&hasWetlands, ptr, sizeof(hasWetlands)); ptr += sizeof(hasWetlands);
    
    offset += headerSize;
    
    // Validate counts
    if (riverSegmentCount > MAX_RIVER_SEGMENTS || waterBodyCount > MAX_WATER_BODIES) {
        return false;
    }
    
    // Deserialize river segments
    for (uint8_t i = 0; i < riverSegmentCount; ++i) {
        if (!riverSegments[i].DeserializeFromBinary(buffer, offset)) {
            return false;
        }
    }
    
    // Deserialize water bodies
    for (uint8_t i = 0; i < waterBodyCount; ++i) {
        if (!waterBodies[i].DeserializeFromBinary(buffer, offset)) {
            return false;
        }
    }
    
    return IsValid();
}

bool HydrologicalData::IsValid() const {
    if (drainageArea < 0.0f || 
        precipitationCapture < 0.0f || precipitationCapture > 1.0f ||
        runoffCoefficient < 0.0f || runoffCoefficient > 1.0f ||
        infiltrationRate < 0.0f ||
        riverSegmentCount > MAX_RIVER_SEGMENTS ||
        waterBodyCount > MAX_WATER_BODIES) {
        return false;
    }
    
    // Validate all active river segments
    for (uint8_t i = 0; i < riverSegmentCount; ++i) {
        if (!riverSegments[i].IsValid()) {
            return false;
        }
    }
    
    // Validate all active water bodies
    for (uint8_t i = 0; i < waterBodyCount; ++i) {
        if (!waterBodies[i].IsValid()) {
            return false;
        }
    }
    
    return true;
}

void HydrologicalData::SetDefaults() {
    drainageArea = 10.0f;
    precipitationCapture = 0.7f;
    runoffCoefficient = 0.3f;
    infiltrationRate = 10.0f;
    
    riverSegmentCount = 0;
    waterBodyCount = 0;
    
    groundwaterLevel = 5.0f;
    aquiferThickness = 20.0f;
    porosity = 0.25f;
    permeability = 1.0f;
    rechargeRate = 100.0f;
    
    totalFlow = 1.0f;
    flowDirection = 0.0f;
    watershedID = 0;
    
    springFlow = 1.2f;
    summerFlow = 0.8f;
    autumnFlow = 1.0f;
    winterFlow = 1.1f;
    
    hasWaterfall = false;
    hasSpring = false;
    hasFloodPlain = false;
    hasWetlands = false;
}

bool HydrologicalData::AddRiverSegment(const RiverSegment& segment) {
    if (riverSegmentCount >= MAX_RIVER_SEGMENTS || !segment.IsValid()) {
        return false;
    }
    
    riverSegments[riverSegmentCount] = segment;
    riverSegmentCount++;
    return true;
}

bool HydrologicalData::RemoveRiverSegment(size_t index) {
    if (index >= riverSegmentCount) {
        return false;
    }
    
    // Shift segments down
    for (size_t i = index; i < riverSegmentCount - 1; ++i) {
        riverSegments[i] = riverSegments[i + 1];
    }
    riverSegmentCount--;
    return true;
}

const RiverSegment* HydrologicalData::GetRiverSegment(size_t index) const {
    if (index >= riverSegmentCount) {
        return nullptr;
    }
    return &riverSegments[index];
}

bool HydrologicalData::AddWaterBody(const WaterBody& body) {
    if (waterBodyCount >= MAX_WATER_BODIES || !body.IsValid()) {
        return false;
    }
    
    waterBodies[waterBodyCount] = body;
    waterBodyCount++;
    return true;
}

bool HydrologicalData::RemoveWaterBody(size_t index) {
    if (index >= waterBodyCount) {
        return false;
    }
    
    // Shift bodies down
    for (size_t i = index; i < waterBodyCount - 1; ++i) {
        waterBodies[i] = waterBodies[i + 1];
    }
    waterBodyCount--;
    return true;
}

const WaterBody* HydrologicalData::GetWaterBody(size_t index) const {
    if (index >= waterBodyCount) {
        return nullptr;
    }
    return &waterBodies[index];
}

float HydrologicalData::GetTotalRiverLength() const {
    float totalLength = 0.0f;
    for (uint8_t i = 0; i < riverSegmentCount; ++i) {
        totalLength += riverSegments[i].GetLength();
    }
    return totalLength;
}

float HydrologicalData::GetTotalWaterSurface() const {
    float totalArea = 0.0f;
    for (uint8_t i = 0; i < waterBodyCount; ++i) {
        totalArea += waterBodies[i].area;
    }
    return totalArea;
}

float HydrologicalData::GetSeasonalFlowMultiplier(int month) const {
    switch (month / 3) {
        case 0: return springFlow;   // Mar-May
        case 1: return summerFlow;   // Jun-Aug
        case 2: return autumnFlow;   // Sep-Nov
        case 3: return winterFlow;   // Dec-Feb
        default: return 1.0f;
    }
}

std::string HydrologicalData::ToString() const {
    std::ostringstream oss;
    oss << "HydrologicalData{rivers=" << static_cast<int>(riverSegmentCount)
        << ", waterBodies=" << static_cast<int>(waterBodyCount)
        << ", drainageArea=" << drainageArea << "km²"
        << ", totalFlow=" << totalFlow << "m³/s}";
    return oss.str();
}

size_t HydrologicalData::GetSerializedSize() const {
    size_t size = sizeof(float) * 14 + sizeof(uint8_t) * 3 + sizeof(bool) * 4;
    
    // Add river segment sizes
    for (uint8_t i = 0; i < riverSegmentCount; ++i) {
        size += sizeof(float) * 8 + sizeof(uint8_t) + sizeof(riverSegments[i].connectsTo);
    }
    
    // Add water body sizes
    for (uint8_t i = 0; i < waterBodyCount; ++i) {
        size += sizeof(uint8_t) + sizeof(float) * 6 + sizeof(waterBodies[i].connectedRivers);
    }
    
    return size;
}

// Utility Functions

const char* RiverSizeToString(RiverSize size) {
    switch (size) {
        case RiverSize::STREAM: return "Stream";
        case RiverSize::CREEK: return "Creek";
        case RiverSize::RIVER: return "River";
        case RiverSize::MAJOR_RIVER: return "Major River";
        default: return "Unknown";
    }
}

const char* WaterBodyTypeToString(WaterBodyType type) {
    switch (type) {
        case WaterBodyType::NONE: return "None";
        case WaterBodyType::RIVER: return "River";
        case WaterBodyType::LAKE: return "Lake";
        case WaterBodyType::POND: return "Pond";
        case WaterBodyType::MARSH: return "Marsh";
        case WaterBodyType::WETLAND: return "Wetland";
        case WaterBodyType::SPRING: return "Spring";
        case WaterBodyType::WATERFALL: return "Waterfall";
        default: return "Unknown";
    }
}

RiverSize StringToRiverSize(const std::string& str) {
    if (str == "Stream") return RiverSize::STREAM;
    if (str == "Creek") return RiverSize::CREEK;
    if (str == "River") return RiverSize::RIVER;
    if (str == "Major River") return RiverSize::MAJOR_RIVER;
    return RiverSize::STREAM; // Default
}

WaterBodyType StringToWaterBodyType(const std::string& str) {
    if (str == "None") return WaterBodyType::NONE;
    if (str == "River") return WaterBodyType::RIVER;
    if (str == "Lake") return WaterBodyType::LAKE;
    if (str == "Pond") return WaterBodyType::POND;
    if (str == "Marsh") return WaterBodyType::MARSH;
    if (str == "Wetland") return WaterBodyType::WETLAND;
    if (str == "Spring") return WaterBodyType::SPRING;
    if (str == "Waterfall") return WaterBodyType::WATERFALL;
    return WaterBodyType::POND; // Default
}

} // namespace World
} // namespace VoxelCastle
