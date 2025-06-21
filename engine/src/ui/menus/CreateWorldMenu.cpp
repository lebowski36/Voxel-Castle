#include "ui/menus/CreateWorldMenu.h"
#include "ui/core/UISystem.h"
#include "ui/core/UITheme.h"
#include "ui/MenuSystem.h"
#include <iostream>

namespace VoxelEngine {
namespace UI {

CreateWorldMenu::CreateWorldMenu()
    : menuSystem_(nullptr) {
}

void CreateWorldMenu::Initialize() {
    UIComponent::Initialize();
    
    std::cout << "[CreateWorldMenu] Initializing create world menu..." << std::endl;
    
    CreateLayout();
    CreateTitle();
    CreateSeedInput();
    CreateButtons();
    ApplyTheme();
    
    std::cout << "[CreateWorldMenu] Create world menu initialization complete" << std::endl;
}

void CreateWorldMenu::CreateLayout() {
    // Set up main vertical layout
    mainLayout_ = std::make_shared<FlexLayout>(FlexLayout::Direction::COLUMN);
    mainLayout_->SetJustifyContent(FlexLayout::JustifyContent::CENTER);
    mainLayout_->SetAlignItems(FlexLayout::AlignItems::CENTER);
    mainLayout_->SetGap(30.0f);
    SetLayout(mainLayout_);
    
    // Create background panel with semi-transparent dark background
    backgroundPanel_ = CreateComponent<Panel>();
    backgroundPanel_->SetBackgroundColor(glm::vec4(0.05f, 0.05f, 0.1f, 0.95f));
    backgroundPanel_->SetBorderVisible(true);
    backgroundPanel_->SetBorderColor(glm::vec4(0.3f, 0.3f, 0.4f, 1.0f));
    backgroundPanel_->SetBorderWidth(2.0f);
    backgroundPanel_->SetCornerRadius(12.0f);
    backgroundPanel_->SetPadding(40.0f);
    AddChild(backgroundPanel_);
}

void CreateWorldMenu::CreateTitle() {
    // Create title label
    titleLabel_ = CreateComponent<Label>("Create New World");
    titleLabel_->SetFontSize(32.0f);
    titleLabel_->SetTextAlign(Label::TextAlign::CENTER);
    titleLabel_->SetAutoSize(true);
    
    auto uiSystem = GetUISystem();
    if (uiSystem) {
        auto theme = uiSystem->GetTheme();
        titleLabel_->SetTextColor(theme->colors.textPrimary);
    }
    
    backgroundPanel_->AddChild(titleLabel_);
}

void CreateWorldMenu::CreateSeedInput() {
    // Create seed label
    seedLabel_ = CreateComponent<Label>("World Seed (leave empty for random)");
    seedLabel_->SetFontSize(16.0f);
    seedLabel_->SetTextAlign(Label::TextAlign::CENTER);
    seedLabel_->SetAutoSize(true);
    
    auto uiSystem = GetUISystem();
    if (uiSystem) {
        auto theme = uiSystem->GetTheme();
        seedLabel_->SetTextColor(theme->colors.textSecondary);
    }
    
    backgroundPanel_->AddChild(seedLabel_);
    
    // Create seed input field
    seedInput_ = CreateComponent<TextInput>("Enter world seed...");
    seedInput_->SetSize(glm::vec2(300.0f, 44.0f));
    seedInput_->SetMaxLength(100);
    seedInput_->SetOnTextChanged([this](const std::string& text) { OnSeedChanged(text); });
    seedInput_->SetOnEnter([this]() { OnSeedEnter(); });
    
    // Style the input field
    seedInput_->SetBackgroundColor(glm::vec4(0.15f, 0.15f, 0.2f, 1.0f));
    seedInput_->SetTextColor(glm::vec4(1.0f, 1.0f, 1.0f, 1.0f));
    seedInput_->SetBorderColor(glm::vec4(0.4f, 0.4f, 0.5f, 1.0f));
    seedInput_->SetFocusColor(glm::vec4(0.3f, 0.6f, 1.0f, 1.0f));
    
    backgroundPanel_->AddChild(seedInput_);
}

void CreateWorldMenu::CreateButtons() {
    // Create button container panel
    buttonPanel_ = CreateComponent<Panel>();
    buttonPanel_->SetBackgroundVisible(false);
    
    // Set up button layout
    buttonLayout_ = std::make_shared<FlexLayout>(FlexLayout::Direction::ROW);
    buttonLayout_->SetJustifyContent(FlexLayout::JustifyContent::CENTER);
    buttonLayout_->SetAlignItems(FlexLayout::AlignItems::CENTER);
    buttonLayout_->SetGap(20.0f);
    buttonPanel_->SetLayout(buttonLayout_);
    
    // Create buttons
    createButton_ = CreateComponent<Button>("Create World");
    createButton_->SetStyle(Button::Style::SUCCESS);
    createButton_->SetSize(Button::Size::LARGE);
    createButton_->SetOnClick([this]() { OnCreateWorldClick(); });
    buttonPanel_->AddChild(createButton_);
    
    cancelButton_ = CreateComponent<Button>("Cancel");
    cancelButton_->SetStyle(Button::Style::SECONDARY);
    cancelButton_->SetSize(Button::Size::LARGE);
    cancelButton_->SetOnClick([this]() { OnCancelClick(); });
    buttonPanel_->AddChild(cancelButton_);
    
    backgroundPanel_->AddChild(buttonPanel_);
}

void CreateWorldMenu::ApplyTheme() {
    auto uiSystem = GetUISystem();
    if (!uiSystem) return;
    
    auto theme = uiSystem->GetTheme();
    
    // Apply theme to labels
    titleLabel_->SetTextColor(theme->colors.textPrimary);
    seedLabel_->SetTextColor(theme->colors.textSecondary);
    
    std::cout << "[CreateWorldMenu] Theme applied successfully" << std::endl;
}

void CreateWorldMenu::OnCreateWorldClick() {
    std::string seed = seedInput_->GetText();
    std::cout << "[CreateWorldMenu] Creating world with seed: '" << seed << "'" << std::endl;
    
    if (menuSystem_) {
        // Request procedural world creation with the entered seed
        menuSystem_->requestProceduralWorldCreation(seed);
    }
}

void CreateWorldMenu::OnCancelClick() {
    std::cout << "[CreateWorldMenu] Cancel clicked, returning to main menu" << std::endl;
    
    if (menuSystem_) {
        menuSystem_->closeAllMenus();
    }
}

void CreateWorldMenu::OnSeedChanged(const std::string& seed) {
    // Optional: Validate seed input
    std::cout << "[CreateWorldMenu] Seed changed to: '" << seed << "'" << std::endl;
}

void CreateWorldMenu::OnSeedEnter() {
    // When user presses Enter in seed field, create the world
    OnCreateWorldClick();
}

} // namespace UI
} // namespace VoxelEngine
