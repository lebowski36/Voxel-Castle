#include "rendering/DebugOverlay.h"
#include "rendering/FontManager.h"
#include <glm/gtc/matrix_transform.hpp>
#include <iomanip> // For std::fixed and std::setprecision
#include <sstream> // For std::ostringstream

namespace VoxelEngine {
namespace Rendering {

DebugOverlay::DebugOverlay(TextRenderer* textRenderer, FontManager* fontManager)
    : m_textRenderer(textRenderer), m_fontManager(fontManager), m_isVisible(false) {}

void DebugOverlay::toggleVisibility() {
    m_isVisible = !m_isVisible;
}

bool DebugOverlay::isVisible() const {
    return m_isVisible;
}

void DebugOverlay::render(const SpectatorCamera& camera, int screenWidth, int screenHeight) {
    if (!m_isVisible || !m_textRenderer || !m_fontManager || 
        !m_fontManager->isFontLoaded() || !m_textRenderer->isShaderReady()) {
        return;
    }

    // Orthographic projection for 2D text
    glm::mat4 projection = glm::ortho(0.0f, static_cast<float>(screenWidth), 0.0f, static_cast<float>(screenHeight));

    // Text properties
    float scale = 0.5f; // Adjust scale as needed for 2D text
    glm::vec3 textColor(1.0f, 1.0f, 1.0f); // White text
    float lineSpacing = m_fontManager->getFontSize() * scale * 1.2f; // Example line spacing
    float x_offset = 10.0f;
    float y_offset = static_cast<float>(screenHeight) - lineSpacing; // Start from top-left

    // Camera Coordinates Text
    std::ostringstream camPosStream;
    glm::vec3 camPos = camera.getPosition();
    camPosStream << std::fixed << std::setprecision(2)
                 << "Cam Pos: (" << camPos.x << ", " << camPos.y << ", " << camPos.z << ")";
    m_textRenderer->renderText2D(camPosStream.str(), x_offset, y_offset, scale, textColor, projection);
    y_offset -= lineSpacing;

    // Add more debug information here as needed, decrementing y_offset for each new line
    // Example: Camera LookAt (Direction)
    // std::ostringstream camDirStream;
    // glm::vec3 camDir = camera.getFront(); // Assuming getFront() gives the look direction
    // camDirStream << std::fixed << std::setprecision(2)
    //              << "Cam Dir: (" << camDir.x << ", " << camDir.y << ", " << camDir.z << ")";
    // m_textRenderer->renderText2D(camDirStream.str(), x_offset, y_offset, scale, textColor, projection);
    // y_offset -= lineSpacing;

}

} // namespace Rendering
} // namespace VoxelEngine
