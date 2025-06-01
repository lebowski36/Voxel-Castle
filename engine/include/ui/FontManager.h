#pragma once
#include <string>
#include <unordered_map>
#include <glm/glm.hpp>
#include <glad/glad.h>

namespace VoxelEngine {
namespace UI {

struct GlyphInfo {
    glm::vec2 uv0;
    glm::vec2 uv1;
    glm::vec2 size;
    glm::vec2 offset;
    float advance;
};

class FontManager {
public:
    FontManager();
    ~FontManager();

    // Loads a TTF font and builds a glyph atlas
    bool loadFont(const std::string& fontPath, float pixelHeight = 32.0f);

    // Returns OpenGL texture ID for the atlas
    GLuint getAtlasTexture() const { return atlasTexture_; }

    // Returns glyph info for a character
    const GlyphInfo* getGlyph(char c) const;

    float getFontHeight() const { return fontHeight_; }

private:
    GLuint atlasTexture_ = 0;
    float fontHeight_ = 32.0f;
    std::unordered_map<char, GlyphInfo> glyphs_;
    unsigned char* atlasBitmap_ = nullptr;
    int atlasWidth_ = 512;
    int atlasHeight_ = 64;
};

} // namespace UI
} // namespace VoxelEngine
