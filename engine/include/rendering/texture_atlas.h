#ifndef VOXEL_ENGINE_TEXTURE_ATLAS_H
#define VOXEL_ENGINE_TEXTURE_ATLAS_H

#include "world/voxel_types.h"
#include <glad/glad.h>
#include <glm/vec2.hpp>
#include <glm/vec4.hpp>
#include <string>
#include <unordered_map>

namespace VoxelEngine {
namespace Rendering {

// Assuming a 256x256 texture atlas with 16x16 pixel tiles.
inline constexpr float ATLAS_WIDTH_PX = 256.0f;
inline constexpr float ATLAS_HEIGHT_PX = 256.0f;
inline constexpr float TILE_WIDTH_PX = 16.0f;
inline constexpr float TILE_HEIGHT_PX = 16.0f;

inline constexpr float TILE_UV_WIDTH = TILE_WIDTH_PX / ATLAS_WIDTH_PX;
inline constexpr float TILE_UV_HEIGHT = TILE_HEIGHT_PX / ATLAS_HEIGHT_PX;

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
    TextureAtlas();
    ~TextureAtlas();

    TextureCoordinates getTextureCoordinates(VoxelEngine::World::VoxelType type) const;

    // New methods for texture ID management
    bool loadTexture(const std::string& texturePath);
    GLuint getTextureID() const { return m_texture_id; }
    void setTextureID(GLuint id) { m_texture_id = id; }
    bool isTextureLoaded() const { return m_texture_id != 0; }
    void releaseTexture(); // Clean up GPU resources

private:
    std::unordered_map<VoxelEngine::World::VoxelType, TextureCoordinates> m_voxel_texture_coords;
    GLuint m_texture_id = 0; // Store the OpenGL texture ID
};

} // namespace Rendering
} // namespace VoxelEngine

#endif // VOXEL_ENGINE_TEXTURE_ATLAS_H
