# UI System Developer Guide

*Version: 2.0 (Complete Rebuild)*  
*Last Updated: 2025-06-21*  
*Status: 🚨 UNDER CONSTRUCTION - REBUILDING ENTIRE SYSTEM*

## 🎯 **Overview**

This guide explains how to create menus and UI components using the new Voxel Castle UI system. The system has been completely rebuilt to address fundamental architectural problems and provide a modern, responsive, component-based UI framework.

## 🚨 **IMPORTANT: System Status**

**The UI system is currently being completely rebuilt.** This document outlines the planned architecture and will be updated as implementation progresses.

**Current Status**: Rebuilding core framework  
**ETA**: Available for use after Phase 1-2 completion  
**Migration**: Legacy UI system will be phased out gradually  

## 🏗️ **Architecture Overview**

The new UI system follows modern component-based architecture principles:

### **Core Concepts:**
- **Components**: Reusable UI building blocks (Button, TextInput, Panel, etc.)
- **Layout Managers**: Automatic positioning and sizing (Flex, Grid, Stack)
- **Responsive Design**: Adapts to screen size changes automatically
- **Event System**: Clean input handling with proper event propagation
- **Theme System**: Consistent styling and appearance

### **Key Advantages Over Legacy System:**
- ✅ No more off-screen positioning bugs
- ✅ Automatic responsive design for all screen sizes
- ✅ **Multi-DPI scaling support with automatic screen density detection**
- ✅ **User-configurable UI scaling (50% to 200%) for accessibility**
- ✅ **Resolution-independent design using logical pixels**
- ✅ Reusable components reduce code duplication
- ✅ Clean separation of layout logic and rendering
- ✅ Proper input handling and focus management
- ✅ Easy to create new menus with minimal boilerplate
- ✅ **Multi-DPI scaling support with automatic screen density detection**
- ✅ **User-configurable UI scaling for accessibility and preference**

## 🧩 **Component System**

### **Available Components** (Planned)
```cpp
// Basic Components
Button          // Clickable button with hover states
TextInput       // Text input field with validation
Label           // Text display
Panel           // Container component
Slider          // Range input control
Dropdown        // Selection dropdown
Checkbox        // Boolean input
ProgressBar     // Progress indication

// Layout Components  
FlexLayout      // Flexbox-style responsive layout
GridLayout      // Grid-based layout
StackLayout     // Vertical/horizontal stacking
```

## 📐 **Layout System**

### **FlexLayout** (Primary Layout Manager)
```cpp
auto layout = std::make_shared<FlexLayout>(FlexDirection::Column);
layout->SetJustifyContent(JustifyContent::Center);
layout->SetAlignItems(AlignItems::Center);
layout->SetGap(20); // 20px spacing between elements
```

### **Responsive Sizing**
```cpp
// Percentage-based sizing
button->SetWidth("50%");        // 50% of parent width
button->SetHeight("auto");      // Auto-size based on content

// Fixed sizing with constraints
button->SetMinWidth(200);       // Minimum 200px wide
button->SetMaxWidth(400);       // Maximum 400px wide

// DPI-aware sizing (automatically scales with UI scale factor)
button->SetLogicalWidth(200);   // 200 logical pixels (scales with DPI)
button->SetPhysicalWidth(200);  // 200 physical pixels (no scaling)
```

## 🔍 **UI Scaling & DPI Support**

### **Automatic DPI Detection**
```cpp
// System automatically detects and applies appropriate scaling
UISystem::Initialize();  // Detects DPI and sets scale factor

// Get current scale information
float scaleFactor = UISystem::GetScaleFactor();  // e.g., 1.5 for 150% scaling
float dpi = UISystem::GetScreenDPI();            // e.g., 144 DPI
```

### **Scale-Aware Component Creation**
```cpp
// All measurements automatically scale with UI scale factor
auto button = CreateComponent<Button>("Scaled Button");

// Logical units (recommended) - automatically scaled
button->SetWidth(LogicalPixels(200));    // Scales to 300px at 150% scale
button->SetHeight(LogicalPixels(50));    // Scales to 75px at 150% scale

// Physical units (use sparingly) - no scaling applied
button->SetWidth(PhysicalPixels(200));   // Always exactly 200px

// Responsive + scalable (best practice)
button->SetWidth("80%");                 // 80% of parent, then scaled by DPI
button->SetMinWidth(LogicalPixels(100)); // Minimum 100 logical pixels
```

