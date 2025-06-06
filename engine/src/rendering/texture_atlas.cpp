#include "rendering/texture_atlas.h"
#include "../../game/include/utils/debug_logger.h"
#include <filesystem>
#include <iostream>

// Include STB Image library for texture loading
#include "../../external/stb_image.h"

namespace VoxelEngine {
namespace Rendering {

// Define a consistent base directory for asset paths
const std::string BASE_DIRECTORY = "/home/system-x1/Projects/Voxel Castle/";

TextureAtlas::TextureAtlas() : m_texture_id(0) {
    initializeAllBlockTextures();
    
    // Automatically load the default texture atlas
    loadTexture("atlas.png");
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

bool TextureAtlas::loadTexture(const std::string& texturePath) {
    std::string resolvedPath;
    if (std::filesystem::path(texturePath).is_absolute()) {
        resolvedPath = texturePath; // Use the provided absolute path
    } else {
        resolvedPath = BASE_DIRECTORY + texturePath; // Prepend the base directory
    }

    DEBUG_LOG("TextureAtlas", "Loading texture from: " + resolvedPath);

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

    std::cout << "[INFO] Loading texture atlas..." << std::endl;
    DEBUG_LOG("TextureAtlas", "Texture loaded successfully. Dimensions: " + std::to_string(width) + "x" + std::to_string(height) + ", Channels: " + std::to_string(channels));

    GLenum format;
    if (channels == 1) { format = GL_RED; }
    else if (channels == 3) { format = GL_RGB; }
    else if (channels == 4) { format = GL_RGBA; }
    else {
        std::cerr << "[ERROR] Unknown number of channels: " << channels << " for texture " << resolvedPath << std::endl;
        stbi_image_free(data);
        return false;
    }

    // Clean up any existing texture
    if (m_texture_id != 0) {
        glDeleteTextures(1, &m_texture_id);
    }

    glGenTextures(1, &m_texture_id);
    
    if (m_texture_id == 0) {
        std::cerr << "[TextureAtlas::loadTexture] Error: glGenTextures failed to generate texture ID." << std::endl;
        stbi_image_free(data);
        return false;
    }
    
    glBindTexture(GL_TEXTURE_2D, m_texture_id);

    // Set texture filtering and wrapping parameters for pixel art / texture atlases
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

    glTexImage2D(GL_TEXTURE_2D, 0, format, width, height, 0, format, GL_UNSIGNED_BYTE, data);
    
    stbi_image_free(data);
    
    DEBUG_LOG("TextureAtlas", "Texture loaded with ID: " + std::to_string(m_texture_id));
    return true;
}

void TextureAtlas::releaseTexture() {
    if (m_texture_id != 0) {
        glDeleteTextures(1, &m_texture_id);
        m_texture_id = 0;
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

} // namespace Rendering
} // namespace VoxelEngine
