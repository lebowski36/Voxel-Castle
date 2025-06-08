#include "world/regional_data.h"
#include <fstream>
#include <cstring>
#include <chrono>
#include <sstream>
#include <iostream>
#include <filesystem>

namespace VoxelCastle {
namespace World {

// RegionalData Implementation

RegionalData::RegionalData() 
    : magicNumber(MAGIC_NUMBER)
    , version(CURRENT_VERSION)
    , flags(0)
    , dataSize(0)
    , regionX(0)
    , regionZ(0)
    , primaryBiome(BiomeType::PLAINS)
    , temperature(15.0f)
    , humidity(60.0f)
    , elevation(100.0f)
    , precipitation(800.0f)
    , generationSeed(0)
    , generationTime(0)
    , simulationLevel(50)
{
    std::memset(reserved, 0, sizeof(reserved));
    SetDefaults();
}

RegionalData::RegionalData(int32_t x, int32_t z, BiomeType biome, 
                          float temp, float humid, float elev)
    : magicNumber(MAGIC_NUMBER)
    , version(CURRENT_VERSION)
    , flags(0)
    , dataSize(0)
    , regionX(x)
    , regionZ(z)
    , primaryBiome(biome)
    , temperature(temp)
    , humidity(humid)
    , elevation(elev)
    , precipitation(800.0f)
    , generationSeed(0)
    , generationTime(0)
    , simulationLevel(50)
{
    std::memset(reserved, 0, sizeof(reserved));
    SetDefaults();
}

bool RegionalData::SerializeToBinary(std::vector<uint8_t>& buffer) const {
    // Calculate header size
    size_t headerSize = sizeof(uint32_t) * 4 +  // magicNumber, version, flags, dataSize
                       sizeof(int32_t) * 2 +   // regionX, regionZ
                       sizeof(uint32_t) +      // primaryBiome (as uint32_t)
                       sizeof(float) * 4 +     // temperature, humidity, elevation, precipitation
                       sizeof(uint64_t) +      // generationSeed
                       sizeof(uint32_t) +      // generationTime
                       sizeof(uint8_t) +       // simulationLevel
                       sizeof(reserved);       // reserved space
    
    // Calculate data sizes for advanced structures
    size_t geologicalSize = geological.GetSerializedSize();
    size_t hydrologicalSize = hydrological.GetSerializedSize();
    size_t climateSize = climate.GetSerializedSize();
    
    size_t totalSize = headerSize + geologicalSize + hydrologicalSize + climateSize;
    
    buffer.resize(totalSize);
    uint8_t* ptr = buffer.data();
    
    // Update dataSize before serializing
    const_cast<RegionalData*>(this)->dataSize = static_cast<uint32_t>(totalSize - sizeof(uint32_t) * 4);
    
    // Write header
    std::memcpy(ptr, &magicNumber, sizeof(magicNumber)); ptr += sizeof(magicNumber);
    std::memcpy(ptr, &version, sizeof(version)); ptr += sizeof(version);
    std::memcpy(ptr, &flags, sizeof(flags)); ptr += sizeof(flags);
    std::memcpy(ptr, &dataSize, sizeof(dataSize)); ptr += sizeof(dataSize);
    
    // Write coordinates
    std::memcpy(ptr, &regionX, sizeof(regionX)); ptr += sizeof(regionX);
    std::memcpy(ptr, &regionZ, sizeof(regionZ)); ptr += sizeof(regionZ);
    
    // Write basic environmental data (legacy compatibility)
    uint32_t biomeValue = static_cast<uint32_t>(primaryBiome);
    std::memcpy(ptr, &biomeValue, sizeof(biomeValue)); ptr += sizeof(biomeValue);
    std::memcpy(ptr, &temperature, sizeof(temperature)); ptr += sizeof(temperature);
    std::memcpy(ptr, &humidity, sizeof(humidity)); ptr += sizeof(humidity);
    std::memcpy(ptr, &elevation, sizeof(elevation)); ptr += sizeof(elevation);
    std::memcpy(ptr, &precipitation, sizeof(precipitation)); ptr += sizeof(precipitation);
    
    // Write generation metadata
    std::memcpy(ptr, &generationSeed, sizeof(generationSeed)); ptr += sizeof(generationSeed);
    std::memcpy(ptr, &generationTime, sizeof(generationTime)); ptr += sizeof(generationTime);
    std::memcpy(ptr, &simulationLevel, sizeof(simulationLevel)); ptr += sizeof(simulationLevel);
    
    // Write reserved space
    std::memcpy(ptr, reserved, sizeof(reserved)); ptr += sizeof(reserved);
    
    // Serialize advanced data structures
    std::vector<uint8_t> geologicalBuffer;
    if (!geological.SerializeToBinary(geologicalBuffer)) {
        return false;
    }
    std::memcpy(ptr, geologicalBuffer.data(), geologicalBuffer.size());
    ptr += geologicalBuffer.size();
    
    std::vector<uint8_t> hydrologicalBuffer;
    if (!hydrological.SerializeToBinary(hydrologicalBuffer)) {
        return false;
    }
    std::memcpy(ptr, hydrologicalBuffer.data(), hydrologicalBuffer.size());
    ptr += hydrologicalBuffer.size();
    
    std::vector<uint8_t> climateBuffer;
    if (!climate.SerializeToBinary(climateBuffer)) {
        return false;
    }
    std::memcpy(ptr, climateBuffer.data(), climateBuffer.size());
    
    return true;
}

bool RegionalData::DeserializeFromBinary(const std::vector<uint8_t>& buffer) {
    if (buffer.size() < sizeof(uint32_t) * 4) {
        std::cerr << "DEBUG: Buffer too small for header: " << buffer.size() << " < " << (sizeof(uint32_t) * 4) << std::endl;
        return false; // Buffer too small for header
    }
    
    const uint8_t* ptr = buffer.data();
    
    // Read and validate header
    uint32_t readMagic;
    std::memcpy(&readMagic, ptr, sizeof(readMagic)); ptr += sizeof(readMagic);
    if (readMagic != MAGIC_NUMBER) {
        std::cerr << "DEBUG: Invalid magic number: " << readMagic << " != " << MAGIC_NUMBER << std::endl;
        return false; // Invalid magic number
    }
    
    uint32_t readVersion;
    std::memcpy(&readVersion, ptr, sizeof(readVersion)); ptr += sizeof(readVersion);
    if (readVersion > CURRENT_VERSION) {
        std::cerr << "DEBUG: Version too new: " << readVersion << " > " << CURRENT_VERSION << std::endl;
        return false; // Version too new
    }
    
    std::memcpy(&flags, ptr, sizeof(flags)); ptr += sizeof(flags);
    std::memcpy(&dataSize, ptr, sizeof(dataSize)); ptr += sizeof(dataSize);
    
    // Validate data size
    std::cerr << "DEBUG: Buffer size: " << buffer.size() << ", Expected: " << (sizeof(uint32_t) * 4 + dataSize) << ", dataSize: " << dataSize << std::endl;
    if (buffer.size() != sizeof(uint32_t) * 4 + dataSize) {
        std::cerr << "DEBUG: Size mismatch!" << std::endl;
        return false; // Size mismatch
    }
    
    // Read coordinates
    std::memcpy(&regionX, ptr, sizeof(regionX)); ptr += sizeof(regionX);
    std::memcpy(&regionZ, ptr, sizeof(regionZ)); ptr += sizeof(regionZ);
    
    // Read environmental data
    uint32_t biomeValue;
    std::memcpy(&biomeValue, ptr, sizeof(biomeValue)); ptr += sizeof(biomeValue);
    primaryBiome = static_cast<BiomeType>(biomeValue);
    
    std::memcpy(&temperature, ptr, sizeof(temperature)); ptr += sizeof(temperature);
    std::memcpy(&humidity, ptr, sizeof(humidity)); ptr += sizeof(humidity);
    std::memcpy(&elevation, ptr, sizeof(elevation)); ptr += sizeof(elevation);
    std::memcpy(&precipitation, ptr, sizeof(precipitation)); ptr += sizeof(precipitation);
    
    // Read generation metadata
    std::memcpy(&generationSeed, ptr, sizeof(generationSeed)); ptr += sizeof(generationSeed);
    std::memcpy(&generationTime, ptr, sizeof(generationTime)); ptr += sizeof(generationTime);
    std::memcpy(&simulationLevel, ptr, sizeof(simulationLevel)); ptr += sizeof(simulationLevel);
    
    // Read reserved space
    std::memcpy(reserved, ptr, sizeof(reserved)); ptr += sizeof(reserved);
    
    // Deserialize advanced data structures
    size_t offset = ptr - buffer.data();
    std::cout << "DEBUG: Starting advanced deserialization at offset: " << offset << std::endl;
    
    // Deserialize geological data
    std::cout << "DEBUG: Deserializing geological data..." << std::endl;
    if (!geological.DeserializeFromBinary(buffer, offset)) {
        std::cout << "DEBUG: Geological deserialization FAILED" << std::endl;
        return false;
    }
    std::cout << "DEBUG: Geological deserialization OK, new offset: " << offset << std::endl;
    
    // Deserialize hydrological data
    std::cout << "DEBUG: Deserializing hydrological data..." << std::endl;
    if (!hydrological.DeserializeFromBinary(buffer, offset)) {
        std::cout << "DEBUG: Hydrological deserialization FAILED" << std::endl;
        return false;
    }
    std::cout << "DEBUG: Hydrological deserialization OK, new offset: " << offset << std::endl;
    
    // Deserialize climate data
    std::cout << "DEBUG: Deserializing climate data..." << std::endl;
    if (!climate.DeserializeFromBinary(buffer, offset)) {
        std::cout << "DEBUG: Climate deserialization FAILED" << std::endl;
        return false;
    }
    std::cout << "DEBUG: Climate deserialization OK, new offset: " << offset << std::endl;
    
    // Store version info
    magicNumber = readMagic;
    version = readVersion;
    
    return IsValid();
}

bool RegionalData::SaveToFile(const std::string& filepath) const {
    std::vector<uint8_t> buffer;
    if (!SerializeToBinary(buffer)) {
        return false;
    }
    
    std::ofstream file(filepath, std::ios::binary);
    if (!file.is_open()) {
        return false;
    }
    
    file.write(reinterpret_cast<const char*>(buffer.data()), buffer.size());
    return file.good();
}

bool RegionalData::LoadFromFile(const std::string& filepath) {
    std::ifstream file(filepath, std::ios::binary);
    if (!file.is_open()) {
        return false;
    }
    
    // Get file size
    file.seekg(0, std::ios::end);
    size_t fileSize = file.tellg();
    file.seekg(0, std::ios::beg);
    
    // Read file into buffer
    std::vector<uint8_t> buffer(fileSize);
    file.read(reinterpret_cast<char*>(buffer.data()), fileSize);
    
    if (!file.good()) {
        return false;
    }
    
    return DeserializeFromBinary(buffer);
}

bool RegionalData::IsValid() const {
    // Validate magic number and version
    if (magicNumber != MAGIC_NUMBER || version == 0 || version > CURRENT_VERSION) {
        return false;
    }
    
    // Validate environmental ranges
    if (temperature < -100.0f || temperature > 100.0f) return false;
    if (humidity < 0.0f || humidity > 100.0f) return false;
    if (elevation < -500.0f || elevation > 10000.0f) return false;
    if (precipitation < 0.0f || precipitation > 10000.0f) return false;
    
    // Validate biome type
    if (!IsValidBiomeType(primaryBiome)) {
        return false;
    }
    
    // Validate simulation level
    if (simulationLevel > 100) {
        return false;
    }
    
    // Validate advanced data structures
    if (!geological.IsValid()) {
        return false;
    }
    
    if (!hydrological.IsValid()) {
        return false;
    }
    
    if (!climate.IsValid()) {
        return false;
    }
    
    return true;
}

void RegionalData::SetDefaults() {
    // Set reasonable defaults based on biome type
    switch (primaryBiome) {
        case BiomeType::DESERT:
            temperature = 30.0f;
            humidity = 20.0f;
            precipitation = 200.0f;
            break;
        case BiomeType::MOUNTAINS:
            temperature = 5.0f;
            humidity = 60.0f;
            precipitation = 600.0f;
            break;
        case BiomeType::FOREST:
            temperature = 12.0f;
            humidity = 70.0f;
            precipitation = 1000.0f;
            break;
        case BiomeType::OCEAN:
            temperature = 18.0f;
            humidity = 85.0f;
            precipitation = 1200.0f;
            break;
        case BiomeType::PLAINS:
        default:
            temperature = 15.0f;
            humidity = 60.0f;
            precipitation = 800.0f;
            break;
    }
    
    // Initialize advanced data structures with defaults
    geological.SetDefaults();
    hydrological.SetDefaults();
    climate.SetDefaults();
}

std::string RegionalData::GetRegionFilename() const {
    return GetRegionFilename(regionX, regionZ);
}

std::string RegionalData::GetRegionFilename(int32_t x, int32_t z) {
    std::ostringstream oss;
    oss << "region_" 
        << std::setfill('0') << std::setw(4) << std::abs(x)
        << (x < 0 ? "n" : "p") << "_"
        << std::setfill('0') << std::setw(4) << std::abs(z)
        << (z < 0 ? "n" : "p") << ".bin";
    return oss.str();
}

std::string RegionalData::ToString() const {
    std::ostringstream oss;
    oss << "RegionalData (" << regionX << ", " << regionZ << ")\n"
        << "  Version: " << version << "\n"
        << "  Biome: " << BiomeTypeToString(primaryBiome) << "\n"
        << "  Temperature: " << temperature << "°C\n"
        << "  Humidity: " << humidity << "%\n"
        << "  Elevation: " << elevation << "m\n"
        << "  Precipitation: " << precipitation << "mm/year\n"
        << "  Generation Seed: " << generationSeed << "\n"
        << "  Generation Time: " << generationTime << "\n"
        << "  Simulation Level: " << static_cast<int>(simulationLevel) << "%\n"
        << "  Geological Data: " << (geological.IsValid() ? "Valid" : "Invalid") << "\n"
        << "  Hydrological Data: " << (hydrological.IsValid() ? "Valid" : "Invalid") << "\n"
        << "  Climate Data: " << (climate.IsValid() ? "Valid" : "Invalid") << "\n";
    return oss.str();
}

// RegionalDatabase Implementation

RegionalDatabase::RegionalDatabase() {
}

RegionalDatabase::~RegionalDatabase() {
    Shutdown();
}

bool RegionalDatabase::Initialize(const std::string& worldPath) {
    worldPath_ = worldPath;
    regionsPath_ = worldPath + "/regions";
    
    // Create directories if they don't exist
    return CreateWorldDirectory(worldPath);
}

void RegionalDatabase::Shutdown() {
    ClearCache();
    worldPath_.clear();
    regionsPath_.clear();
}

bool RegionalDatabase::GetRegionalData(int32_t x, int32_t z, RegionalData& data) const {
    std::pair<int32_t, int32_t> coords = {x, z};
    
    // Check cache first
    auto it = cache_.find(coords);
    if (it != cache_.end()) {
        data = it->second;
        // Update access time
        cacheTimestamps_[coords] = GetCurrentTimestamp();
        return true;
    }
    
    // Load from file
    std::string filepath = GetRegionFilePath(x, z);
    if (!data.LoadFromFile(filepath)) {
        return false;
    }
    
    // Add to cache
    if (cache_.size() >= MAX_CACHE_SIZE) {
        EvictOldestCacheEntry();
    }
    cache_[coords] = data;
    cacheTimestamps_[coords] = GetCurrentTimestamp();
    
    return true;
}

bool RegionalDatabase::SetRegionalData(int32_t x, int32_t z, const RegionalData& data) {
    if (!data.IsValid()) {
        return false;
    }
    
    std::string filepath = GetRegionFilePath(x, z);
    if (!data.SaveToFile(filepath)) {
        return false;
    }
    
    // Update cache
    std::pair<int32_t, int32_t> coords = {x, z};
    cache_[coords] = data;
    cacheTimestamps_[coords] = GetCurrentTimestamp();
    
    return true;
}

bool RegionalDatabase::LoadRegionBatch(const std::vector<std::pair<int32_t, int32_t>>& coordinates) {
    for (const auto& coord : coordinates) {
        RegionalData data;
        if (!GetRegionalData(coord.first, coord.second, data)) {
            return false; // If any fails, return false
        }
    }
    return true;
}

void RegionalDatabase::ClearCache() {
    cache_.clear();
    cacheTimestamps_.clear();
}

bool RegionalDatabase::CreateWorldDirectory(const std::string& worldPath) const {
    try {
        // Create the world directory
        std::filesystem::create_directories(worldPath);
        
        // Create the regions subdirectory
        std::string regionsDir = worldPath + "/regions";
        std::filesystem::create_directories(regionsDir);
        
        return true;
    } catch (const std::exception& e) {
        std::cerr << "Failed to create world directory: " << e.what() << std::endl;
        return false;
    }
}

std::string RegionalDatabase::GetRegionFilePath(int32_t x, int32_t z) const {
    return regionsPath_ + "/" + RegionalData::GetRegionFilename(x, z);
}

size_t RegionalDatabase::GetCacheSize() const {
    return cache_.size();
}

std::vector<std::pair<int32_t, int32_t>> RegionalDatabase::GetLoadedRegions() const {
    std::vector<std::pair<int32_t, int32_t>> regions;
    for (const auto& entry : cache_) {
        regions.push_back(entry.first);
    }
    return regions;
}

void RegionalDatabase::EvictOldestCacheEntry() const {
    if (cache_.empty()) return;
    
    auto oldest = cacheTimestamps_.begin();
    for (auto it = cacheTimestamps_.begin(); it != cacheTimestamps_.end(); ++it) {
        if (it->second < oldest->second) {
            oldest = it;
        }
    }
    
    cache_.erase(oldest->first);
    cacheTimestamps_.erase(oldest);
}

uint64_t RegionalDatabase::GetCurrentTimestamp() const {
    return std::chrono::duration_cast<std::chrono::milliseconds>(
        std::chrono::steady_clock::now().time_since_epoch()
    ).count();
}

} // namespace World
} // namespace VoxelCastle
