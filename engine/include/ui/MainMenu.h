#ifndef MAIN_MENU_H
#define MAIN_MENU_H

#include "BaseMenu.h"

class MainMenu : public BaseMenu {
public:
    MainMenu(Game& game, MenuSystem& menuSystem);

private:
    void ResumeGame();
    void CreateWorld();
    void LoadWorld();
    void OpenOptions();
    void ExitGame();
};

#endif // MAIN_MENU_H