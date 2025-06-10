#include "world/world_metadata.h"
#include <fstream>
#include <filesystem>
#include <iomanip>
#include <sstream>
#include <iostream>
#include <nlohmann/json.hpp>

using json = nlohmann::json;
namespace fs = std::filesystem;

namespace VoxelCastle {
namespace World {

WorldMetadata::WorldMetadata(const std::string& name, uint64_t worldSeed)
    : worldName(name)
    , seed(worldSeed)
    , createdDate(std::chrono::system_clock::now())
    , lastPlayed(std::chrono::system_clock::now())
{
}

bool WorldMetadata::SaveToFile(const std::string& worldPath) const {
    try {
        // Ensure the world directory exists
        fs::create_directories(worldPath);
        
        // Create JSON object
        json j;
        j["version"] = version;
        j["worldName"] = worldName;
        j["seed"] = seed;
        j["gameMode"] = gameMode;
        j["worldType"] = worldType;
        j["spawnX"] = spawnX;
        j["spawnY"] = spawnY;
        j["spawnZ"] = spawnZ;
        j["generateStructures"] = generateStructures;
        
        // Convert timestamps to ISO 8601 strings
        auto created_time_t = std::chrono::system_clock::to_time_t(createdDate);
        auto lastplayed_time_t = std::chrono::system_clock::to_time_t(lastPlayed);
        
        std::stringstream ss;
        ss << std::put_time(std::gmtime(&created_time_t), "%Y-%m-%dT%H:%M:%SZ");
        j["createdDate"] = ss.str();
        
        ss.str("");
        ss.clear();
        ss << std::put_time(std::gmtime(&lastplayed_time_t), "%Y-%m-%dT%H:%M:%SZ");
        j["lastPlayed"] = ss.str();
        
        j["playTime"] = playTime;
        
        // Write to file
        std::string levelDatPath = worldPath + "/level.dat";
        std::ofstream file(levelDatPath);
        if (!file.is_open()) {
            std::cerr << "[ERROR] Failed to open level.dat for writing: " << levelDatPath << std::endl;
            return false;
        }
        
        file << j.dump(4); // Pretty print with 4-space indentation
        file.close();
        
        std::cout << "[INFO] Saved world metadata to: " << levelDatPath << std::endl;
        return true;
        
    } catch (const std::exception& e) {
        std::cerr << "[ERROR] Failed to save world metadata: " << e.what() << std::endl;
        return false;
    }
}

bool WorldMetadata::LoadFromFile(const std::string& worldPath) {
    try {
        std::string levelDatPath = worldPath + "/level.dat";
        
        // Check if file exists
        if (!fs::exists(levelDatPath)) {
            std::cerr << "[ERROR] level.dat not found: " << levelDatPath << std::endl;
            return false;
        }
        
        // Read file
        std::ifstream file(levelDatPath);
        if (!file.is_open()) {
            std::cerr << "[ERROR] Failed to open level.dat for reading: " << levelDatPath << std::endl;
            return false;
        }
        
        json j;
        file >> j;
        file.close();
        
        // Parse JSON
        version = j.value("version", 1);
        worldName = j.value("worldName", "Unknown World");
        seed = j.value("seed", static_cast<uint64_t>(0));
        gameMode = j.value("gameMode", "creative");
        worldType = j.value("worldType", "normal");
        spawnX = j.value("spawnX", 0.0f);
        spawnY = j.value("spawnY", 64.0f);
        spawnZ = j.value("spawnZ", 0.0f);
        generateStructures = j.value("generateStructures", true);
        playTime = j.value("playTime", static_cast<uint64_t>(0));
        
        // Parse timestamps - simplified parsing for now
        if (j.contains("createdDate")) {
            // For now, just set to current time if parsing fails
            // TODO: Implement proper ISO 8601 parsing
            createdDate = std::chrono::system_clock::now();
        }
        
        if (j.contains("lastPlayed")) {
            lastPlayed = std::chrono::system_clock::now();
        }
        
        std::cout << "[INFO] Loaded world metadata from: " << levelDatPath << std::endl;
        return true;
        
    } catch (const std::exception& e) {
        std::cerr << "[ERROR] Failed to load world metadata: " << e.what() << std::endl;
        return false;
    }
}

std::string WorldMetadata::GetCreatedDateString() const {
    return FormatTimePoint(createdDate);
}

std::string WorldMetadata::GetLastPlayedString() const {
    return FormatTimePoint(lastPlayed);
}

std::string WorldMetadata::GetPlayTimeString() const {
    uint64_t hours = playTime / 3600;
    uint64_t minutes = (playTime % 3600) / 60;
    uint64_t seconds = playTime % 60;
    
    std::stringstream ss;
    ss << hours << "h " << minutes << "m " << seconds << "s";
    return ss.str();
}

std::string WorldMetadata::FormatTimePoint(const std::chrono::system_clock::time_point& tp) const {
    auto time_t = std::chrono::system_clock::to_time_t(tp);
    std::stringstream ss;
    ss << std::put_time(std::localtime(&time_t), "%Y-%m-%d %H:%M:%S");
    return ss.str();
}

} // namespace World
} // namespace VoxelCastle
