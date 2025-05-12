#ifndef VOXEL_ENGINE_TEXTURE_ATLAS_H
#define VOXEL_ENGINE_TEXTURE_ATLAS_H

#include "world/voxel_types.h"
#include <glm/vec2.hpp>
#include <glm/vec4.hpp>
#include <unordered_map>

namespace VoxelEngine {
namespace Rendering {

// Assuming a 256x256 texture atlas with 16x16 pixel tiles.
const float ATLAS_WIDTH_PX = 256.0f;
const float ATLAS_HEIGHT_PX = 256.0f;
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
        // Initialize texture coordinates for each voxel type
        // This is a placeholder and needs to be defined based on the actual atlas layout
        // For simplicity, let's assign them in order of VoxelType enum
        // This assumes VoxelType enum values are contiguous and start from 0 (AIR).

        int tiles_per_row = static_cast<int>(ATLAS_WIDTH_PX / TILE_WIDTH_PX);

        // Example: Map VoxelType::STONE to the first tile (0,0)
        // VoxelType::GRASS to the second tile (1,0) etc.
        // Note: AIR (0) might not have a texture or be a fully transparent tile.

        // For VoxelType::STONE (assuming it's 1 after AIR)
        if (static_cast<uint8_t>(VoxelEngine::World::VoxelType::STONE) < 256) {
            int stone_idx = static_cast<int>(VoxelEngine::World::VoxelType::STONE);
            float u_stone = (stone_idx % tiles_per_row) * TILE_UV_WIDTH;
            float v_stone = (stone_idx / tiles_per_row) * TILE_UV_HEIGHT;
            m_voxel_texture_coords[VoxelEngine::World::VoxelType::STONE] = {
                {u_stone, v_stone + TILE_UV_HEIGHT}, // Min (bottom-left in UV, so V is higher)
                {u_stone + TILE_UV_WIDTH, v_stone}   // Max (top-right in UV, so V is lower)
            };
        }
        
        // For VoxelType::DIRT (assuming it's 2)
         if (static_cast<uint8_t>(VoxelEngine::World::VoxelType::DIRT) < 256) {
            int dirt_idx = static_cast<int>(VoxelEngine::World::VoxelType::DIRT);
            float u_dirt = (dirt_idx % tiles_per_row) * TILE_UV_WIDTH;
            float v_dirt = (dirt_idx / tiles_per_row) * TILE_UV_HEIGHT;
            m_voxel_texture_coords[VoxelEngine::World::VoxelType::DIRT] = {
                {u_dirt, v_dirt + TILE_UV_HEIGHT}, 
                {u_dirt + TILE_UV_WIDTH, v_dirt}   
            };
        }

        // For VoxelType::GRASS (assuming it's 3)
        // Special case for grass: top, side, bottom might differ
        // For now, let's use a single texture for all faces.
        // Later, we can extend this to support VoxelFaceType.
         if (static_cast<uint8_t>(VoxelEngine::World::VoxelType::GRASS) < 256) {
            int grass_idx = static_cast<int>(VoxelEngine::World::VoxelType::GRASS); // Example: Grass is the 3rd tile
            float u_grass = (grass_idx % tiles_per_row) * TILE_UV_WIDTH;
            float v_grass = (grass_idx / tiles_per_row) * TILE_UV_HEIGHT;
            m_voxel_texture_coords[VoxelEngine::World::VoxelType::GRASS] = {
                {u_grass, v_grass + TILE_UV_HEIGHT},
                {u_grass + TILE_UV_WIDTH, v_grass}
            };
        }
        
        // For VoxelType::WOOD (assuming it's 4)
        if (static_cast<uint8_t>(VoxelEngine::World::VoxelType::WOOD) < 256) {
            int wood_idx = static_cast<int>(VoxelEngine::World::VoxelType::WOOD);
            float u_wood = (wood_idx % tiles_per_row) * TILE_UV_WIDTH;
            float v_wood = (wood_idx / tiles_per_row) * TILE_UV_HEIGHT;
            m_voxel_texture_coords[VoxelEngine::World::VoxelType::WOOD] = {
                {u_wood, v_wood + TILE_UV_HEIGHT},
                {u_wood + TILE_UV_WIDTH, v_wood}
            };
        }
        
        // ... and so on for other voxel types.
        // A more robust system would load this from a configuration file.
    }

    TextureCoordinates getTextureCoordinates(VoxelEngine::World::VoxelType type) const {
        auto it = m_voxel_texture_coords.find(type);
        if (it != m_voxel_texture_coords.end()) {
            return it->second;
        }
        // Return a default/error texture coordinate (e.g., a magenta square)
        // For now, returning stone if not found, or a specific debug tile.
        // Let's use tile (0,0) as a fallback for unmapped types.
        return {{0.0f, TILE_UV_HEIGHT}, {TILE_UV_WIDTH, 0.0f}};
    }

private:
    std::unordered_map<VoxelEngine::World::VoxelType, TextureCoordinates> m_voxel_texture_coords;
};

} // namespace Rendering
} // namespace VoxelEngine

#endif // VOXEL_ENGINE_TEXTURE_ATLAS_H
