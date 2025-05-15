#define STB_TRUETYPE_IMPLEMENTATION
#include "rendering/FontManager.h"
#include <fstream>
#include <vector>
#include <iostream>

namespace VoxelEngine {
namespace Rendering {

FontManager::FontManager() 
    : characterMap_(), // Initialize maps/vectors first if they don't depend on others
      atlasTextureID_(0), 
      atlasWidth_(1024), 
      atlasHeight_(1024), 
      fontSize_(32.0f), 
      fontInfo_(new stbtt_fontinfo()), 
      charData_(new stbtt_packedchar[128]) // ASCII 32-127 (96 chars) + 1 for potential null or extended
{
    // Initialize fallback character
    fallbackCharacter_.textureID = 0; 
    fallbackCharacter_.size = glm::ivec2(fontSize_ / 2, fontSize_);
    fallbackCharacter_.bearing = glm::ivec2(0, 0);
    fallbackCharacter_.advance = static_cast<GLuint>(fontSize_ / 2);
    fallbackCharacter_.uv_x0 = glm::vec2(0, 0);
    fallbackCharacter_.uv_y0 = glm::vec2(0, 0);
    fallbackCharacter_.uv_x1 = glm::vec2(0, 0);
    fallbackCharacter_.uv_y1 = glm::vec2(0, 0);
}

FontManager::~FontManager() {
    if (atlasTextureID_ != 0) {
        glDeleteTextures(1, &atlasTextureID_);
    }
    delete fontInfo_;
    delete[] charData_;
}

bool FontManager::loadFont(const std::string& fontPath, float fontSize) {
    fontSize_ = fontSize;
    std::ifstream fontFile(fontPath, std::ios::binary | std::ios::ate);
    if (!fontFile.is_open()) {
        std::cerr << "Failed to open font file: " << fontPath << std::endl;
        return false;
    }

    std::streamsize size = fontFile.tellg();
    fontFile.seekg(0, std::ios::beg);

    std::vector<unsigned char> fontBuffer(size);
    if (!fontFile.read(reinterpret_cast<char*>(fontBuffer.data()), size)) {
        std::cerr << "Failed to read font file: " << fontPath << std::endl;
        return false;
    }
    fontFile.close();

    characterMap_.clear();
    if (!generateAtlas(fontBuffer)) {
        return false;
    }

    std::cout << "Font loaded and atlas generated successfully: " << fontPath << std::endl;
    return true;
}

bool FontManager::generateAtlas(const std::vector<unsigned char>& fontBuffer) {
    if (fontBuffer.empty()) {
        std::cerr << "ERROR::FONTMANAGER: TTF buffer is empty." << std::endl;
        return false;
    }

    if (!stbtt_InitFont(fontInfo_, fontBuffer.data(), stbtt_GetFontOffsetForIndex(fontBuffer.data(), 0))) {
        std::cerr << "ERROR::FONTMANAGER: Failed to initialize font info." << std::endl;
        return false;
    }

    std::vector<unsigned char> atlasBitmap(atlasWidth_ * atlasHeight_, 0);
    stbtt_pack_context pc;
    if (!stbtt_PackBegin(&pc, atlasBitmap.data(), atlasWidth_, atlasHeight_, 0, 1, nullptr)) {
        std::cerr << "ERROR::FONTMANAGER: Failed to initialize stbtt_pack_context." << std::endl;
        return false;
    }

    stbtt_PackSetOversampling(&pc, 2, 2);

    if (!stbtt_PackFontRange(&pc, fontBuffer.data(), 0, fontSize_, 32, 96, charData_)) {
        std::cerr << "ERROR::FONTMANAGER: Failed to pack font range." << std::endl;
        stbtt_PackEnd(&pc);
        return false;
    }
    stbtt_PackEnd(&pc);

    if (atlasTextureID_ != 0) {
        glDeleteTextures(1, &atlasTextureID_);
    }
    glGenTextures(1, &atlasTextureID_);
    glBindTexture(GL_TEXTURE_2D, atlasTextureID_);
    glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
    glTexImage2D(
        GL_TEXTURE_2D, 0, GL_RED, atlasWidth_, atlasHeight_,
        0, GL_RED, GL_UNSIGNED_BYTE, atlasBitmap.data()
    );

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    characterMap_.clear();
    for (int i = 0; i < 96; ++i) {
        char c = static_cast<char>(32 + i);
        const stbtt_packedchar* pc = &charData_[i];

        CharacterInfo character;
        character.textureID = atlasTextureID_;
        character.size = glm::ivec2(pc->x1 - pc->x0, pc->y1 - pc->y0);
        character.bearing = glm::ivec2(pc->xoff, pc->yoff);
        character.advance = static_cast<GLuint>(pc->xadvance);

        character.uv_x0 = glm::vec2(static_cast<float>(pc->x0) / atlasWidth_, static_cast<float>(pc->y0) / atlasHeight_);
        character.uv_y0 = glm::vec2(static_cast<float>(pc->x0) / atlasWidth_, static_cast<float>(pc->y1) / atlasHeight_);
        character.uv_x1 = glm::vec2(static_cast<float>(pc->x1) / atlasWidth_, static_cast<float>(pc->y1) / atlasHeight_);
        character.uv_y1 = glm::vec2(static_cast<float>(pc->x1) / atlasWidth_, static_cast<float>(pc->y0) / atlasHeight_);

        characterMap_[c] = character;
    }

    if (characterMap_.count('?')) {
        fallbackCharacter_ = characterMap_['?'];
    } else if (!characterMap_.empty()) {
        fallbackCharacter_ = characterMap_.begin()->second;
    }

    glBindTexture(GL_TEXTURE_2D, 0);
    std::cout << "Font atlas generated." << std::endl;
    return true;
}

const CharacterInfo& FontManager::getCharacter(char c) const {
    auto it = characterMap_.find(c);
    if (it != characterMap_.end()) {
        return it->second;
    }
    return fallbackCharacter_;
}

float FontManager::getFontSize() const {
    return fontSize_;
}

} // namespace Rendering
} // namespace VoxelEngine
