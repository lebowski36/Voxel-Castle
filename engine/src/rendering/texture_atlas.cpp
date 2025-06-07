#include "rendering/texture_atlas.h"
#include "../../game/include/utils/debug_logger.h"
#include "world/voxel_face_patterns.h"
#include <filesystem>
#include <iostream>

// Include STB Image library for texture loading
#include "../../external/stb_image.h"

namespace VoxelEngine {
namespace Rendering {

// Define a consistent base directory for asset paths
const std::string BASE_DIRECTORY = "/home/system-x1/Projects/Voxel Castle/";

TextureAtlas::TextureAtlas() {
    // Initialize all texture IDs to 0
    for (int i = 0; i < 3; ++i) {
        m_texture_ids[i] = 0;
    }
    
    initializeAllBlockTextures();
    
    // Automatically load the multi-atlas system
    loadMultiAtlas("assets/textures/atlas_main_16x16.png", 
                   "assets/textures/atlas_side_3x3.png", 
                   "assets/textures/atlas_bottom_1x1.png");
}

void TextureAtlas::initializeAllBlockTextures() {
    // Initialize UV coordinates for all 256 possible block types
    for (int block_id = 0; block_id < MAX_BLOCK_TYPES; ++block_id) {
        VoxelEngine::World::VoxelType type = static_cast<VoxelEngine::World::VoxelType>(block_id);
        m_voxel_texture_coords[type] = calculateTextureCoordinates(static_cast<uint8_t>(block_id));
    }
    
    std::cout << "[TextureAtlas] Initialized UV coordinates for " << MAX_BLOCK_TYPES << " block types." << std::endl;
}

TextureCoordinates TextureAtlas::calculateTextureCoordinates(uint8_t block_id) const {
    // Calculate tile position in the 16x16 grid
    int tile_idx_x = block_id % TILES_PER_ROW;
    int tile_idx_y = block_id / TILES_PER_ROW;

    float u_min = static_cast<float>(tile_idx_x) * TILE_UV_WIDTH;
    float u_max = static_cast<float>(tile_idx_x + 1) * TILE_UV_WIDTH;

    // Convert from image space (top-down) to OpenGL UV space (bottom-up)
    float v_gl_min = 1.0f - (static_cast<float>(tile_idx_y + 1) * TILE_UV_HEIGHT);
    float v_gl_max = 1.0f - (static_cast<float>(tile_idx_y) * TILE_UV_HEIGHT);

    return {{u_min, v_gl_min}, {u_max, v_gl_max}};
}

TextureAtlas::~TextureAtlas() {
    releaseTexture();
}

TextureCoordinates TextureAtlas::getTextureCoordinates(VoxelEngine::World::VoxelType type) const {
    auto it = m_voxel_texture_coords.find(type);
    if (it != m_voxel_texture_coords.end()) {
        return it->second;
    }
    
    // Fallback to AIR tile if type not found
    return calculateTextureCoordinates(static_cast<uint8_t>(VoxelEngine::World::VoxelType::AIR));
}

// Face-specific texture coordinate retrieval
TextureCoordinates TextureAtlas::getTextureCoordinates(VoxelEngine::World::VoxelType type, AtlasType atlasType) const {
    using namespace VoxelEngine::World;
    
    FacePattern pattern = getFacePattern(type);
    uint8_t blockId = static_cast<uint8_t>(type);
    
    switch (pattern) {
        case FacePattern::UNIFORM:
            // All faces use the same texture from the main atlas
            return calculateTextureCoordinates(blockId);
            
        case FacePattern::TOP_BOTTOM_DIFFERENT:
            switch (atlasType) {
                case AtlasType::MAIN:
                    // Main atlas contains the top/bottom texture
                    return calculateTextureCoordinates(blockId);
                case AtlasType::SIDE:
                    // Side atlas contains side textures - use efficient packing
                    return calculateSideAtlasCoordinates(type);
                case AtlasType::BOTTOM:
                    // Bottom atlas not used for this pattern, fallback to main
                    return calculateTextureCoordinates(blockId);
            }
            break;
            
        case FacePattern::ALL_DIFFERENT:
            switch (atlasType) {
                case AtlasType::MAIN:
                    // Main atlas contains the top texture
                    return calculateTextureCoordinates(blockId);
                case AtlasType::SIDE:
                    // Side atlas contains side textures
                    return calculateSideAtlasCoordinates(type);
                case AtlasType::BOTTOM:
                    // Bottom atlas contains bottom textures
                    return calculateBottomAtlasCoordinates(type);
            }
            break;
            
        case FacePattern::DIRECTIONAL:
            // Not implemented yet - use main atlas for now
            return calculateTextureCoordinates(blockId);
    }
    
    // Fallback to AIR tile if type not found
    return calculateTextureCoordinates(static_cast<uint8_t>(VoxelEngine::World::VoxelType::AIR));
}

bool TextureAtlas::loadTexture(const std::string& texturePath) {
    return loadSingleTexture(texturePath, AtlasType::MAIN);
}

bool TextureAtlas::loadMultiAtlas(const std::string& mainPath, const std::string& sidePath, const std::string& bottomPath) {
    bool success = true;
    
    success &= loadSingleTexture(mainPath, AtlasType::MAIN);
    success &= loadSingleTexture(sidePath, AtlasType::SIDE);
    success &= loadSingleTexture(bottomPath, AtlasType::BOTTOM);
    
    if (success) {
        std::cout << "[TextureAtlas] Multi-atlas system loaded successfully!" << std::endl;
    } else {
        std::cerr << "[TextureAtlas] Failed to load one or more atlas files!" << std::endl;
    }
    
    return success;
}

bool TextureAtlas::loadSingleTexture(const std::string& texturePath, AtlasType atlasType) {
    std::string resolvedPath;
    if (std::filesystem::path(texturePath).is_absolute()) {
        resolvedPath = texturePath; // Use the provided absolute path
    } else {
        resolvedPath = BASE_DIRECTORY + texturePath; // Prepend the base directory
    }

    DEBUG_LOG("TextureAtlas", "Loading " + getAtlasTypeName(atlasType) + " atlas from: " + resolvedPath);

    // Check if file exists and is accessible
    if (!std::filesystem::exists(resolvedPath)) {
        std::cerr << "[ERROR] Texture file not found: " << resolvedPath << std::endl;
        return false;
    }

    int width, height, channels;
    stbi_set_flip_vertically_on_load(true);
    unsigned char* data = stbi_load(resolvedPath.c_str(), &width, &height, &channels, 0);
    if (!data) {
        std::cerr << "[ERROR] Error loading texture '" << resolvedPath << "': " << stbi_failure_reason() << std::endl;
        return false;
    }

    std::cout << "[INFO] Loading " << getAtlasTypeName(atlasType) << " atlas..." << std::endl;
    DEBUG_LOG("TextureAtlas", "Texture loaded successfully. Dimensions: " + std::to_string(width) + "x" + std::to_string(height) + ", Channels: " + std::to_string(channels));

    GLenum format;
    if (channels == 1) { format = GL_RED; }
    else if (channels == 3) { format = GL_RGB; }
    else if (channels == 4) { format = GL_RGBA; }
    else {
        std::cerr << "[ERROR] Unknown number of channels: " << channels << " for texture " << resolvedPath << std::endl;
        std::cerr << "[ERROR] Unknown number of channels: " << channels << " for texture " << resolvedPath << std::endl;
        stbi_image_free(data);
        return false;
    }

    int atlasIndex = static_cast<int>(atlasType);
    
    // Clean up any existing texture
    if (m_texture_ids[atlasIndex] != 0) {
        glDeleteTextures(1, &m_texture_ids[atlasIndex]);
    }

    glGenTextures(1, &m_texture_ids[atlasIndex]);
    
    if (m_texture_ids[atlasIndex] == 0) {
        std::cerr << "[TextureAtlas::loadSingleTexture] Error: glGenTextures failed to generate texture ID." << std::endl;
        stbi_image_free(data);
        return false;
    }
    
    glBindTexture(GL_TEXTURE_2D, m_texture_ids[atlasIndex]);

    // Set texture filtering and wrapping parameters for pixel art / texture atlases
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

    glTexImage2D(GL_TEXTURE_2D, 0, format, width, height, 0, format, GL_UNSIGNED_BYTE, data);
    
    stbi_image_free(data);
    
    DEBUG_LOG("TextureAtlas", getAtlasTypeName(atlasType) + " atlas loaded with ID: " + std::to_string(m_texture_ids[atlasIndex]));
    return true;
}

void TextureAtlas::releaseTexture() {
    for (int i = 0; i < 3; ++i) {
        if (m_texture_ids[i] != 0) {
            glDeleteTextures(1, &m_texture_ids[i]);
            m_texture_ids[i] = 0;
        }
    }
}

void TextureAtlas::rebuildTextureMapping() {
    // Clear existing mapping and rebuild for all 256 block types
    m_voxel_texture_coords.clear();
    initializeAllBlockTextures();
    std::cout << "[TextureAtlas] Rebuilt texture mapping for all block types." << std::endl;
}

bool TextureAtlas::hasTexture(VoxelEngine::World::VoxelType type) const {
    return m_voxel_texture_coords.find(type) != m_voxel_texture_coords.end();
}

std::string TextureAtlas::getAtlasTypeName(AtlasType atlasType) const {
    switch (atlasType) {
        case AtlasType::MAIN: return "main";
        case AtlasType::SIDE: return "side";
        case AtlasType::BOTTOM: return "bottom";
        default: return "unknown";
    }
}

TextureCoordinates TextureAtlas::calculateSideAtlasCoordinates(VoxelEngine::World::VoxelType type) const {
    // Map VoxelType to side atlas slot based on efficient packing allocation
    static const std::unordered_map<VoxelEngine::World::VoxelType, uint8_t> sideAtlasMapping = {
        {VoxelEngine::World::VoxelType::GRASS, 0},        // ALL_DIFFERENT pattern
        {VoxelEngine::World::VoxelType::WOOD_OAK, 1},     // TOP_BOTTOM_DIFFERENT pattern
        {VoxelEngine::World::VoxelType::WOOD_PINE, 2},    // TOP_BOTTOM_DIFFERENT pattern
        {VoxelEngine::World::VoxelType::WOOD_BIRCH, 3},   // TOP_BOTTOM_DIFFERENT pattern
        {VoxelEngine::World::VoxelType::WOOD_MAHOGANY, 4}, // TOP_BOTTOM_DIFFERENT pattern
        {VoxelEngine::World::VoxelType::OAK_BEAM, 5},     // TOP_BOTTOM_DIFFERENT pattern
        {VoxelEngine::World::VoxelType::PINE_BEAM, 6},    // TOP_BOTTOM_DIFFERENT pattern
        {VoxelEngine::World::VoxelType::HARDWOOD_BEAM, 7} // TOP_BOTTOM_DIFFERENT pattern
    };
    
    auto it = sideAtlasMapping.find(type);
    if (it != sideAtlasMapping.end()) {
        uint8_t slotId = it->second;
        
        // Side atlas is 3x3 grid (96x96 pixels, 32x32 per tile)
        int tile_idx_x = slotId % 3;
        int tile_idx_y = slotId / 3;
        
        float tile_uv_width = 32.0f / 96.0f;   // 1/3
        float tile_uv_height = 32.0f / 96.0f;  // 1/3
        
        float u_min = static_cast<float>(tile_idx_x) * tile_uv_width;
        float u_max = static_cast<float>(tile_idx_x + 1) * tile_uv_width;
        
        // Fix UV coordinate system: OpenGL UV (0,0) is bottom-left, but our atlas has slot 0 at visual top-left
        // Need to flip V coordinates: slot 0 (top-left visually) maps to OpenGL UV (0, 0.667) to (0.333, 1.0)
        float v_gl_min = 1.0f - static_cast<float>(tile_idx_y + 1) * tile_uv_height;
        float v_gl_max = 1.0f - static_cast<float>(tile_idx_y) * tile_uv_height;
        
        return {{u_min, v_gl_min}, {u_max, v_gl_max}};
    }
    
    // Fallback to AIR tile in main atlas if not found
    return calculateTextureCoordinates(static_cast<uint8_t>(VoxelEngine::World::VoxelType::AIR));
}

TextureCoordinates TextureAtlas::calculateBottomAtlasCoordinates(VoxelEngine::World::VoxelType type) const {
    // Map VoxelType to bottom atlas slot based on efficient packing allocation
    static const std::unordered_map<VoxelEngine::World::VoxelType, uint8_t> bottomAtlasMapping = {
        {VoxelEngine::World::VoxelType::GRASS, 0}  // ALL_DIFFERENT pattern
    };
    
    auto it = bottomAtlasMapping.find(type);
    if (it != bottomAtlasMapping.end()) {
        // Bottom atlas is 1x1 grid (32x32 pixels, single tile covers entire atlas)
        // For the single tile, UV coordinates are simply 0,0 to 1,1
        return {{0.0f, 0.0f}, {1.0f, 1.0f}};
    }
    
    // Fallback to AIR tile in main atlas if not found
    return calculateTextureCoordinates(static_cast<uint8_t>(VoxelEngine::World::VoxelType::AIR));
}

AtlasType TextureAtlas::getAtlasForFace(VoxelEngine::World::VoxelType type, VoxelEngine::World::Face face) const {
    using namespace VoxelEngine::World;
    
    FacePattern pattern = getFacePattern(type);
    
    switch (pattern) {
        case FacePattern::UNIFORM:
            // All faces use main atlas
            return AtlasType::MAIN;
            
        case FacePattern::TOP_BOTTOM_DIFFERENT:
            switch (face) {
                case Face::TOP:
                case Face::BOTTOM:
                    return AtlasType::MAIN;  // Top/bottom use main atlas
                case Face::FRONT:
                case Face::BACK:
                case Face::LEFT:
                case Face::RIGHT:
                    return AtlasType::SIDE;  // Sides use side atlas
            }
            break;
            
        case FacePattern::ALL_DIFFERENT:
            switch (face) {
                case Face::TOP:
                    return AtlasType::MAIN;    // Top uses main atlas
                case Face::BOTTOM:
                    return AtlasType::BOTTOM;  // Bottom uses bottom atlas
                case Face::FRONT:
                case Face::BACK:
                case Face::LEFT:
                case Face::RIGHT:
                    return AtlasType::SIDE;    // Sides use side atlas
            }
            break;
            
        case FacePattern::DIRECTIONAL:
            // Not implemented yet - use main atlas for now
            return AtlasType::MAIN;
    }
    
    // Default fallback
    return AtlasType::MAIN;
}

} // namespace Rendering
} // namespace VoxelEngine
