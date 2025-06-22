#include "ui/menus/MainMenu.h"
#include "ui/core/UISystem.h"
#include "ui/core/UITheme.h"
#include "ui/MenuSystem.h"
#include <iostream>

namespace VoxelEngine {
namespace UI {

MainMenu::MainMenu()
    : menuSystem_(nullptr) {
}

void MainMenu::Initialize() {
    UIComponent::Initialize();
    
    std::cout << "[MainMenu] Initializing new main menu..." << std::endl;
    
    CreateLayout();
    CreateTitle();
    CreateButtons();
    ApplyTheme();
    
    std::cout << "[MainMenu] Main menu initialization complete" << std::endl;
}

void MainMenu::CreateLayout() {
    // Set up main vertical layout that centers content on screen
    mainLayout_ = std::make_shared<FlexLayout>(FlexLayout::Direction::COLUMN);
    mainLayout_->SetJustifyContent(FlexLayout::JustifyContent::CENTER);
    mainLayout_->SetAlignItems(FlexLayout::AlignItems::CENTER);
    
    // Get responsive spacing based on screen size
    auto uiSystem = GetUISystem();
    if (uiSystem) {
        float screenWidth = uiSystem->GetScreenSize().x;
        auto theme = uiSystem->GetTheme();
        float responsiveGap = theme->GetResponsiveSpacing(40.0f, screenWidth);
        mainLayout_->SetGap(responsiveGap);
        
        // Add responsive padding to prevent content from touching screen edges
        float padding = theme->GetResponsiveSpacing(50.0f, screenWidth);
        mainLayout_->SetPadding(padding);
    } else {
        mainLayout_->SetGap(40.0f);
        mainLayout_->SetPadding(50.0f);
    }
    
    SetLayout(mainLayout_);
}

void MainMenu::CreateTitle() {
    // Create title label with responsive sizing
    titleLabel_ = CreateComponent<Label>("Voxel Castle");
    titleLabel_->SetTextAlign(Label::TextAlign::CENTER);
    titleLabel_->SetAutoSize(true);
    
    auto uiSystem = GetUISystem();
    if (uiSystem) {
        float screenWidth = uiSystem->GetScreenSize().x;
        auto theme = uiSystem->GetTheme();
        
        // Responsive font size: larger on bigger screens
        float baseFontSize = 48.0f;
        float responsiveFontSize = theme->GetResponsiveFontSize(baseFontSize, screenWidth);
        titleLabel_->SetFontSize(responsiveFontSize);
    } else {
        titleLabel_->SetFontSize(48.0f);
    }
    
    // Add title directly to MainMenu (will be centered by mainLayout_)
    AddChild(titleLabel_);
}

void MainMenu::CreateButtons() {
    // Create button container panel with responsive sizing
    buttonPanel_ = CreateComponent<Panel>();
    buttonPanel_->SetBackgroundVisible(false);
    
    // Set up responsive button layout
    buttonLayout_ = std::make_shared<FlexLayout>(FlexLayout::Direction::COLUMN);
    buttonLayout_->SetJustifyContent(FlexLayout::JustifyContent::CENTER);
    buttonLayout_->SetAlignItems(FlexLayout::AlignItems::CENTER);
    
    auto uiSystem = GetUISystem();
    if (uiSystem) {
        float screenWidth = uiSystem->GetScreenSize().x;
        auto theme = uiSystem->GetTheme();
        
        // Responsive gap between buttons
        float responsiveGap = theme->GetResponsiveSpacing(20.0f, screenWidth);
        buttonLayout_->SetGap(responsiveGap);
    } else {
        buttonLayout_->SetGap(20.0f);
    }
    
    buttonPanel_->SetLayout(buttonLayout_);
    
    // Create buttons
    resumeButton_ = CreateComponent<Button>("Resume Game");
    resumeButton_->SetStyle(Button::Style::PRIMARY);
    resumeButton_->SetSize(Button::Size::LARGE);
    resumeButton_->SetOnClick([this]() { OnResumeGameClick(); });
    buttonPanel_->AddChild(resumeButton_);
    
    newWorldButton_ = CreateComponent<Button>("New World");
    newWorldButton_->SetStyle(Button::Style::PRIMARY);
    newWorldButton_->SetSize(Button::Size::LARGE);
    newWorldButton_->SetOnClick([this]() { OnNewWorldClick(); });
    buttonPanel_->AddChild(newWorldButton_);
    
    loadWorldButton_ = CreateComponent<Button>("Load World");
    loadWorldButton_->SetStyle(Button::Style::SECONDARY);
    loadWorldButton_->SetSize(Button::Size::LARGE);
    loadWorldButton_->SetOnClick([this]() { OnLoadWorldClick(); });
    buttonPanel_->AddChild(loadWorldButton_);
    
    settingsButton_ = CreateComponent<Button>("Settings");
    settingsButton_->SetStyle(Button::Style::SECONDARY);
    settingsButton_->SetSize(Button::Size::LARGE);
    settingsButton_->SetOnClick([this]() { OnSettingsClick(); });
    buttonPanel_->AddChild(settingsButton_);
    
    exitButton_ = CreateComponent<Button>("Exit Game");
    exitButton_->SetStyle(Button::Style::DANGER);
    exitButton_->SetSize(Button::Size::LARGE);
    exitButton_->SetOnClick([this]() { OnExitGameClick(); });
    buttonPanel_->AddChild(exitButton_);
    
    AddChild(buttonPanel_);
}

void MainMenu::ApplyTheme() {
    auto uiSystem = GetUISystem();
    if (!uiSystem) return;
    
    auto theme = uiSystem->GetTheme();
    
    // Apply theme to title
    titleLabel_->SetTextColor(theme->colors.textPrimary);
    
    std::cout << "[MainMenu] Theme applied successfully" << std::endl;
}

void MainMenu::OnResumeGameClick() {
    std::cout << "[MainMenu] Resume Game clicked" << std::endl;
    
    if (menuSystem_) {
        // Use legacy world system for Resume Game
        menuSystem_->requestWorldInitialization();
    }
}

void MainMenu::OnNewWorldClick() {
    std::cout << "[MainMenu] New World clicked" << std::endl;
    
    if (menuSystem_) {
        // Show new world creation menu
        menuSystem_->onNewGameClicked();
    }
}

void MainMenu::OnLoadWorldClick() {
    std::cout << "[MainMenu] Load World clicked" << std::endl;
    
    if (menuSystem_) {
        // TODO: Implement world loading menu
        menuSystem_->onLoadGameClicked();
    }
}

void MainMenu::OnSettingsClick() {
    std::cout << "[MainMenu] Settings clicked" << std::endl;
    
    if (menuSystem_) {
        // TODO: Implement settings menu
        menuSystem_->onSettingsClicked();
    }
}

void MainMenu::OnExitGameClick() {
    std::cout << "[MainMenu] Exit Game clicked" << std::endl;
    
    if (menuSystem_) {
        menuSystem_->requestExit();
    }
}

void MainMenu::OnScreenSizeChanged(float screenWidth, float screenHeight) {
    UIComponent::OnScreenSizeChanged(screenWidth, screenHeight);
    
    auto uiSystem = GetUISystem();
    if (!uiSystem) return;
    
    auto theme = uiSystem->GetTheme();
    
    // Update responsive spacing for main layout
    if (mainLayout_) {
        float responsiveGap = theme->GetResponsiveSpacing(40.0f, screenWidth);
        float responsivePadding = theme->GetResponsiveSpacing(50.0f, screenWidth);
        mainLayout_->SetGap(responsiveGap);
        mainLayout_->SetPadding(responsivePadding);
    }
    
    // Update responsive font size for title
    if (titleLabel_) {
        float baseFontSize = 48.0f;
        float responsiveFontSize = theme->GetResponsiveFontSize(baseFontSize, screenWidth);
        titleLabel_->SetFontSize(responsiveFontSize);
    }
    
    // Update responsive gap for button layout
    if (buttonLayout_) {
        float responsiveGap = theme->GetResponsiveSpacing(20.0f, screenWidth);
        buttonLayout_->SetGap(responsiveGap);
    }
    
    std::cout << "[MainMenu] Updated responsive layout for screen size: " << screenWidth << "x" << screenHeight << std::endl;
}

} // namespace UI
} // namespace VoxelEngine
