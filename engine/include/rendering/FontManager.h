#ifndef FONTMANAGER_H
#define FONTMANAGER_H

#include <string>
#include <vector>
#include <map>
#include <glad/glad.h>
#include <glm/glm.hpp>

// Include stb_truetype.h directly
#include "stb_truetype.h"

namespace VoxelEngine {
namespace Rendering {

struct CharacterInfo {
    GLuint textureID;
    glm::ivec2 size;
    glm::ivec2 bearing;
    GLuint advance;
    glm::vec2 uv_x0, uv_y0; // Tex coords for quad: bottom-left
    glm::vec2 uv_x1, uv_y1; // Tex coords for quad: top-right
};

class FontManager {
public:
    FontManager();
    ~FontManager();

    bool loadFont(const std::string& fontPath, float fontSize);
    const CharacterInfo& getCharacter(char c) const;
    GLuint getAtlasTextureID() const { return atlasTextureID_; }
    int getAtlasWidth() const { return atlasWidth_; }
    int getAtlasHeight() const { return atlasHeight_; }
    float getFontSize() const; // Added declaration
    bool isFontLoaded() const { return fontInfo_ != nullptr && charData_ != nullptr && atlasTextureID_ != 0; } // Added

    // These might be useful for advanced users or debugging, ensure they return const pointers
    const stbtt_fontinfo* getFontInfo() const { return fontInfo_; }
    const stbtt_packedchar* getCharData() const { return charData_; }

private:
    bool generateAtlas(const std::vector<unsigned char>& fontBuffer);

    std::map<char, CharacterInfo> characterMap_;
    GLuint atlasTextureID_;
    int atlasWidth_;
    int atlasHeight_;
    float fontSize_;

    stbtt_fontinfo* fontInfo_;
    stbtt_packedchar* charData_; // Array for 96 chars (ASCII 32-127)

    CharacterInfo fallbackCharacter_;
};

} // namespace Rendering
} // namespace VoxelEngine

#endif // FONTMANAGER_H
