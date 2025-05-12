#ifndef VOXEL_ENGINE_TEXTURE_ATLAS_H
#define VOXEL_ENGINE_TEXTURE_ATLAS_H

#include "world/voxel_types.h"
#include <glm/vec2.hpp>
#include <glm/vec4.hpp>
#include <unordered_map>

namespace VoxelEngine {
namespace Rendering {

// Assuming a 256x256 texture atlas with 16x16 pixel tiles.
const float ATLAS_WIDTH_PX = 256.0f; // Corrected: 16 tiles * 16px/tile = 256px
const float ATLAS_HEIGHT_PX = 256.0f;  // Corrected: 16 rows * 16px/tile = 256px
const float TILE_WIDTH_PX = 16.0f;
const float TILE_HEIGHT_PX = 16.0f;

const float TILE_UV_WIDTH = TILE_WIDTH_PX / ATLAS_WIDTH_PX;
const float TILE_UV_HEIGHT = TILE_HEIGHT_PX / ATLAS_HEIGHT_PX;

struct TextureCoordinates {
    glm::vec2 uv_min; // Bottom-left
    glm::vec2 uv_max; // Top-right

    // Convenience to get all four corners for a quad
    // Order: Bottom-left, Bottom-right, Top-right, Top-left (standard for quads)
    glm::vec4 getUVRect() const {
        return glm::vec4(uv_min.x, uv_min.y, uv_max.x, uv_max.y);
    }
    // Specific corners if needed for individual vertex assignment
    glm::vec2 getBottomLeft() const { return uv_min; }
    glm::vec2 getBottomRight() const { return glm::vec2(uv_max.x, uv_min.y); }
    glm::vec2 getTopRight() const { return uv_max; }
    glm::vec2 getTopLeft() const { return glm::vec2(uv_min.x, uv_max.y); }
};

class TextureAtlas {
public:
    TextureAtlas() {
        int tiles_per_row = static_cast<int>(ATLAS_WIDTH_PX / TILE_WIDTH_PX);

        // Map all voxel types (AIR, STONE, DIRT, GRASS, WOOD, LEAVES, WATER, SAND)
        for (int voxel_id = 0; voxel_id <= 7; ++voxel_id) {
            VoxelEngine::World::VoxelType type = static_cast<VoxelEngine::World::VoxelType>(voxel_id);
            float u = (voxel_id % tiles_per_row) * TILE_UV_WIDTH;
            float v = (voxel_id / tiles_per_row) * TILE_UV_HEIGHT;
            m_voxel_texture_coords[type] = {
                {u, v}, // uv_min (bottom-left of the tile)
                {u + TILE_UV_WIDTH, v + TILE_UV_HEIGHT}   // uv_max (top-right of the tile)
            };
        }
    }

    TextureCoordinates getTextureCoordinates(VoxelEngine::World::VoxelType type) const {
        auto it = m_voxel_texture_coords.find(type);
        if (it != m_voxel_texture_coords.end()) {
            return it->second;
        }
        // Return a default/error texture coordinate
        // For now, let's return coordinates for the first tile (ID 0) as a fallback.
        int tiles_per_row = static_cast<int>(ATLAS_WIDTH_PX / TILE_WIDTH_PX);
        float u_fallback = (0 % tiles_per_row) * TILE_UV_WIDTH;
        float v_fallback = (0 / tiles_per_row) * TILE_UV_HEIGHT;
        return {{u_fallback, v_fallback}, {u_fallback + TILE_UV_WIDTH, v_fallback + TILE_UV_HEIGHT}};
    }

private:
    std::unordered_map<VoxelEngine::World::VoxelType, TextureCoordinates> m_voxel_texture_coords;
};

} // namespace Rendering
} // namespace VoxelEngine

#endif // VOXEL_ENGINE_TEXTURE_ATLAS_H
