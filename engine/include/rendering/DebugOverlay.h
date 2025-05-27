#ifndef DEBUG_OVERLAY_H
#define DEBUG_OVERLAY_H

#include <string>
#include <vector>
#include "rendering/TextRenderer.h" 
#include "../../game/include/SpectatorCamera.h"    

// Forward declare FontManager if TextRenderer.h doesn't pull it in directly for this header's needs
// Or ensure TextRenderer.h includes what's necessary (like FontManager.h for getFontSize)
// For now, assuming TextRenderer might provide enough, or we'll adjust if FontManager is directly needed here.
namespace VoxelEngine {
namespace Rendering {

class FontManager; // Forward declaration

class DebugOverlay {
public:
    DebugOverlay(TextRenderer* textRenderer, FontManager* fontManager);

    void render(const SpectatorCamera& camera, int screenWidth, int screenHeight);
    void toggleVisibility(); 
    bool isVisible() const;  

private:
    TextRenderer* m_textRenderer;
    FontManager* m_fontManager; // Store FontManager for font metrics
    bool m_isVisible;
};

} // namespace Rendering
} // namespace VoxelEngine

#endif // DEBUG_OVERLAY_H
