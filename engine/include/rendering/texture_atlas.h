#ifndef VOXEL_ENGINE_TEXTURE_ATLAS_H
#define VOXEL_ENGINE_TEXTURE_ATLAS_H

#include "world/voxel_types.h"
#include "world/voxel_face_patterns.h"
#include <glad/glad.h>
#include <glm/vec2.hpp>
#include <glm/vec4.hpp>
#include <string>
#include <unordered_map>
#include <vector>
#include <memory>

namespace VoxelEngine {
namespace Rendering {

// Metadata-driven atlas configuration (replaces hardcoded constants)
struct AtlasMetadata {
    int tile_size_px = 32;
    int max_grid_size = 16;
    int total_blocks = 256;
};

struct AtlasFileInfo {
    std::string filename;
    int grid_width = 16;
    int grid_height = 16;
    int total_slots = 256;
    int used_slots = 256;
    float efficiency = 100.0f;
    std::unordered_map<int, std::pair<int, int>> block_to_slot; // block_id -> (slot_x, slot_y)
};

struct AtlasTypeInfo {
    std::vector<AtlasFileInfo> files;
    std::vector<GLuint> texture_ids;
};

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

enum class AtlasType {
    MAIN = 0,    // Top faces and uniform blocks
    SIDE = 1,    // Side faces
    BOTTOM = 2   // Bottom faces
};

class TextureAtlas {
public:
    TextureAtlas();
    ~TextureAtlas();

    TextureCoordinates getTextureCoordinates(VoxelEngine::World::VoxelType type) const;
    
    // Face-specific texture coordinate retrieval
    TextureCoordinates getTextureCoordinates(VoxelEngine::World::VoxelType type, AtlasType atlasType) const;
    
    // Face-based atlas selection
    AtlasType getAtlasForFace(VoxelEngine::World::VoxelType type, VoxelEngine::World::Face face) const;
    
    // Metadata-driven atlas loading
    bool loadFromMetadata(const std::string& metadataPath = "assets/textures/atlas_metadata.json");
    
    // Legacy compatibility methods (deprecated but maintained)
    bool loadTexture(const std::string& texturePath); // Loads main atlas (backward compatibility)
    bool loadMultiAtlas(const std::string& mainPath, const std::string& sidePath, const std::string& bottomPath);
    GLuint getTextureID() const; // Main atlas (backward compatibility)
    GLuint getTextureID(AtlasType atlasType) const;
    void setTextureID(GLuint id); // Main atlas (backward compatibility)
    void setTextureID(AtlasType atlasType, GLuint id);
    bool isTextureLoaded() const; // Main atlas (backward compatibility)
    bool isTextureLoaded(AtlasType atlasType) const;
    void releaseTexture(); // Clean up GPU resources
    
    // Atlas management
    void rebuildTextureMapping(); // Rebuild mapping for all 256 block types
    bool hasTexture(VoxelEngine::World::VoxelType type) const;
    
    // Metadata access
    const AtlasMetadata& getMetadata() const { return metadata_; }
    const AtlasTypeInfo& getAtlasInfo(AtlasType atlasType) const;

private:
    std::unordered_map<VoxelEngine::World::VoxelType, TextureCoordinates> m_voxel_texture_coords;
    
    // Metadata-driven atlas system
    AtlasMetadata metadata_;
    AtlasTypeInfo atlas_info_[3]; // [MAIN, SIDE, BOTTOM]
    
    // Legacy support
    GLuint m_texture_ids[3] = {0, 0, 0}; // [MAIN, SIDE, BOTTOM] atlas texture IDs (deprecated)
    
    // Metadata loading and parsing
    bool parseMetadataFile(const std::string& metadataPath);
    bool loadAtlasFiles();
    
    void initializeAllBlockTextures(); // Initialize UV coordinates for all 256 blocks
    TextureCoordinates calculateTextureCoordinates(uint8_t block_id) const;
    TextureCoordinates calculateMetadataBasedCoordinates(VoxelEngine::World::VoxelType type, AtlasType atlasType) const;
    
    // Helper functions for multi-atlas support
    bool loadSingleTexture(const std::string& texturePath, AtlasType atlasType);
    std::string getAtlasTypeName(AtlasType atlasType) const;
    
    // Atlas-specific coordinate calculation helpers (legacy)
    TextureCoordinates calculateSideAtlasCoordinates(VoxelEngine::World::VoxelType type) const;
    TextureCoordinates calculateBottomAtlasCoordinates(VoxelEngine::World::VoxelType type) const;
};

} // namespace Rendering
} // namespace VoxelEngine

#endif // VOXEL_ENGINE_TEXTURE_ATLAS_H
