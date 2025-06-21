#pragma once

#include "ui/core/UIComponent.h"
#include "ui/components/Button.h"
#include "ui/components/Label.h"
#include "ui/components/Panel.h"
#include "ui/components/TextInput.h"
#include "ui/layout/FlexLayout.h"
#include <memory>

namespace VoxelEngine {
namespace UI {

class MenuSystem;

/**
 * @brief Create new world menu
 * 
 * Allows the user to:
 * - Enter a world seed (optional)
 * - Create a new world with procedural generation
 * - Cancel and return to main menu
 */
class CreateWorldMenu : public UIComponent {
public:
    CreateWorldMenu();
    ~CreateWorldMenu() override = default;
    
    void Initialize() override;
    void SetMenuSystem(MenuSystem* menuSystem) { menuSystem_ = menuSystem; }
    
private:
    void OnCreateWorldClick();
    void OnCancelClick();
    void OnSeedChanged(const std::string& seed);
    void OnSeedEnter();
    
    void CreateLayout();
    void CreateTitle();
    void CreateSeedInput();
    void CreateButtons();
    void ApplyTheme();
    
    MenuSystem* menuSystem_;
    
    // UI Components
    std::shared_ptr<Panel> backgroundPanel_;
    std::shared_ptr<Label> titleLabel_;
    std::shared_ptr<Label> seedLabel_;
    std::shared_ptr<TextInput> seedInput_;
    std::shared_ptr<Panel> buttonPanel_;
    
    // Buttons
    std::shared_ptr<Button> createButton_;
    std::shared_ptr<Button> cancelButton_;
    
    // Layout
    std::shared_ptr<FlexLayout> mainLayout_;
    std::shared_ptr<FlexLayout> buttonLayout_;
};

} // namespace UI
} // namespace VoxelEngine
