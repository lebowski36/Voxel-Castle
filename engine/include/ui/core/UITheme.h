#pragma once

#include <glm/glm.hpp>
#include <string>
#include <memory>

namespace VoxelEngine {
namespace UI {

/**
 * @brief Color definitions for the UI theme system
 */
struct UIColors {
    // Background colors
    glm::vec4 backgroundPrimary{0.043f, 0.055f, 0.102f, 1.0f};    // #0B0E1A
    glm::vec4 backgroundSecondary{0.082f, 0.106f, 0.180f, 1.0f};  // #151B2E
    glm::vec4 backgroundAccent{0.118f, 0.165f, 0.278f, 1.0f};     // #1E2A47
    
    // Surface colors
    glm::vec4 surfacePrimary{0.145f, 0.184f, 0.290f, 1.0f};       // #252F4A
    glm::vec4 surfaceSecondary{0.165f, 0.212f, 0.314f, 1.0f};     // #2A3650
    glm::vec4 surfaceAccent{0.204f, 0.267f, 0.400f, 1.0f};        // #344466
    
    // Accent colors
    glm::vec4 primaryAccent{0.290f, 0.620f, 1.0f, 1.0f};          // #4A9EFF
    glm::vec4 secondaryAccent{0.420f, 0.369f, 1.0f, 1.0f};        // #6B5EFF
    glm::vec4 success{0.290f, 1.0f, 0.533f, 1.0f};                // #4AFF88
    glm::vec4 warning{1.0f, 0.722f, 0.290f, 1.0f};                // #FFB84A
    glm::vec4 error{1.0f, 0.290f, 0.420f, 1.0f};                  // #FF4A6B
    
    // Text colors
    glm::vec4 textPrimary{1.0f, 1.0f, 1.0f, 1.0f};                // #FFFFFF
    glm::vec4 textSecondary{0.722f, 0.773f, 0.910f, 1.0f};        // #B8C5E8
    glm::vec4 textMuted{0.478f, 0.541f, 0.659f, 1.0f};            // #7A8AA8
    glm::vec4 textDisabled{0.290f, 0.333f, 0.408f, 1.0f};         // #4A5568
    
    // Interactive states
    glm::vec4 hover{0.0f, 0.0f, 0.0f, 0.1f};     // Overlay for hover
    glm::vec4 pressed{0.0f, 0.0f, 0.0f, 0.2f};   // Overlay for pressed
    glm::vec4 focus{0.290f, 0.620f, 1.0f, 0.3f}; // Focus outline
};

/**
 * @brief Typography settings for the UI theme
 */
struct UITypography {
    // Font sizes (in logical pixels)
    float headingLarge = 48.0f;     // H1 - Titles
    float headingMedium = 32.0f;    // H2 - Subtitles
    float headingSmall = 24.0f;     // H3 - Sections
    float bodyLarge = 18.0f;        // Large body text
    float bodyMedium = 16.0f;       // Standard body text
    float bodySmall = 14.0f;        // Small body text
    float caption = 12.0f;          // Captions and hints
    float button = 16.0f;           // Button text
    
    // Font weights
    int weightLight = 300;
    int weightRegular = 400;
    int weightMedium = 500;
    int weightSemiBold = 600;
    int weightBold = 700;
    
    // Letter spacing (em units)
    float spacingTight = -0.02f;
    float spacingNormal = 0.0f;
    float spacingWide = 0.02f;
};

/**
 * @brief Spacing system based on 8-point grid
 */
struct UISpacing {
    float xs = 4.0f;    // Micro spacing
    float sm = 8.0f;    // Small spacing
    float md = 16.0f;   // Standard spacing
    float lg = 24.0f;   // Large spacing
    float xl = 32.0f;   // Extra large spacing
    float xxl = 48.0f;  // Section spacing
    float xxxl = 64.0f; // Major spacing
};

/**
 * @brief Border radius values for consistent rounded corners
 */
struct UIBorderRadius {
    float none = 0.0f;
    float small = 4.0f;
    float medium = 8.0f;
    float large = 12.0f;
    float xlarge = 16.0f;
    float round = 50.0f; // For circular elements
};

/**
 * @brief Shadow definitions for depth and elevation
 */
struct UIShadows {
    struct Shadow {
        glm::vec2 offset;
        float blur;
        float spread;
        glm::vec4 color;
    };
    
