#pragma once

#include "ui/elements/BaseMenu.h"
#include "ui/elements/UIButton.h"
#include "ui/elements/UITextInput.h"
#include <memory>
#include <string>

namespace VoxelEngine {
namespace UI {

class MenuSystem;

/**
 * @brief UI screen for creating a new world.
 * 
 * Allows the user to input a world seed and other world generation options.
 */
class CreateWorldMenu : public BaseMenu {
public:
    CreateWorldMenu(UIRenderer* renderer);
    ~CreateWorldMenu() override = default;

    /**
     * @brief Initialize the create world menu
     * @param menuSystem Pointer to the menu system that owns this menu
     * @return true if initialization succeeded, false otherwise
     */
    bool initialize(MenuSystem* menuSystem);

    void render() override;

private:
    std::shared_ptr<UITextInput> seedInput_;
    std::shared_ptr<UIButton> createButton_;
    std::shared_ptr<UIButton> cancelButton_;

    void onCreateClicked();
    void onCancelClicked();
};

} // namespace UI
} // namespace VoxelEngine
