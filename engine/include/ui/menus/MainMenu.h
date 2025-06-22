#pragma once

#include "ui/core/UIComponent.h"
#include "ui/components/Button.h"
#include "ui/components/Label.h"
#include "ui/components/Panel.h"
#include "ui/layout/FlexLayout.h"
#include <memory>

namespace VoxelEngine {
namespace UI {

class MenuSystem;

/**
 * @brief Main menu for the game
 * 
 * Provides access to:
 * - Resume Game (legacy world system)
 * - New World (new procedural system)
 * - Load World
 * - Settings
 * - Exit Game
 */
class MainMenu : public UIComponent {
public:
    MainMenu();
    ~MainMenu() override = default;
    
    void Initialize() override;
    void SetMenuSystem(MenuSystem* menuSystem) { menuSystem_ = menuSystem; }
    
    // Responsive design
    void OnScreenSizeChanged(float screenWidth, float screenHeight) override;
    
private:
    void OnResumeGameClick();
    void OnNewWorldClick();
    void OnLoadWorldClick();
    void OnSettingsClick();
    void OnExitGameClick();
    
    void CreateLayout();
    void CreateTitle();
    void CreateButtons();
    void ApplyTheme();
    
    MenuSystem* menuSystem_;
    
    // UI Components
    std::shared_ptr<Panel> backgroundPanel_;
    std::shared_ptr<Label> titleLabel_;
    std::shared_ptr<Panel> buttonPanel_;
    
    // Buttons
    std::shared_ptr<Button> resumeButton_;
    std::shared_ptr<Button> newWorldButton_;
    std::shared_ptr<Button> loadWorldButton_;
    std::shared_ptr<Button> settingsButton_;
    std::shared_ptr<Button> exitButton_;
    
    // Layout
    std::shared_ptr<FlexLayout> mainLayout_;
    std::shared_ptr<FlexLayout> buttonLayout_;
};

} // namespace UI
} // namespace VoxelEngine
