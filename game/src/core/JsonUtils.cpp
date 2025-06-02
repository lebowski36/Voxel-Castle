#include "core/JsonUtils.h"
#include <sstream>
#include <iomanip>
#include <iostream>
#include <filesystem>

namespace VoxelCastle {
namespace Core {

bool JsonUtils::writeJsonToFile(const std::string& filePath, const std::string& jsonContent) {
    try {
        // Ensure directory exists
        std::filesystem::path path(filePath);
        std::filesystem::create_directories(path.parent_path());
        
        std::ofstream file(filePath);
        if (!file.is_open()) {
            std::cerr << "[JsonUtils] Failed to open file for writing: " << filePath << std::endl;
            return false;
        }
        
        file << jsonContent;
        return file.good();
    } catch (const std::exception& e) {
        std::cerr << "[JsonUtils] Exception writing JSON to file " << filePath << ": " << e.what() << std::endl;
        return false;
    }
}

bool JsonUtils::readJsonFromFile(const std::string& filePath, std::string& jsonContent) {
    try {
        std::ifstream file(filePath);
        if (!file.is_open()) {
            std::cerr << "[JsonUtils] Failed to open file for reading: " << filePath << std::endl;
            return false;
        }
        
        std::stringstream buffer;
        buffer << file.rdbuf();
        jsonContent = buffer.str();
        return true;
    } catch (const std::exception& e) {
        std::cerr << "[JsonUtils] Exception reading JSON from file " << filePath << ": " << e.what() << std::endl;
        return false;
    }
}

std::string JsonUtils::createMetadataJson(
    const std::string& version,
    const std::string& worldName,
    const glm::vec3& playerPosition,
    uint64_t playTimeSeconds,
    const std::string& cameraMode
) {
    std::vector<std::pair<std::string, std::string>> pairs;
    
    pairs.emplace_back("version", "\"" + escapeJsonString(version) + "\"");
    pairs.emplace_back("saveDate", "\"" + getCurrentTimestamp() + "\"");
    pairs.emplace_back("playTime", std::to_string(playTimeSeconds));
    pairs.emplace_back("worldName", "\"" + escapeJsonString(worldName) + "\"");
    
    // Format player position as JSON object
    std::ostringstream posStr;
    posStr << "{\"x\": " << std::fixed << std::setprecision(2) << playerPosition.x
           << ", \"y\": " << playerPosition.y
           << ", \"z\": " << playerPosition.z << "}";
    pairs.emplace_back("playerPosition", posStr.str());
    
    pairs.emplace_back("cameraMode", "\"" + escapeJsonString(cameraMode) + "\"");
    
    return createJsonObject(pairs);
}

bool JsonUtils::parseMetadataJson(
    const std::string& jsonContent,
    std::string& version,
    std::string& worldName,
    glm::vec3& playerPosition,
    uint64_t& playTimeSeconds,
    std::string& cameraMode
) {
    // Simple JSON parsing (for a production system, consider using nlohmann/json or similar)
    // This is a basic implementation for the current needs
    
    try {
        // Extract version
        size_t versionPos = jsonContent.find("\"version\":");
        if (versionPos != std::string::npos) {
            size_t startQuote = jsonContent.find("\"", versionPos + 10);
            size_t endQuote = jsonContent.find("\"", startQuote + 1);
            if (startQuote != std::string::npos && endQuote != std::string::npos) {
                version = jsonContent.substr(startQuote + 1, endQuote - startQuote - 1);
            }
        }
        
        // Extract worldName
        size_t worldNamePos = jsonContent.find("\"worldName\":");
        if (worldNamePos != std::string::npos) {
            size_t startQuote = jsonContent.find("\"", worldNamePos + 12);
            size_t endQuote = jsonContent.find("\"", startQuote + 1);
            if (startQuote != std::string::npos && endQuote != std::string::npos) {
                worldName = jsonContent.substr(startQuote + 1, endQuote - startQuote - 1);
            }
        }
        
        // Extract playTime
        size_t playTimePos = jsonContent.find("\"playTime\":");
        if (playTimePos != std::string::npos) {
            size_t valueStart = jsonContent.find_first_of("0123456789", playTimePos + 11);
            size_t valueEnd = jsonContent.find_first_not_of("0123456789", valueStart);
            if (valueStart != std::string::npos && valueEnd != std::string::npos) {
                std::string playTimeStr = jsonContent.substr(valueStart, valueEnd - valueStart);
                playTimeSeconds = std::stoull(playTimeStr);
            }
        }
        
        // Extract cameraMode
        size_t cameraModePos = jsonContent.find("\"cameraMode\":");
        if (cameraModePos != std::string::npos) {
            size_t startQuote = jsonContent.find("\"", cameraModePos + 13);
            size_t endQuote = jsonContent.find("\"", startQuote + 1);
            if (startQuote != std::string::npos && endQuote != std::string::npos) {
                cameraMode = jsonContent.substr(startQuote + 1, endQuote - startQuote - 1);
            }
        }
        
        // Extract player position (simplified)
        size_t playerPosPos = jsonContent.find("\"playerPosition\":");
        if (playerPosPos != std::string::npos) {
            // Find the x, y, z values
            size_t xPos = jsonContent.find("\"x\":", playerPosPos);
            size_t yPos = jsonContent.find("\"y\":", playerPosPos);
            size_t zPos = jsonContent.find("\"z\":", playerPosPos);
            
            if (xPos != std::string::npos && yPos != std::string::npos && zPos != std::string::npos) {
                // Extract x value
                size_t xValueStart = jsonContent.find_first_of("-0123456789.", xPos + 4);
                size_t xValueEnd = jsonContent.find_first_of(",}", xValueStart);
                if (xValueStart != std::string::npos && xValueEnd != std::string::npos) {
                    playerPosition.x = std::stof(jsonContent.substr(xValueStart, xValueEnd - xValueStart));
                }
                
                // Extract y value
                size_t yValueStart = jsonContent.find_first_of("-0123456789.", yPos + 4);
                size_t yValueEnd = jsonContent.find_first_of(",}", yValueStart);
                if (yValueStart != std::string::npos && yValueEnd != std::string::npos) {
                    playerPosition.y = std::stof(jsonContent.substr(yValueStart, yValueEnd - yValueStart));
                }
                
                // Extract z value
                size_t zValueStart = jsonContent.find_first_of("-0123456789.", zPos + 4);
                size_t zValueEnd = jsonContent.find_first_of(",}", zValueStart);
                if (zValueStart != std::string::npos && zValueEnd != std::string::npos) {
                    playerPosition.z = std::stof(jsonContent.substr(zValueStart, zValueEnd - zValueStart));
                }
            }
        }
        
        return true;
    } catch (const std::exception& e) {
        std::cerr << "[JsonUtils] Exception parsing metadata JSON: " << e.what() << std::endl;
        return false;
    }
}

std::string JsonUtils::getCurrentTimestamp() {
    return formatTimestamp(std::chrono::system_clock::now());
}

std::string JsonUtils::formatTimestamp(const std::chrono::system_clock::time_point& timePoint) {
    auto time_t = std::chrono::system_clock::to_time_t(timePoint);
    auto tm = *std::gmtime(&time_t);
    
    std::ostringstream oss;
    oss << std::put_time(&tm, "%Y-%m-%dT%H:%M:%SZ");
    return oss.str();
}

std::chrono::system_clock::time_point JsonUtils::parseTimestamp(const std::string& timestampStr) {
    // Simple timestamp parsing - for production, consider using more robust parsing
    std::tm tm = {};
    std::istringstream ss(timestampStr);
    ss >> std::get_time(&tm, "%Y-%m-%dT%H:%M:%SZ");
    
    if (ss.fail()) {
        return std::chrono::system_clock::time_point{}; // Return epoch
    }
    
    return std::chrono::system_clock::from_time_t(std::mktime(&tm));
}

std::string JsonUtils::escapeJsonString(const std::string& str) {
    std::string escaped;
    escaped.reserve(str.size() + str.size() / 4); // Reserve some extra space for escapes
    
    for (char c : str) {
        switch (c) {
            case '"':  escaped += "\\\""; break;
            case '\\': escaped += "\\\\"; break;
            case '\b': escaped += "\\b"; break;
            case '\f': escaped += "\\f"; break;
            case '\n': escaped += "\\n"; break;
            case '\r': escaped += "\\r"; break;
            case '\t': escaped += "\\t"; break;
            default:
                if (c < 0x20) {
                    // Control characters
                    escaped += "\\u";
                    escaped += "0000";
                    escaped[escaped.size() - 2] = "0123456789abcdef"[(c >> 4) & 0xF];
                    escaped[escaped.size() - 1] = "0123456789abcdef"[c & 0xF];
                } else {
                    escaped += c;
                }
                break;
        }
    }
    
    return escaped;
}

std::string JsonUtils::createJsonObject(const std::vector<std::pair<std::string, std::string>>& pairs) {
    std::ostringstream json;
    json << "{\n";
    
    for (size_t i = 0; i < pairs.size(); ++i) {
        json << "  \"" << pairs[i].first << "\": " << pairs[i].second;
        if (i < pairs.size() - 1) {
            json << ",";
        }
        json << "\n";
    }
    
    json << "}";
    return json.str();
}

} // namespace Core
} // namespace VoxelCastle
