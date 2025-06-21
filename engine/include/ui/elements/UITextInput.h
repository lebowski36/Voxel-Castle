#pragma once

#include "ui/UIElement.h"
#include "ui/UIRenderer.h"
#include <string>
#include <functional>

namespace VoxelEngine {
namespace UI {

class UITextInput : public UIElement {
public:
    UITextInput(UIRenderer* renderer);
    ~UITextInput() override = default;

    void setText(const std::string& text);
    const std::string& getText() const { return text_; }
    void setPlaceholder(const std::string& placeholder);
    const std::string& getPlaceholder() const { return placeholder_; }

    void setBackgroundColor(const glm::vec4& color);
    void setTextColor(const glm::vec4& color);
    void setBorderColor(const glm::vec4& color);

    void setOnEnter(std::function<void()> onEnter);

    void render() override;
    
    // Legacy input handling for compatibility
    bool handleInput(float mouseX, float mouseY, bool clicked) override;
    
    // New detailed input handling
    void handleMouseButton(int button, int action, int mods, double xpos, double ypos) override;
    void handleKey(int key, int scancode, int action, int mods) override;
    void handleChar(unsigned int codepoint) override;

    bool isFocused() const override { return focused_; }

private:
    UIRenderer* renderer_;
    std::function<void()> onEnter_;
    std::string text_;
    std::string placeholder_;
    glm::vec4 backgroundColor_ = {0.2f, 0.2f, 0.2f, 1.0f};
    glm::vec4 textColor_ = {1.0f, 1.0f, 1.0f, 1.0f};
    glm::vec4 borderColor_ = {0.5f, 0.5f, 0.5f, 1.0f};
    bool focused_ = false;
};

} // namespace UI
} // namespace VoxelEngine
