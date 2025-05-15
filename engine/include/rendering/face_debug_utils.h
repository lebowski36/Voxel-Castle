#pragma once
#include <glm/glm.hpp>
#include <cstdint>

namespace VoxelEngine {
namespace Rendering {

// Encodes a direction ID into a vec4 color for debug visualization
inline glm::vec4 encodeFaceDebugColor(int directionId) {
    // Ensure alpha is always 1.0 for debug mode, colors are distinct
    switch (directionId) {
        case 1:  return glm::vec4(1.0f, 0.0f, 0.0f, 1.0f); // +X (Right) = Red
        case 2:  return glm::vec4(0.0f, 1.0f, 1.0f, 1.0f); // -X (Left)  = Cyan
        case 3:  return glm::vec4(0.0f, 1.0f, 0.0f, 1.0f); // +Y (Top)   = Green
        case 4:  return glm::vec4(1.0f, 0.0f, 1.0f, 1.0f); // -Y (Bottom)= Magenta
        case 5:  return glm::vec4(0.0f, 0.0f, 1.0f, 1.0f); // +Z (Front) = Blue
        case 6:  return glm::vec4(1.0f, 1.0f, 0.0f, 1.0f); // -Z (Back)  = Yellow
        default: return glm::vec4(0.5f, 0.5f, 0.5f, 1.0f); // Unknown = Grey
    }
}

} // namespace Rendering
} // namespace VoxelEngine
