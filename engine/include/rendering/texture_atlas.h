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
        // Assuming voxel IDs 0 through 7 are used as per VoxelType enum
        for (int voxel_id_int = static_cast<int>(VoxelEngine::World::VoxelType::AIR); voxel_id_int <= static_cast<int>(VoxelEngine::World::VoxelType::SAND); ++voxel_id_int) {
            VoxelEngine::World::VoxelType type = static_cast<VoxelEngine::World::VoxelType>(voxel_id_int);
            
            // tile_idx_x is the column index of the tile in the atlas (0-indexed from left)
            int tile_idx_x = voxel_id_int % tiles_per_row;
            // tile_idx_y is the row index of the tile in the atlas (0-indexed from top)
            int tile_idx_y = voxel_id_int / tiles_per_row; 

            float u_min = static_cast<float>(tile_idx_x) * TILE_UV_WIDTH;
            float u_max = static_cast<float>(tile_idx_x + 1) * TILE_UV_WIDTH;

            // Assuming OpenGL UV origin (0,0) is bottom-left, 
            // and the atlas image file has its origin (0,0) at the top-left (typical for PNGs loaded by stb_image by default).
            // We need to convert the V coordinate from image space (top-down) to OpenGL UV space (bottom-up).
            // The V coordinate for the bottom edge of the tile in GL UV space:
            float v_gl_min = 1.0f - (static_cast<float>(tile_idx_y + 1) * TILE_UV_HEIGHT);
            // The V coordinate for the top edge of the tile in GL UV space:
            float v_gl_max = 1.0f - (static_cast<float>(tile_idx_y) * TILE_UV_HEIGHT);

            m_voxel_texture_coords[type] = {
                {u_min, v_gl_min}, // uv_min (bottom-left of the tile in OpenGL UV space)
                {u_max, v_gl_max}  // uv_max (top-right of the tile in OpenGL UV space)
            };
        }
    }

    TextureCoordinates getTextureCoordinates(VoxelEngine::World::VoxelType type) const {
        auto it = m_voxel_texture_coords.find(type);
        if (it != m_voxel_texture_coords.end()) {
            return it->second;
        }
        // Fallback to AIR tile (ID 0) if type not found.
        // This should also follow the corrected V coordinate logic.
        // Assuming VoxelType::AIR is 0.
        int fallback_voxel_id = static_cast<int>(VoxelEngine::World::VoxelType::AIR);
        int tiles_per_row = static_cast<int>(ATLAS_WIDTH_PX / TILE_WIDTH_PX);

        int fallback_tile_idx_x = fallback_voxel_id % tiles_per_row;
        int fallback_tile_idx_y = fallback_voxel_id / tiles_per_row;

        float u_min_fallback = static_cast<float>(fallback_tile_idx_x) * TILE_UV_WIDTH;
        float u_max_fallback = static_cast<float>(fallback_tile_idx_x + 1) * TILE_UV_WIDTH;
        
        float v_min_fallback_gl = 1.0f - (static_cast<float>(fallback_tile_idx_y + 1) * TILE_UV_HEIGHT);
        float v_max_fallback_gl = 1.0f - (static_cast<float>(fallback_tile_idx_y) * TILE_UV_HEIGHT);

        return {{u_min_fallback, v_min_fallback_gl}, {u_max_fallback, v_max_fallback_gl}};
    }

private:
    std::unordered_map<VoxelEngine::World::VoxelType, TextureCoordinates> m_voxel_texture_coords;
};

} // namespace Rendering
} // namespace VoxelEngine

#endif // VOXEL_ENGINE_TEXTURE_ATLAS_H
