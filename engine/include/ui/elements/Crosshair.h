#pragma once
#include "UITexture.h"

namespace VoxelEngine {
namespace UI {

/**
 * Crosshair class
 * 
 * Renders a simple crosshair/targeting reticle at the center of the screen
 * for precise aiming in block placement and interaction systems.
 * 
 * This implementation uses a simple white texture with alpha for crosshair lines.
 */
class Crosshair : public UITexture {
public:
    explicit Crosshair(UIRenderer* renderer);
    virtual ~Crosshair() = default;

    // Center the crosshair on the screen
    void centerOnScreen(int screenWidth, int screenHeight);

private:
    // Crosshair appearance settings
    static constexpr float CROSSHAIR_SIZE = 24.0f;  // Total size of crosshair area
    
    // Generate a simple white crosshair texture
    void generateCrosshairTexture();
    
    GLuint crosshairTextureID_;
};

} // namespace UI
} // namespace VoxelEngine
