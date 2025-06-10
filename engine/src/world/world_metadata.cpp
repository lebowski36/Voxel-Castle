#include "world/world_metadata.h"
#include <fstream>
#include <filesystem>
#include <iomanip>
#include <sstream>
#include <iostream>

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
        
        // Convert timestamps to ISO 8601 strings
        auto created_time_t = std::chrono::system_clock::to_time_t(createdDate);
        auto lastplayed_time_t = std::chrono::system_clock::to_time_t(lastPlayed);
        
        std::stringstream createdSS, lastPlayedSS;
        createdSS << std::put_time(std::gmtime(&created_time_t), "%Y-%m-%dT%H:%M:%SZ");
        lastPlayedSS << std::put_time(std::gmtime(&lastplayed_time_t), "%Y-%m-%dT%H:%M:%SZ");
        
        // Write simple JSON format manually
        std::string levelDatPath = worldPath + "/level.dat";
        std::ofstream file(levelDatPath);
        if (!file.is_open()) {
            std::cerr << "[ERROR] Failed to open level.dat for writing: " << levelDatPath << std::endl;
            return false;
        }
        
        file << "{\n";
        file << "    \"version\": " << version << ",\n";
        file << "    \"worldName\": \"" << worldName << "\",\n";
        file << "    \"seed\": " << seed << ",\n";
        file << "    \"gameMode\": \"" << gameMode << "\",\n";
        file << "    \"worldType\": \"" << worldType << "\",\n";
        file << "    \"spawnX\": " << spawnX << ",\n";
        file << "    \"spawnY\": " << spawnY << ",\n";
        file << "    \"spawnZ\": " << spawnZ << ",\n";
        file << "    \"generateStructures\": " << (generateStructures ? "true" : "false") << ",\n";
        file << "    \"worldSize\": " << worldSize << ",\n";
        file << "    \"mountainRanges\": " << mountainRanges << ",\n";
        file << "    \"majorRivers\": " << majorRivers << ",\n";
        file << "    \"biomesIdentified\": " << biomesIdentified << ",\n";
        file << "    \"simulationYears\": " << simulationYears << ",\n";
        file << "    \"highestPeak\": " << highestPeak << ",\n";
        file << "    \"generatorName\": \"" << generatorName << "\",\n";
        file << "    \"generatorVersion\": \"" << generatorVersion << "\",\n";
        file << "    \"createdDate\": \"" << createdSS.str() << "\",\n";
        file << "    \"lastPlayed\": \"" << lastPlayedSS.str() << "\",\n";
        file << "    \"playTime\": " << playTime << "\n";
        file << "}\n";
        
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
        
        // Simple JSON parser - read line by line and extract values
        std::string line;
        while (std::getline(file, line)) {
            // Remove whitespace and find key-value pairs
            size_t colonPos = line.find(':');
            if (colonPos != std::string::npos) {
                std::string key = line.substr(0, colonPos);
                std::string value = line.substr(colonPos + 1);
                
                // Clean up key (remove quotes and whitespace)
                key.erase(0, key.find_first_not_of(" \t\""));
                key.erase(key.find_last_not_of(" \t\"") + 1);
                
                // Clean up value (remove quotes, whitespace, commas)
                value.erase(0, value.find_first_not_of(" \t\""));
                value.erase(value.find_last_not_of(" \t\",") + 1);
                
                // Parse based on key
                if (key == "version") {
                    version = std::stoi(value);
                } else if (key == "worldName") {
                    worldName = value;
                } else if (key == "seed") {
                    seed = std::stoull(value);
                } else if (key == "gameMode") {
                    gameMode = value;
                } else if (key == "worldType") {
                    worldType = value;
                } else if (key == "spawnX") {
                    spawnX = std::stof(value);
                } else if (key == "spawnY") {
                    spawnY = std::stof(value);
                } else if (key == "spawnZ") {
                    spawnZ = std::stof(value);
                } else if (key == "generateStructures") {
                    generateStructures = (value == "true");
                } else if (key == "worldSize") {
                    worldSize = std::stoi(value);
                } else if (key == "mountainRanges") {
                    mountainRanges = std::stoi(value);
                } else if (key == "majorRivers") {
                    majorRivers = std::stoi(value);
                } else if (key == "biomesIdentified") {
                    biomesIdentified = std::stoi(value);
                } else if (key == "simulationYears") {
                    simulationYears = std::stoi(value);
                } else if (key == "highestPeak") {
                    highestPeak = std::stof(value);
                } else if (key == "generatorName") {
                    generatorName = value;
                } else if (key == "generatorVersion") {
                    generatorVersion = value;
                } else if (key == "playTime") {
                    playTime = std::stoull(value);
                }
                // Note: Timestamp parsing is complex, skip for now and use current time
            }
        }
        
        file.close();
        
        // Set timestamps to current time for now
        createdDate = std::chrono::system_clock::now();
        lastPlayed = std::chrono::system_clock::now();
        
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
