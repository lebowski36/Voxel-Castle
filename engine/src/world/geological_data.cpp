#include "world/geological_data.h"
#include <cstring>
#include <sstream>
#include <algorithm>

namespace VoxelCastle {
namespace World {

// GeologicalLayer Implementation

GeologicalLayer::GeologicalLayer() {
    SetDefaults();
}

GeologicalLayer::GeologicalLayer(RockType type, float thick, float hard, float porous) 
    : rockType(type), thickness(thick), hardness(hard), porosity(porous) {
    std::memset(mineralContent, 0, sizeof(mineralContent));
}

bool GeologicalLayer::SerializeToBinary(std::vector<uint8_t>& buffer) const {
    size_t startSize = buffer.size();
    size_t totalSize = sizeof(uint8_t) + // rockType
                       sizeof(float) * 3 + // thickness, hardness, porosity
                       sizeof(mineralContent); // mineral content array
    
    buffer.resize(startSize + totalSize);
    uint8_t* ptr = buffer.data() + startSize;
    
    // Write rock type
    uint8_t rockTypeValue = static_cast<uint8_t>(rockType);
    std::memcpy(ptr, &rockTypeValue, sizeof(rockTypeValue)); ptr += sizeof(rockTypeValue);
    
    // Write properties
    std::memcpy(ptr, &thickness, sizeof(thickness)); ptr += sizeof(thickness);
    std::memcpy(ptr, &hardness, sizeof(hardness)); ptr += sizeof(hardness);
    std::memcpy(ptr, &porosity, sizeof(porosity)); ptr += sizeof(porosity);
    
    // Write mineral content
    std::memcpy(ptr, mineralContent, sizeof(mineralContent));
    
    return true;
}

bool GeologicalLayer::DeserializeFromBinary(const std::vector<uint8_t>& buffer, size_t& offset) {
    size_t requiredSize = sizeof(uint8_t) + sizeof(float) * 3 + sizeof(mineralContent);
    if (offset + requiredSize > buffer.size()) {
        return false;
    }
    
    const uint8_t* ptr = buffer.data() + offset;
    
    // Read rock type
    uint8_t rockTypeValue;
    std::memcpy(&rockTypeValue, ptr, sizeof(rockTypeValue)); ptr += sizeof(rockTypeValue);
    rockType = static_cast<RockType>(rockTypeValue);
    
    // Read properties
    std::memcpy(&thickness, ptr, sizeof(thickness)); ptr += sizeof(thickness);
    std::memcpy(&hardness, ptr, sizeof(hardness)); ptr += sizeof(hardness);
    std::memcpy(&porosity, ptr, sizeof(porosity)); ptr += sizeof(porosity);
    
    // Read mineral content
    std::memcpy(mineralContent, ptr, sizeof(mineralContent));
    
    offset += requiredSize;
    return IsValid();
}

bool GeologicalLayer::IsValid() const {
    return static_cast<uint8_t>(rockType) < static_cast<uint8_t>(RockType::COUNT) &&
           thickness >= 0.0f && thickness <= 1000.0f &&
           hardness >= 0.0f && hardness <= 1.0f &&
           porosity >= 0.0f && porosity <= 1.0f;
}

void GeologicalLayer::SetDefaults() {
    rockType = RockType::SEDIMENTARY_LIMESTONE;
    thickness = 10.0f;
    hardness = 0.5f;
    porosity = 0.3f;
    std::memset(mineralContent, 0, sizeof(mineralContent));
}

std::string GeologicalLayer::ToString() const {
    std::ostringstream oss;
    oss << "GeologicalLayer{type=" << RockTypeToString(rockType)
        << ", thickness=" << thickness << "m"
        << ", hardness=" << hardness
        << ", porosity=" << porosity << "}";
    return oss.str();
}

// GeologicalData Implementation

GeologicalData::GeologicalData() {
    SetDefaults();
}

bool GeologicalData::SerializeToBinary(std::vector<uint8_t>& buffer) const {
    size_t startSize = buffer.size();
    
    // Calculate total size
    size_t headerSize = sizeof(float) * 8 + // basic properties
                        sizeof(uint8_t) + // layerCount
                        sizeof(float) * 3; // mineral, oil, groundwater
    
    size_t layerDataSize = 0;
    for (uint8_t i = 0; i < layerCount; ++i) {
        layerDataSize += sizeof(uint8_t) + sizeof(float) * 3 + sizeof(layers[i].mineralContent);
    }
    
    buffer.resize(startSize + headerSize + layerDataSize);
    uint8_t* ptr = buffer.data() + startSize;
    
    // Write basic properties
    std::memcpy(ptr, &bedrockDepth, sizeof(bedrockDepth)); ptr += sizeof(bedrockDepth);
    std::memcpy(ptr, &soilDepth, sizeof(soilDepth)); ptr += sizeof(soilDepth);
    std::memcpy(ptr, &tectonicStability, sizeof(tectonicStability)); ptr += sizeof(tectonicStability);
    std::memcpy(ptr, &erosionResistance, sizeof(erosionResistance)); ptr += sizeof(erosionResistance);
    
    // Write erosion data
    std::memcpy(ptr, &hydraulicErosion, sizeof(hydraulicErosion)); ptr += sizeof(hydraulicErosion);
    std::memcpy(ptr, &thermalErosion, sizeof(thermalErosion)); ptr += sizeof(thermalErosion);
    std::memcpy(ptr, &chemicalWeathering, sizeof(chemicalWeathering)); ptr += sizeof(chemicalWeathering);
    std::memcpy(ptr, &massWasting, sizeof(massWasting)); ptr += sizeof(massWasting);
    
    // Write resource data
    std::memcpy(ptr, &mineralRichness, sizeof(mineralRichness)); ptr += sizeof(mineralRichness);
    std::memcpy(ptr, &oilReserves, sizeof(oilReserves)); ptr += sizeof(oilReserves);
    std::memcpy(ptr, &groundwaterDepth, sizeof(groundwaterDepth)); ptr += sizeof(groundwaterDepth);
    
    // Write layer count
    std::memcpy(ptr, &layerCount, sizeof(layerCount)); ptr += sizeof(layerCount);
    
    // Serialize each layer
    for (uint8_t i = 0; i < layerCount; ++i) {
        std::vector<uint8_t> layerBuffer;
        if (!layers[i].SerializeToBinary(layerBuffer)) {
            return false;
        }
        std::memcpy(ptr, layerBuffer.data(), layerBuffer.size());
        ptr += layerBuffer.size();
    }
    
    return true;
}

bool GeologicalData::DeserializeFromBinary(const std::vector<uint8_t>& buffer, size_t& offset) {
    size_t headerSize = sizeof(float) * 11 + sizeof(uint8_t);
    if (offset + headerSize > buffer.size()) {
        return false;
    }
    
    const uint8_t* ptr = buffer.data() + offset;
    
    // Read basic properties
    std::memcpy(&bedrockDepth, ptr, sizeof(bedrockDepth)); ptr += sizeof(bedrockDepth);
    std::memcpy(&soilDepth, ptr, sizeof(soilDepth)); ptr += sizeof(soilDepth);
    std::memcpy(&tectonicStability, ptr, sizeof(tectonicStability)); ptr += sizeof(tectonicStability);
    std::memcpy(&erosionResistance, ptr, sizeof(erosionResistance)); ptr += sizeof(erosionResistance);
    
    // Read erosion data
    std::memcpy(&hydraulicErosion, ptr, sizeof(hydraulicErosion)); ptr += sizeof(hydraulicErosion);
    std::memcpy(&thermalErosion, ptr, sizeof(thermalErosion)); ptr += sizeof(thermalErosion);
    std::memcpy(&chemicalWeathering, ptr, sizeof(chemicalWeathering)); ptr += sizeof(chemicalWeathering);
    std::memcpy(&massWasting, ptr, sizeof(massWasting)); ptr += sizeof(massWasting);
    
    // Read resource data
    std::memcpy(&mineralRichness, ptr, sizeof(mineralRichness)); ptr += sizeof(mineralRichness);
    std::memcpy(&oilReserves, ptr, sizeof(oilReserves)); ptr += sizeof(oilReserves);
    std::memcpy(&groundwaterDepth, ptr, sizeof(groundwaterDepth)); ptr += sizeof(groundwaterDepth);
    
    // Read layer count
    std::memcpy(&layerCount, ptr, sizeof(layerCount)); ptr += sizeof(layerCount);
    
    offset += headerSize;
    
    // Validate layer count
    if (layerCount > MAX_LAYERS) {
        return false;
    }
    
    // Deserialize each layer
    for (uint8_t i = 0; i < layerCount; ++i) {
        if (!layers[i].DeserializeFromBinary(buffer, offset)) {
            return false;
        }
    }
    
    return IsValid();
}

bool GeologicalData::IsValid() const {
    if (bedrockDepth < 0.0f || bedrockDepth > 10000.0f ||
        soilDepth < 0.0f || soilDepth > 100.0f ||
        tectonicStability < 0.0f || tectonicStability > 1.0f ||
        erosionResistance < 0.0f || erosionResistance > 1.0f ||
        layerCount > MAX_LAYERS) {
        return false;
    }
    
    // Validate all active layers
    for (uint8_t i = 0; i < layerCount; ++i) {
        if (!layers[i].IsValid()) {
            return false;
        }
    }
    
    return true;
}

void GeologicalData::SetDefaults() {
    bedrockDepth = 50.0f;
    soilDepth = 2.0f;
    tectonicStability = 0.7f;
    erosionResistance = 0.5f;
    
    hydraulicErosion = 0.1f;
    thermalErosion = 0.05f;
    chemicalWeathering = 0.02f;
    massWasting = 0.01f;
    
    mineralRichness = 0.3f;
    oilReserves = 0.0f;
    groundwaterDepth = 10.0f;
    
    layerCount = 3;
    
    // Set up default layers: soil, sedimentary rock, bedrock
    layers[0] = GeologicalLayer(RockType::SOIL_LOAM, 2.0f, 0.1f, 0.8f);
    layers[1] = GeologicalLayer(RockType::SEDIMENTARY_LIMESTONE, 20.0f, 0.4f, 0.3f);
    layers[2] = GeologicalLayer(RockType::IGNEOUS_GRANITE, 50.0f, 0.9f, 0.1f);
}

bool GeologicalData::AddLayer(const GeologicalLayer& layer) {
    if (layerCount >= MAX_LAYERS || !layer.IsValid()) {
        return false;
    }
    
    layers[layerCount] = layer;
    layerCount++;
    return true;
}

bool GeologicalData::RemoveLayer(size_t index) {
    if (index >= layerCount) {
        return false;
    }
    
    // Shift layers down
    for (size_t i = index; i < layerCount - 1; ++i) {
        layers[i] = layers[i + 1];
    }
    layerCount--;
    return true;
}

const GeologicalLayer* GeologicalData::GetLayer(size_t index) const {
    if (index >= layerCount) {
        return nullptr;
    }
    return &layers[index];
}

float GeologicalData::GetMineralDensity(MineralType mineral) const {
    float totalDensity = 0.0f;
    float totalThickness = 0.0f;
    
    for (uint8_t i = 0; i < layerCount; ++i) {
        float layerDensity = layers[i].mineralContent[static_cast<size_t>(mineral)] / 100.0f;
        totalDensity += layerDensity * layers[i].thickness;
        totalThickness += layers[i].thickness;
    }
    
    return totalThickness > 0.0f ? totalDensity / totalThickness : 0.0f;
}

RockType GeologicalData::GetSurfaceRockType() const {
    return layerCount > 0 ? layers[0].rockType : RockType::SOIL_LOAM;
}

float GeologicalData::GetHardnessAtDepth(float depth) const {
    float currentDepth = 0.0f;
    
    for (uint8_t i = 0; i < layerCount; ++i) {
        if (currentDepth + layers[i].thickness > depth) {
            return layers[i].hardness;
        }
        currentDepth += layers[i].thickness;
    }
    
    // Beyond all layers, assume maximum hardness (bedrock)
    return 1.0f;
}

std::string GeologicalData::ToString() const {
    std::ostringstream oss;
    oss << "GeologicalData{layers=" << static_cast<int>(layerCount)
        << ", bedrockDepth=" << bedrockDepth << "m"
        << ", soilDepth=" << soilDepth << "m"
        << ", stability=" << tectonicStability
        << ", erosionResistance=" << erosionResistance << "}";
    return oss.str();
}

size_t GeologicalData::GetSerializedSize() const {
    size_t size = sizeof(float) * 11 + sizeof(uint8_t); // Header
    for (uint8_t i = 0; i < layerCount; ++i) {
        size += sizeof(uint8_t) + sizeof(float) * 3 + sizeof(layers[i].mineralContent);
    }
    return size;
}

// Utility Functions

const char* RockTypeToString(RockType type) {
    switch (type) {
        case RockType::SEDIMENTARY_LIMESTONE: return "Limestone";
        case RockType::SEDIMENTARY_SANDSTONE: return "Sandstone";
        case RockType::SEDIMENTARY_SHALE: return "Shale";
        case RockType::IGNEOUS_GRANITE: return "Granite";
        case RockType::IGNEOUS_BASALT: return "Basalt";
        case RockType::IGNEOUS_OBSIDIAN: return "Obsidian";
        case RockType::METAMORPHIC_MARBLE: return "Marble";
        case RockType::METAMORPHIC_SLATE: return "Slate";
        case RockType::METAMORPHIC_QUARTZITE: return "Quartzite";
        case RockType::SOIL_CLAY: return "Clay";
        case RockType::SOIL_SAND: return "Sand";
        case RockType::SOIL_LOAM: return "Loam";
        default: return "Unknown";
    }
}

const char* MineralTypeToString(MineralType type) {
    switch (type) {
        case MineralType::IRON: return "Iron";
        case MineralType::COPPER: return "Copper";
        case MineralType::GOLD: return "Gold";
        case MineralType::SILVER: return "Silver";
        case MineralType::COAL: return "Coal";
        case MineralType::DIAMOND: return "Diamond";
        case MineralType::EMERALD: return "Emerald";
        case MineralType::QUARTZ: return "Quartz";
        case MineralType::SALT: return "Salt";
        case MineralType::OIL: return "Oil";
        default: return "Unknown";
    }
}

RockType StringToRockType(const std::string& str) {
    if (str == "Limestone") return RockType::SEDIMENTARY_LIMESTONE;
    if (str == "Sandstone") return RockType::SEDIMENTARY_SANDSTONE;
    if (str == "Shale") return RockType::SEDIMENTARY_SHALE;
    if (str == "Granite") return RockType::IGNEOUS_GRANITE;
    if (str == "Basalt") return RockType::IGNEOUS_BASALT;
    if (str == "Obsidian") return RockType::IGNEOUS_OBSIDIAN;
    if (str == "Marble") return RockType::METAMORPHIC_MARBLE;
    if (str == "Slate") return RockType::METAMORPHIC_SLATE;
    if (str == "Quartzite") return RockType::METAMORPHIC_QUARTZITE;
    if (str == "Clay") return RockType::SOIL_CLAY;
    if (str == "Sand") return RockType::SOIL_SAND;
    if (str == "Loam") return RockType::SOIL_LOAM;
    return RockType::SOIL_LOAM; // Default
}

MineralType StringToMineralType(const std::string& str) {
    if (str == "Iron") return MineralType::IRON;
    if (str == "Copper") return MineralType::COPPER;
    if (str == "Gold") return MineralType::GOLD;
    if (str == "Silver") return MineralType::SILVER;
    if (str == "Coal") return MineralType::COAL;
    if (str == "Diamond") return MineralType::DIAMOND;
    if (str == "Emerald") return MineralType::EMERALD;
    if (str == "Quartz") return MineralType::QUARTZ;
    if (str == "Salt") return MineralType::SALT;
    if (str == "Oil") return MineralType::OIL;
    return MineralType::IRON; // Default
}

} // namespace World
} // namespace VoxelCastle
