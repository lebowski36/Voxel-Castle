#ifndef DEBUG_TEXT_H
#define DEBUG_TEXT_H

#include <string>
#include <glm/glm.hpp>

// Forward declaration if VoxelMesh includes this, and this needs VoxelMesh
// Or vice-versa. For now, assume simple struct.

namespace VoxelEngine {
namespace Rendering {

struct DebugTextInfo {
    std::string text;        // The text to display (e.g., "(X,Y,Z)")
    glm::vec3 worldPosition; // Base world position on the face (e.g., voxel's world coordinate)
    glm::vec3 faceNormal;    // Normal of the face the text is on
    // Optional: Could add color, scale if they vary per text item
};

} // namespace Rendering
} // namespace VoxelEngine

#endif // DEBUG_TEXT_H
