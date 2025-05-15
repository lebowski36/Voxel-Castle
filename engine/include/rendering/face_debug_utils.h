#pragma once
#include <glm/glm.hpp>
#include <cstdint>

namespace VoxelEngine {
namespace Rendering {

// Encodes (x, y, z, dir) into a vec4 color for debug visualization
inline glm::vec4 encodeFaceDebugColor(int x, int y, int z, int dir) {
    // Map x, y, z, dir to [0,1] range (assuming chunk size < 256)
    return glm::vec4(
        float(x & 0xFF) / 255.0f,
        float(y & 0xFF) / 255.0f,
        float(z & 0xFF) / 255.0f,
        float((dir + 1) & 0x3) / 3.0f // dir: -1 or +1 mapped to 0 or 1
    );
}

} // namespace Rendering
} // namespace VoxelEngine
