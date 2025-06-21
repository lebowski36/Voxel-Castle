#pragma once

#include <glm/glm.hpp>

namespace VoxelEngine {
namespace UI {

/**
 * @brief Manages DPI detection and UI scaling
 * 
 * Handles automatic DPI detection and user scaling preferences
 * to ensure UI looks consistent across all displays and sizes.
 */
class UIScaleManager {
public:
    UIScaleManager();
    ~UIScaleManager() = default;

    // Initialize with screen information
    bool Initialize(int screenWidth, int screenHeight);
    
    // DPI detection and scaling
    float DetectSystemDPI() const;
    float GetDPIScale() const { return dpiScale_; }
    void SetDPIScale(float scale);
    
    // User scaling preference (50% to 200%)
    float GetUserScale() const { return userScale_; }
    void SetUserScale(float scale);
    bool IsValidUserScale(float scale) const;
    
    // Combined scaling
    float GetTotalScale() const { return dpiScale_ * userScale_; }
    
    // Scaling utilities
    float LogicalToScreen(float logicalPixels) const;
    glm::vec2 LogicalToScreen(const glm::vec2& logicalPixels) const;
    float ScreenToLogical(float screenPixels) const;
    glm::vec2 ScreenToLogical(const glm::vec2& screenPixels) const;
    
    // Screen size update
    void UpdateScreenSize(int width, int height);
    
    // Constants
    static constexpr float MIN_USER_SCALE = 0.5f;
    static constexpr float MAX_USER_SCALE = 2.0f;
    static constexpr float DEFAULT_USER_SCALE = 1.0f;
    static constexpr float DEFAULT_DPI = 96.0f;
    
private:
    float dpiScale_;
    float userScale_;
    int screenWidth_;
    int screenHeight_;
};

} // namespace UI
} // namespace VoxelEngine