### **Multi-Resolution Assets**
```cpp
// System automatically selects appropriate asset resolution
auto icon = LoadScalableIcon("menu/settings");
// Loads: settings_1x.png, settings_2x.png, or settings_3x.png
// Based on current UI scale factor

// Manual asset resolution selection
auto icon = LoadIcon("menu/settings", AssetScale::Scale2x);
```

## 📝 **Creating a New Menu** (Template)

```cpp
#pragma once
#include "ui/core/UIComponent.h"
#include "ui/components/Button.h"
#include "ui/components/Label.h"
#include "ui/layout/FlexLayout.h"

class ExampleMenu : public UIComponent {
public:
    ExampleMenu();
    ~ExampleMenu() override = default;
    
    void Initialize() override;
    
private:
    void OnButtonClick();
    void OnBackClick();
    
    std::shared_ptr<Button> actionButton_;
    std::shared_ptr<Button> backButton_;
    std::shared_ptr<Label> titleLabel_;
};

// Implementation
ExampleMenu::ExampleMenu() {
    // Set up responsive layout
    auto layout = std::make_shared<FlexLayout>(FlexDirection::Column);
    layout->SetJustifyContent(JustifyContent::Center);
    layout->SetAlignItems(AlignItems::Center);
    layout->SetGap(30);
    SetLayout(layout);
}

void ExampleMenu::Initialize() {
    // Create title
    titleLabel_ = CreateComponent<Label>("Menu Title");
    titleLabel_->SetStyle({
        .fontSize = 32,
        .color = Colors::White,
        .fontWeight = FontWeight::Bold
    });
    AddChild(titleLabel_);
    
    // Create action button
    actionButton_ = CreateComponent<Button>("Action Button");
    actionButton_->SetOnClick([this]() { OnButtonClick(); });
    actionButton_->SetStyle({
        .minWidth = 200,
        .height = 50,
        .backgroundColor = Colors::Primary,
        .textColor = Colors::White
    });
    AddChild(actionButton_);
    
    // Create back button
    backButton_ = CreateComponent<Button>("Back");
    backButton_->SetOnClick([this]() { OnBackClick(); });
    AddChild(backButton_);
}
```

## 🎨 **Styling System**

### **Component Styles**
```cpp
ComponentStyle style = {
    .width = "80%",              // Responsive width
    .height = LogicalPixels(60), // DPI-aware fixed height
    .backgroundColor = {0.2f, 0.2f, 0.3f, 1.0f},
    .borderColor = {0.5f, 0.5f, 0.6f, 1.0f},
    .borderWidth = LogicalPixels(2),      // DPI-aware border
    .borderRadius = LogicalPixels(8),     // DPI-aware radius
    .padding = {
        LogicalPixels(10),  // top
        LogicalPixels(15),  // right  
        LogicalPixels(10),  // bottom
        LogicalPixels(15)   // left
    },
    .margin = {LogicalPixels(5), 0, LogicalPixels(5), 0},
    .fontSize = LogicalPixels(16),        // DPI-aware font size
    .textColor = {1.0f, 1.0f, 1.0f, 1.0f}
};
component->SetStyle(style);

// Alternative: Use relative font sizes for better accessibility
style.fontSize = RelativeFontSize::Medium;  // Scales with user font preferences
```

### **Theme System**
```cpp
// Apply consistent theme across all components
auto theme = UITheme::GetDefault();
theme->primaryColor = {0.3f, 0.5f, 0.8f, 1.0f};
theme->backgroundColor = {0.1f, 0.1f, 0.15f, 1.0f};

// DPI-aware theme scaling
theme->baseFontSize = LogicalPixels(16);
theme->buttonHeight = LogicalPixels(48);
theme->elementSpacing = LogicalPixels(12);

UISystem::SetTheme(theme);
```

## 🎯 **Event Handling**

### **Button Events**
```cpp
button->SetOnClick([]() { 
    std::cout << "Button clicked!" << std::endl; 
});

button->SetOnHover([](bool isHovering) {
    if (isHovering) {
        button->SetBackgroundColor(Colors::ButtonHover);
    } else {
        button->SetBackgroundColor(Colors::Button);
    }
});
```