    Shadow none{glm::vec2(0), 0, 0, glm::vec4(0)};
    Shadow small{glm::vec2(0, 2), 4, 0, glm::vec4(0, 0, 0, 0.1f)};
    Shadow medium{glm::vec2(0, 4), 8, 0, glm::vec4(0, 0, 0, 0.15f)};
    Shadow large{glm::vec2(0, 8), 16, 0, glm::vec4(0, 0, 0, 0.2f)};
    Shadow xlarge{glm::vec2(0, 16), 32, 0, glm::vec4(0, 0, 0, 0.25f)};
};

/**
 * @brief Responsive breakpoints for adaptive layouts
 */
struct UIBreakpoints {
    float small = 1366.0f;      // Small desktop (< 1366px)
    float medium = 1920.0f;     // Standard desktop (1366-1920px)
    float large = 2560.0f;      // Large desktop (1920-2560px)
    float xlarge = 3840.0f;     // Ultra-wide/4K (> 2560px)
    
    // Get responsive spacing multiplier based on screen width
    float GetSpacingMultiplier(float screenWidth) const {
        if (screenWidth < small) return 0.75f;        // Compact spacing for small screens
        else if (screenWidth < medium) return 1.0f;   // Normal spacing
        else if (screenWidth < large) return 1.25f;   // Generous spacing for large screens
        else return 1.5f;                             // Extra generous for ultra-wide
    }
    
    // Get responsive font scale based on screen width
    float GetFontScale(float screenWidth) const {
        if (screenWidth < small) return 0.9f;         // Smaller fonts for small screens
        else if (screenWidth < medium) return 1.0f;   // Normal fonts
        else if (screenWidth < large) return 1.1f;    // Slightly larger for large screens
        else return 1.2f;                             // Larger fonts for ultra-wide
    }
    
    // Determine screen size category
    enum class ScreenCategory { SMALL, MEDIUM, LARGE, XLARGE };
    ScreenCategory GetScreenCategory(float screenWidth) const {
        if (screenWidth < small) return ScreenCategory::SMALL;
        else if (screenWidth < medium) return ScreenCategory::MEDIUM;
        else if (screenWidth < large) return ScreenCategory::LARGE;
        else return ScreenCategory::XLARGE;
    }
};

/**
 * @brief Main theme class containing all styling information
 */
class UITheme {
public:
    UITheme();
    ~UITheme() = default;
    
    // Theme components
    UIColors colors;
    UITypography typography;
    UISpacing spacing;
    UIBorderRadius borderRadius;
    UIShadows shadows;
    UIBreakpoints breakpoints;
    
    // Animation timing
    float animationFast = 0.15f;      // 150ms
    float animationMedium = 0.25f;    // 250ms
    float animationSlow = 0.35f;      // 350ms
    
    // Component sizing
    float buttonHeightSmall = 36.0f;
    float buttonHeightMedium = 44.0f;
    float buttonHeightLarge = 52.0f;
    float inputHeight = 48.0f;
    float iconSizeSmall = 16.0f;
    float iconSizeMedium = 24.0f;
    float iconSizeLarge = 32.0f;
    
    // Static theme access
    static std::shared_ptr<UITheme> GetDefault();
    static std::shared_ptr<UITheme> CreateDarkTheme();
    
    // Responsive design helpers
    float GetResponsiveSpacing(float baseSpacing, float screenWidth) const {
        return baseSpacing * breakpoints.GetSpacingMultiplier(screenWidth);
    }
    
    float GetResponsiveFontSize(float baseFontSize, float screenWidth) const {
        return baseFontSize * breakpoints.GetFontScale(screenWidth);
    }
    
    UIBreakpoints::ScreenCategory GetScreenCategory(float screenWidth) const {
        return breakpoints.GetScreenCategory(screenWidth);
    }
    
private:
    static std::shared_ptr<UITheme> defaultTheme_;
};

} // namespace UI
} // namespace VoxelEngine
