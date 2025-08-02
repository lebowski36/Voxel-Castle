#include "ui/FontManager.h"
#include "utils/debug_logger_stub.h"
#include <glad/glad.h>
#define STB_TRUETYPE_IMPLEMENTATION
#include <stb_truetype.h>
#include <fstream>
#include <vector>
#include <iostream>
#include <cstring>

namespace VoxelEngine {
namespace UI {

FontManager::FontManager() {}
FontManager::~FontManager() {
    if (atlasBitmap_) delete[] atlasBitmap_;
    if (atlasTexture_) glDeleteTextures(1, &atlasTexture_);
}

bool FontManager::loadFont(const std::string& fontPath, float pixelHeight) {
    fontHeight_ = pixelHeight;
    // Load font file
    std::ifstream file(fontPath, std::ios::binary | std::ios::ate);
    if (!file) {
        std::cerr << "[FontManager] Failed to open font: " << fontPath << std::endl;
        return false;
    }
    std::streamsize size = file.tellg();
    file.seekg(0, std::ios::beg);
    std::vector<unsigned char> fontBuffer(size);
    if (!file.read((char*)fontBuffer.data(), size)) {
        std::cerr << "[FontManager] Failed to read font: " << fontPath << std::endl;
        return false;
    }
    // Allocate atlas bitmap
    atlasBitmap_ = new unsigned char[atlasWidth_ * atlasHeight_];
    std::memset(atlasBitmap_, 0, atlasWidth_ * atlasHeight_);
    // Bake font
    stbtt_bakedchar bakedChars[96];
    int res = stbtt_BakeFontBitmap(fontBuffer.data(), 0, pixelHeight, atlasBitmap_, atlasWidth_, atlasHeight_, 32, 96, bakedChars);
    if (res <= 0) {
        std::cerr << "[FontManager] stbtt_BakeFontBitmap failed" << std::endl;
        return false;
    }
    // Create OpenGL texture
    glGenTextures(1, &atlasTexture_);
    glBindTexture(GL_TEXTURE_2D, atlasTexture_);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RED, atlasWidth_, atlasHeight_, 0, GL_RED, GL_UNSIGNED_BYTE, atlasBitmap_);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    // Store glyphs
    for (int i = 0; i < 96; ++i) {
        char c = (char)(i + 32);
        stbtt_bakedchar& bc = bakedChars[i];
        GlyphInfo info;
        info.uv0 = glm::vec2(bc.x0 / (float)atlasWidth_, bc.y0 / (float)atlasHeight_);
        info.uv1 = glm::vec2(bc.x1 / (float)atlasWidth_, bc.y1 / (float)atlasHeight_);
        info.size = glm::vec2(bc.x1 - bc.x0, bc.y1 - bc.y0);
        info.offset = glm::vec2(bc.xoff, bc.yoff);
        info.advance = bc.xadvance;
        glyphs_[c] = info;
    }
    DEBUG_LOG("FontManager", "Loaded font: " + std::string(fontPath) + " height: " + std::to_string(pixelHeight));
    return true;
}

const GlyphInfo* FontManager::getGlyph(char c) const {
    auto it = glyphs_.find(c);
    if (it != glyphs_.end()) return &it->second;
    return nullptr;
}

bool FontManager::isTextureValid() const {
    if (atlasTexture_ == 0) return false;
    
    GLboolean isTexture = glIsTexture(atlasTexture_);
    if (!isTexture) {
        std::cout << "[FontManager] WARNING: Atlas texture " << atlasTexture_ << " is not a valid OpenGL texture!" << std::endl;
        return false;
    }
    
    // Check if the texture has valid parameters
    glBindTexture(GL_TEXTURE_2D, atlasTexture_);
    GLint width, height;
    glGetTexLevelParameteriv(GL_TEXTURE_2D, 0, GL_TEXTURE_WIDTH, &width);
    glGetTexLevelParameteriv(GL_TEXTURE_2D, 0, GL_TEXTURE_HEIGHT, &height);
    
    if (width != atlasWidth_ || height != atlasHeight_) {
        std::cout << "[FontManager] WARNING: Atlas texture dimensions mismatch! Expected: " 
                  << atlasWidth_ << "x" << atlasHeight_ << ", Got: " << width << "x" << height << std::endl;
        return false;
    }
    
    return true;
}

} // namespace UI
} // namespace VoxelEngine
