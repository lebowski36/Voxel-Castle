#pragma once

#include "ui/UIElement.h"
#include "ui/UIRenderer.h"
#include <string>
#include <functional>

namespace VoxelEngine {
namespace UI {

/**
 * @brief A checkbox UI element
 */
class UICheckbox : public UIElement {
public:
    UICheckbox(UIRenderer* renderer);
    ~UICheckbox() override = default;

    /**
     * @brief Set the checkbox label text
     * @param label The text to display next to the checkbox
     */
    void setLabel(const std::string& label);

    /**
     * @brief Get the checkbox label text
     * @return The text displayed next to the checkbox
     */
    const std::string& getLabel() const { return label_; }

    /**
     * @brief Set the checked state of the checkbox
     * @param checked Whether the checkbox is checked
     */
    void setChecked(bool checked);

    /**
     * @brief Get the checked state of the checkbox
     * @return Whether the checkbox is checked
     */
    bool isChecked() const { return checked_; }

    /**
     * @brief Set the callback to be called when the checkbox state changes
     * @param callback The function to call with the new state
     */
    void setOnToggle(std::function<void(bool)> callback);

    // UIElement interface
    void update(float deltaTime) override;
    void render() override;
    bool handleInput(float mouseX, float mouseY, bool clicked) override;

private:
    UIRenderer* renderer_;
    std::string label_;
    bool checked_{false};
    std::function<void(bool)> onToggle_;
    bool isHovered_{false};
};

} // namespace UI
} // namespace VoxelEngine
