#include "ui/core/UITheme.h"

namespace VoxelEngine {
namespace UI {

std::shared_ptr<UITheme> UITheme::defaultTheme_ = nullptr;

UITheme::UITheme() {
    // Colors are initialized with default values in the header
    // Typography, spacing, etc. are also initialized with defaults
}

std::shared_ptr<UITheme> UITheme::GetDefault() {
    if (!defaultTheme_) {
        defaultTheme_ = std::make_shared<UITheme>();
    }
    return defaultTheme_;
}

std::shared_ptr<UITheme> UITheme::CreateDarkTheme() {
    auto theme = std::make_shared<UITheme>();
    
    // Customize for dark theme (current colors are already dark)
    // This method allows for future light/dark theme variants
    
    return theme;
}

} // namespace UI
} // namespace VoxelEngine
