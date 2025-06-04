#include "rendering/texture_atlas.h"
#include "../../game/include/utils/debug_logger.h"
#include <filesystem>
#include <iostream>

// Include STB Image library for texture loading
#define STB_IMAGE_IMPLEMENTATION
#include "../../external/stb_image.h"

namespace VoxelEngine {
namespace Rendering {

// Define the constants declared in the header
const float TILE_UV_WIDTH = TILE_WIDTH_PX / ATLAS_WIDTH_PX;
const float TILE_UV_HEIGHT = TILE_HEIGHT_PX / ATLAS_HEIGHT_PX;

// Define a consistent base directory for asset paths
const std::string BASE_DIRECTORY = "/home/system-x1/Projects/Voxel Castle/";

TextureAtlas::TextureAtlas() : m_texture_id(0) {
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
    
    // Automatically load the default texture atlas
    loadTexture("atlas.png");
}

TextureAtlas::~TextureAtlas() {
    // Clean up the texture if it exists
    if (m_texture_id != 0) {
        glDeleteTextures(1, &m_texture_id);
        m_texture_id = 0;
    }
}

TextureCoordinates TextureAtlas::getTextureCoordinates(VoxelEngine::World::VoxelType type) const {
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

bool TextureAtlas::loadTexture(const std::string& path) {
    std::string resolvedPath;
    if (std::filesystem::path(path).is_absolute()) {
        resolvedPath = path; // Use the provided absolute path
    } else {
        resolvedPath = BASE_DIRECTORY + "assets/textures/" + path; // Prepend the base directory
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

} // namespace Rendering
} // namespace VoxelEngine