### **Input Events**
```cpp
textInput->SetOnTextChanged([](const std::string& newText) {
    // Validate input as user types
    if (newText.length() > 50) {
        textInput->SetBorderColor(Colors::Error);
    } else {
        textInput->SetBorderColor(Colors::Normal);
    }
});

textInput->SetOnEnter([this]() {
    // Handle enter key press
    SubmitForm();
});
```

## 📱 **Responsive Design & UI Scaling**

### **Multi-Level Scaling System**
The UI system supports comprehensive scaling at multiple levels:

1. **DPI Scaling**: Automatic detection and scaling for high-DPI displays
2. **User Scaling**: Manual UI scale preference (50% - 200%)
3. **Responsive Layout**: Dynamic layout changes based on screen size
4. **Component Scaling**: Individual component size adaptation

### **DPI & Resolution Support**
```cpp
// Automatic DPI detection and scaling
UISystem::Initialize() {
    float dpiScale = GetSystemDPIScale(); // 1.0, 1.25, 1.5, 2.0, etc.
    UISystem::SetDPIScale(dpiScale);
    
    // All UI measurements automatically scaled
    button->SetHeight(48); // Physical: 48px * dpiScale
}

// Support for common resolutions
// 1366x768  (HD)     - Compact layout, smaller components
// 1920x1080 (FHD)    - Standard layout
// 2560x1440 (QHD)    - Expanded layout
// 3840x2160 (4K)     - Large layout with high DPI scaling
```

### **User UI Scale Preference**
```cpp
// User can adjust UI scale from 50% to 200%
UISystem::SetUserScale(1.25f); // 125% scaling

// Affects all UI elements proportionally
auto button = CreateComponent<Button>("Large Button");
button->SetHeight(48); // Actually rendered at 60px (48 * 1.25)
```

### **Responsive Breakpoints**
```cpp
enum class ScreenSize {
    SMALL,    // < 1366px width  - Mobile/small laptop
    MEDIUM,   // 1366-1920px     - Standard desktop
    LARGE,    // 1920-2560px     - Large desktop
    XLARGE    // > 2560px        - Ultra-wide/4K
};

// Automatic layout adaptation
void ExampleMenu::OnScreenSizeChanged(int width, int height) {
    auto screenSize = UISystem::GetScreenSizeCategory(width);
    
    switch (screenSize) {
        case ScreenSize::SMALL:
            // Compact vertical layout
            GetLayout()->SetDirection(FlexDirection::Column);
            GetLayout()->SetGap(15);
            break;
            
        case ScreenSize::LARGE:
            // Expanded horizontal layout
            GetLayout()->SetDirection(FlexDirection::Row);
            GetLayout()->SetGap(30);
            break;
    }
}
```

### **Scalable Component Design**
```cpp
// Use logical pixels - automatically scaled by system
ComponentStyle style = {
    .width = "60%",              // Percentage - always responsive
    .minWidth = 200,             // Logical pixels - scaled by DPI + user preference
    .height = 48,                // Standard button height - scales automatically
    .fontSize = 16,              // Readable text - scales with UI scale
    .padding = {12, 20, 12, 20}, // Comfortable touch targets - scales up
    .borderRadius = 8            // Consistent appearance - scales proportionally
};
```

## 🔧 **Best Practices**

### **Component Creation**
1. Always use `CreateComponent<T>()` instead of `new` for automatic memory management
2. Set up layout managers before adding child components
3. Use percentage-based sizing for responsive design
4. Set min/max constraints to prevent UI from breaking
5. **Use LogicalPixels() for all fixed measurements to ensure DPI scaling**
6. **Test with multiple scale factors (100%, 150%, 200%) during development**

### **Performance**
1. Reuse components when possible instead of recreating
2. Use component pooling for frequently created/destroyed elements
3. Batch style updates to minimize redraws
4. Use visibility flags instead of destroying components temporarily
5. **Cache scaled measurements to avoid recalculation on every frame**
6. **Use appropriate asset resolution - don't load 3x assets at 1x scale**

### **Maintainability**
1. Keep menu classes focused on layout and event handling
2. Extract complex logic into separate classes
3. Use consistent naming conventions
4. Document custom components and their usage
5. **Always specify measurements in logical units unless physical pixels are specifically needed**
6. **Test UI at different scale factors and screen sizes during development**
