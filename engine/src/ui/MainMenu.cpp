#include <SFML/Graphics.hpp>
#include <iostream>
#include "MainMenu.h"
#include "Game.h"
#include "MenuSystem.h"

MainMenu::MainMenu(Game& game, MenuSystem& menuSystem) : BaseMenu(game, menuSystem) {
    sf::Vector2f windowSize(static_cast<float>(game.GetWindow().getSize().x), static_cast<float>(game.GetWindow().getSize().y));

    // Add new buttons first to establish layout
    AddButton("Create World", sf::Vector2f(windowSize.x / 2, windowSize.y / 2 - 120), std::bind(&MainMenu::CreateWorld, this));
    AddButton("Load World", sf::Vector2f(windowSize.x / 2, windowSize.y / 2 - 60), std::bind(&MainMenu::LoadWorld, this));
    AddButton("Resume Game (Legacy)", sf::Vector2f(windowSize.x / 2, windowSize.y / 2), std::bind(&MainMenu::ResumeGame, this));

    // Original buttons, now adjusted or removed
    // AddButton("Resume Game", sf::Vector2f(windowSize.x / 2, windowSize.y / 2 - 60), std::bind(&MainMenu::ResumeGame, this));
    AddButton("Options", sf::Vector2f(windowSize.x / 2, windowSize.y / 2 + 60), std::bind(&MainMenu::OpenOptions, this));
    AddButton("Exit", sf::Vector2f(windowSize.x / 2, windowSize.y / 2 + 120), std::bind(&MainMenu::ExitGame, this));
}

void MainMenu::ResumeGame() {
    menuSystem.SetState(MenuState::HUD);
    game.ResumeGame();
}

void MainMenu::CreateWorld() {
    // Placeholder for new world generation logic
    // For now, it can just log a message
    std::cout << "Create World button clicked!" << std::endl;
    // Eventually, this will transition to a new menu state for world creation
}

void MainMenu::LoadWorld() {
    // Placeholder for loading a world from the new system
    std::cout << "Load World button clicked!" << std::endl;
}

void MainMenu::OpenOptions() {
    std::cout << "Options button clicked!" << std::endl;
    // Logic to open options menu
}

void MainMenu::ExitGame() {
    game.Exit();
}