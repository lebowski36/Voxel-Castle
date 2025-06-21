#include "ui/core/UIScaleManager.h"
#include <algorithm>
#include <iostream>

// Include platform-specific headers for DPI detection
#ifdef _WIN32
#include <windows.h>
#include <shellscalingapi.h>
#elif defined(__APPLE__)
#include <CoreGraphics/CoreGraphics.h>
#elif defined(__linux__)
#include <X11/Xlib.h>
#include <X11/Xatom.h>
#include <X11/Xresource.h>
#endif

namespace VoxelEngine {
namespace UI {

UIScaleManager::UIScaleManager()
    : dpiScale_(1.0f)
    , userScale_(DEFAULT_USER_SCALE)
    , screenWidth_(0)
    , screenHeight_(0) {
}

bool UIScaleManager::Initialize(int screenWidth, int screenHeight) {
    screenWidth_ = screenWidth;
    screenHeight_ = screenHeight;
    
    // Detect system DPI
    float systemDPI = DetectSystemDPI();
    dpiScale_ = systemDPI / DEFAULT_DPI;
    
    std::cout << "[UIScaleManager] Initialized with screen: " << screenWidth << "x" << screenHeight << std::endl;
    std::cout << "[UIScaleManager] Detected DPI: " << systemDPI << " (scale: " << dpiScale_ << ")" << std::endl;
    
    return true;
}

float UIScaleManager::DetectSystemDPI() const {
    float dpi = DEFAULT_DPI;
    
#ifdef _WIN32
    // Windows DPI detection
    HDC hdc = GetDC(nullptr);
    if (hdc) {
        dpi = static_cast<float>(GetDeviceCaps(hdc, LOGPIXELSX));
        ReleaseDC(nullptr, hdc);
    }
#elif defined(__APPLE__)
    // macOS DPI detection
    auto mainDisplay = CGMainDisplayID();
    auto mode = CGDisplayCopyDisplayMode(mainDisplay);
    if (mode) {
        size_t width = CGDisplayModeGetWidth(mode);
        size_t pixelWidth = CGDisplayModeGetPixelWidth(mode);
        
        if (width > 0) {
            float scaleFactor = static_cast<float>(pixelWidth) / static_cast<float>(width);
            dpi = DEFAULT_DPI * scaleFactor;
        }
        
        CGDisplayModeRelease(mode);
    }
#elif defined(__linux__)
    // Linux X11 DPI detection
    Display* display = XOpenDisplay(nullptr);
    if (display) {
        char* resourceString = XResourceManagerString(display);
        if (resourceString) {
            XrmDatabase db = XrmGetStringDatabase(resourceString);
            if (db) {
                XrmValue value;
                char* type;
                if (XrmGetResource(db, "Xft.dpi", "String", &type, &value)) {
                    if (value.addr) {
                        dpi = std::stof(value.addr);
                    }
                }
                XrmDestroyDatabase(db);
            }
        }
        
        // Fallback: calculate from screen size
        if (dpi == DEFAULT_DPI) {
            int screen = DefaultScreen(display);
            double xres = ((double)DisplayWidth(display, screen) * 25.4) / 
                         (double)DisplayWidthMM(display, screen);
            dpi = static_cast<float>(xres);
        }
        
        XCloseDisplay(display);
    }
#endif
    
    // Clamp to reasonable range
    dpi = std::max(72.0f, std::min(300.0f, dpi));
    
    return dpi;
}

void UIScaleManager::SetDPIScale(float scale) {
    dpiScale_ = std::max(0.5f, std::min(3.0f, scale));
    std::cout << "[UIScaleManager] DPI scale set to: " << dpiScale_ << std::endl;
}

void UIScaleManager::SetUserScale(float scale) {
    if (IsValidUserScale(scale)) {
        userScale_ = scale;
        std::cout << "[UIScaleManager] User scale set to: " << userScale_ << std::endl;
    }
}

bool UIScaleManager::IsValidUserScale(float scale) const {
    return scale >= MIN_USER_SCALE && scale <= MAX_USER_SCALE;
}

float UIScaleManager::LogicalToScreen(float logicalPixels) const {
    return logicalPixels * GetTotalScale();
}

glm::vec2 UIScaleManager::LogicalToScreen(const glm::vec2& logicalPixels) const {
    float scale = GetTotalScale();
    return glm::vec2(logicalPixels.x * scale, logicalPixels.y * scale);
}

float UIScaleManager::ScreenToLogical(float screenPixels) const {
    float totalScale = GetTotalScale();
    return totalScale > 0 ? screenPixels / totalScale : screenPixels;
}

glm::vec2 UIScaleManager::ScreenToLogical(const glm::vec2& screenPixels) const {
    float totalScale = GetTotalScale();
    if (totalScale > 0) {
        return glm::vec2(screenPixels.x / totalScale, screenPixels.y / totalScale);
    }
    return screenPixels;
}

void UIScaleManager::UpdateScreenSize(int width, int height) {
    screenWidth_ = width;
    screenHeight_ = height;
    
    // Re-detect DPI if screen changed significantly
    float newDPI = DetectSystemDPI();
    float newDPIScale = newDPI / DEFAULT_DPI;
    
    if (std::abs(newDPIScale - dpiScale_) > 0.1f) {
        dpiScale_ = newDPIScale;
        std::cout << "[UIScaleManager] DPI scale updated to: " << dpiScale_ << " due to screen change" << std::endl;
    }
}

} // namespace UI
} // namespace VoxelEngine
