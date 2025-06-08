#include "rendering/texture_atlas.h"
#include "utils/debug_logger_stub.h"
#include "world/voxel_face_patterns.h"
#include <filesystem>
#include <iostream>
#include <algorithm>

// Include STB Image library for texture loading
#include "../../external/stb_image.h"

// Add JSON parsing support
#include <fstream>
#include <sstream>

namespace VoxelEngine {
namespace Rendering {

// Define a consistent base directory for asset paths
const std::string BASE_DIRECTORY = "/home/system-x1/Projects/Voxel Castle/";

TextureAtlas::TextureAtlas() {
    // Initialize all texture IDs to 0
    for (int i = 0; i < 3; ++i) {
        m_texture_ids[i] = 0;
    }
    
    // Try to load from metadata first, fallback to legacy system if needed
    if (!loadFromMetadata()) {
        std::cout << "[TextureAtlas] Metadata loading failed, falling back to legacy system" << std::endl;
        
        initializeAllBlockTextures();
        
        // Fallback to hardcoded atlas loading  
        loadMultiAtlas("assets/textures/atlas_main.png", 
                       "assets/textures/atlas_side.png", 
                       "assets/textures/atlas_bottom.png");
    }
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
            
        case FacePattern::ALL_FACES_DIFFERENT:
            // Same as ALL_DIFFERENT for compatibility
            switch (atlasType) {
                case AtlasType::MAIN:
                    return calculateTextureCoordinates(blockId);
                case AtlasType::SIDE:
                    return calculateSideAtlasCoordinates(type);
                case AtlasType::BOTTOM:
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
            
        case FacePattern::ALL_FACES_DIFFERENT:
            // Same as ALL_DIFFERENT for compatibility
            switch (face) {
                case Face::TOP:
                    return AtlasType::MAIN;
                case Face::BOTTOM:
                    return AtlasType::BOTTOM;
                case Face::FRONT:
                case Face::BACK:
                case Face::LEFT:
                case Face::RIGHT:
                    return AtlasType::SIDE;
            }
            break;
            
        case FacePattern::DIRECTIONAL:
            // Not implemented yet - use main atlas for now
            return AtlasType::MAIN;
    }
    
    // Default fallback
    return AtlasType::MAIN;
}

// Simple JSON parsing for atlas metadata (using manual parsing for now)
bool TextureAtlas::loadFromMetadata(const std::string& metadataPath) {
    std::string resolvedPath;
    if (std::filesystem::path(metadataPath).is_absolute()) {
        resolvedPath = metadataPath;
    } else {
        resolvedPath = BASE_DIRECTORY + metadataPath;
    }
    
    DEBUG_LOG("TextureAtlas", "Loading atlas metadata from: " + resolvedPath);
    
    if (!parseMetadataFile(resolvedPath)) {
        std::cerr << "[TextureAtlas] Failed to parse metadata file: " << resolvedPath << std::endl;
        return false;
    }
    
    if (!loadAtlasFiles()) {
        std::cerr << "[TextureAtlas] Failed to load atlas files from metadata" << std::endl;
        return false;
    }
    
    // Build texture coordinate mappings from metadata
    initializeAllBlockTextures();
    
    std::cout << "[TextureAtlas] Successfully loaded metadata-driven atlas system" << std::endl;
    std::cout << "[TextureAtlas] Tile size: " << metadata_.tile_size_px << "px" << std::endl;
    std::cout << "[TextureAtlas] Total blocks: " << metadata_.total_blocks << std::endl;
    
    return true;
}

bool TextureAtlas::parseMetadataFile(const std::string& metadataPath) {
    // For now, use the hardcoded values from the metadata we know exists
    // TODO: Implement proper JSON parsing (could use nlohmann/json or manual parsing)
    
    // Read the metadata file to extract key information
    std::ifstream file(metadataPath);
    if (!file.is_open()) {
        return false;
    }
    
    std::string content((std::istreambuf_iterator<char>(file)),
                        std::istreambuf_iterator<char>());
    file.close();
    
    // Basic parsing to extract tile_size_px (look for "tile_size_px": 32)
    size_t tile_size_pos = content.find("\"tile_size_px\":");
    if (tile_size_pos != std::string::npos) {
        size_t start = content.find_first_of("0123456789", tile_size_pos);
        size_t end = content.find_first_not_of("0123456789", start);
        if (start != std::string::npos && end != std::string::npos) {
            metadata_.tile_size_px = std::stoi(content.substr(start, end - start));
        }
    }
    
    // Extract file information for each atlas type
    const char* atlas_types[] = {"main", "side", "bottom"};
    
    for (int type_idx = 0; type_idx < 3; ++type_idx) {
        AtlasType atlasType = static_cast<AtlasType>(type_idx);
        AtlasTypeInfo& info = atlas_info_[type_idx];
        
        // Find the atlas type section
        std::string search_str = "\"" + std::string(atlas_types[type_idx]) + "\":";
        size_t atlas_section = content.find(search_str);
        if (atlas_section == std::string::npos) continue;
        
        // Look for filename in this section
        size_t filename_pos = content.find("\"filename\":", atlas_section);
        if (filename_pos != std::string::npos) {
            size_t quote_start = content.find("\"", filename_pos + 11);
            size_t quote_end = content.find("\"", quote_start + 1);
            if (quote_start != std::string::npos && quote_end != std::string::npos) {
                AtlasFileInfo fileInfo;
                fileInfo.filename = content.substr(quote_start + 1, quote_end - quote_start - 1);
                
                // Extract grid size
                size_t grid_size_pos = content.find("\"grid_size\":", filename_pos);
                if (grid_size_pos != std::string::npos && grid_size_pos < content.find("}", atlas_section)) {
                    // Look for [width, height] pattern
                    size_t bracket_start = content.find("[", grid_size_pos);
                    size_t comma_pos = content.find(",", bracket_start);
                    size_t bracket_end = content.find("]", comma_pos);
                    
                    if (bracket_start != std::string::npos && comma_pos != std::string::npos && bracket_end != std::string::npos) {
                        std::string width_str = content.substr(bracket_start + 1, comma_pos - bracket_start - 1);
                        std::string height_str = content.substr(comma_pos + 1, bracket_end - comma_pos - 1);
                        
                        // Remove whitespace
                        width_str.erase(std::remove_if(width_str.begin(), width_str.end(), ::isspace), width_str.end());
                        height_str.erase(std::remove_if(height_str.begin(), height_str.end(), ::isspace), height_str.end());
                        
                        fileInfo.grid_width = std::stoi(width_str);
                        fileInfo.grid_height = std::stoi(height_str);
                    }
                }
                
                info.files.push_back(fileInfo);
            }
        }
    }
    
    DEBUG_LOG("TextureAtlas", "Parsed metadata - tile size: " + std::to_string(metadata_.tile_size_px) + "px");
    
    return true;
}

bool TextureAtlas::loadAtlasFiles() {
    bool success = true;
    
    for (int type_idx = 0; type_idx < 3; ++type_idx) {
        AtlasType atlasType = static_cast<AtlasType>(type_idx);
        AtlasTypeInfo& info = atlas_info_[type_idx];
        
        // Load all files for this atlas type
        for (const auto& fileInfo : info.files) {
            std::string atlasPath = "assets/textures/" + fileInfo.filename;
            
            GLuint textureId = 0;
            if (loadSingleTexture(atlasPath, atlasType)) {
                textureId = m_texture_ids[type_idx]; // Get the loaded texture ID
                info.texture_ids.push_back(textureId);
                
                DEBUG_LOG("TextureAtlas", "Loaded " + getAtlasTypeName(atlasType) + " atlas: " + fileInfo.filename + 
                         " (grid: " + std::to_string(fileInfo.grid_width) + "x" + std::to_string(fileInfo.grid_height) + ")");
            } else {
                success = false;
                std::cerr << "[TextureAtlas] Failed to load atlas file: " << atlasPath << std::endl;
            }
        }
    }
    
    return success;
}

TextureCoordinates TextureAtlas::calculateMetadataBasedCoordinates(VoxelEngine::World::VoxelType type, AtlasType atlasType) const {
    int type_idx = static_cast<int>(atlasType);
    const AtlasTypeInfo& info = atlas_info_[type_idx];
    
    if (info.files.empty()) {
        // Fallback to legacy calculation
        return calculateTextureCoordinates(static_cast<uint8_t>(type));
    }
    
    // For now, use the first (and typically only) file
    const AtlasFileInfo& fileInfo = info.files[0];
    
    // Calculate tile position using metadata-driven grid size
    int block_id = static_cast<int>(type);
    int tile_idx_x = block_id % fileInfo.grid_width;
    int tile_idx_y = block_id / fileInfo.grid_width;
    
    // Calculate UV coordinates using actual tile size from metadata
    float tile_uv_width = static_cast<float>(metadata_.tile_size_px) / 
                         (fileInfo.grid_width * metadata_.tile_size_px);
    float tile_uv_height = static_cast<float>(metadata_.tile_size_px) / 
                          (fileInfo.grid_height * metadata_.tile_size_px);
    
    float u_min = static_cast<float>(tile_idx_x) * tile_uv_width;
    float u_max = static_cast<float>(tile_idx_x + 1) * tile_uv_width;
    
    // Convert from image space (top-down) to OpenGL UV space (bottom-up)
    float v_gl_min = 1.0f - (static_cast<float>(tile_idx_y + 1) * tile_uv_height);
    float v_gl_max = 1.0f - (static_cast<float>(tile_idx_y) * tile_uv_height);
    
    TextureCoordinates coords;
    coords.uv_min = glm::vec2(u_min, v_gl_min);
    coords.uv_max = glm::vec2(u_max, v_gl_max);
    
    return coords;
}

// Implement backward compatibility methods using metadata when available
GLuint TextureAtlas::getTextureID() const {
    return getTextureID(AtlasType::MAIN);
}

GLuint TextureAtlas::getTextureID(AtlasType atlasType) const {
    int type_idx = static_cast<int>(atlasType);
    const AtlasTypeInfo& info = atlas_info_[type_idx];
    
    if (!info.texture_ids.empty()) {
        return info.texture_ids[0]; // Return first texture ID for this type
    }
    
    // Fallback to legacy array
    return m_texture_ids[type_idx];
}

void TextureAtlas::setTextureID(GLuint id) {
    setTextureID(AtlasType::MAIN, id);
}

void TextureAtlas::setTextureID(AtlasType atlasType, GLuint id) {
    int type_idx = static_cast<int>(atlasType);
    m_texture_ids[type_idx] = id;
    
    // Also update metadata structure if it exists
    AtlasTypeInfo& info = atlas_info_[type_idx];
    if (info.texture_ids.empty()) {
        info.texture_ids.push_back(id);
    } else {
        info.texture_ids[0] = id;
    }
}

bool TextureAtlas::isTextureLoaded() const {
    return isTextureLoaded(AtlasType::MAIN);
}

bool TextureAtlas::isTextureLoaded(AtlasType atlasType) const {
    return getTextureID(atlasType) != 0;
}

const AtlasTypeInfo& TextureAtlas::getAtlasInfo(AtlasType atlasType) const {
    return atlas_info_[static_cast<int>(atlasType)];
}

} // namespace Rendering
} // namespace VoxelEngine
