#ifndef VOXELCASTLE_CORE_JSON_UTILS_H
#define VOXELCASTLE_CORE_JSON_UTILS_H

#include <string>
#include <fstream>
#include <glm/vec3.hpp>
#include <chrono>
#include <vector>

namespace VoxelCastle {
namespace Core {

/**
 * @class JsonUtils
 * @brief Utility functions for JSON operations in the save system.
 */
class JsonUtils {
public:
    /**
     * @brief Write a JSON string to a file.
     * @param filePath The path to the file to write.
     * @param jsonContent The JSON content as a string.
     * @return True if successful, false otherwise.
     */
    static bool writeJsonToFile(const std::string& filePath, const std::string& jsonContent);

    /**
     * @brief Read JSON content from a file.
     * @param filePath The path to the file to read.
     * @param jsonContent Output parameter for the JSON content.
     * @return True if successful, false otherwise.
     */
    static bool readJsonFromFile(const std::string& filePath, std::string& jsonContent);

    /**
     * @brief Create metadata JSON for a save file.
     * @param version The save format version.
     * @param worldName The name of the world.
     * @param playerPosition The player's current position.
     * @param playTimeSeconds Total play time in seconds.
     * @param cameraMode The current camera mode.
     * @param cameraYaw Camera yaw angle in degrees.
     * @param cameraPitch Camera pitch angle in degrees.
     * @return JSON string containing the metadata.
     */
    static std::string createMetadataJson(
        const std::string& version,
        const std::string& worldName,
        const glm::vec3& playerPosition,
        uint64_t playTimeSeconds,
        const std::string& cameraMode,
        float cameraYaw = 0.0f,
        float cameraPitch = 0.0f
    );

    /**
     * @brief Parse metadata JSON from a string.
     * @param jsonContent The JSON content to parse.
     * @param version Output parameter for the save format version.
     * @param worldName Output parameter for the world name.
     * @param playerPosition Output parameter for the player position.
     * @param playTimeSeconds Output parameter for play time.
     * @param cameraMode Output parameter for camera mode.
     * @param cameraYaw Output parameter for camera yaw angle.
     * @param cameraPitch Output parameter for camera pitch angle.
     * @return True if parsing was successful, false otherwise.
     */
    static bool parseMetadataJson(
        const std::string& jsonContent,
        std::string& version,
        std::string& worldName,
        glm::vec3& playerPosition,
        uint64_t& playTimeSeconds,
        std::string& cameraMode,
        float& cameraYaw,
        float& cameraPitch
    );

    /**
     * @brief Get current timestamp in ISO 8601 format.
     * @return Current timestamp as a string.
     */
    static std::string getCurrentTimestamp();

    /**
     * @brief Format a time point to ISO 8601 string.
     * @param timePoint The time point to format.
     * @return Formatted timestamp string.
     */
    static std::string formatTimestamp(const std::chrono::system_clock::time_point& timePoint);

    /**
     * @brief Parse an ISO 8601 timestamp string to a time point.
     * @param timestampStr The timestamp string to parse.
     * @return The parsed time point, or epoch if parsing fails.
     */
    static std::chrono::system_clock::time_point parseTimestamp(const std::string& timestampStr);

private:
    /**
     * @brief Escape a string for JSON.
     * @param str The string to escape.
     * @return The escaped string.
     */
    static std::string escapeJsonString(const std::string& str);

    /**
     * @brief Create a JSON object string from key-value pairs.
     * @param pairs Vector of key-value pairs.
     * @return JSON object string.
     */
    static std::string createJsonObject(const std::vector<std::pair<std::string, std::string>>& pairs);
};

} // namespace Core
} // namespace VoxelCastle

#endif // VOXELCASTLE_CORE_JSON_UTILS_H
