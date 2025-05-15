#include "rendering/debug_render_mode.h"
#ifndef WINDOW_H
#define WINDOW_H

#include <string>
#include <SDL3/SDL.h> // Use SDL3 specific path
#include <glad/glad.h> // Include GLAD header

class Window {
public:
    Window(const std::string &title, int width, int height);
    ~Window();

    bool init();
    void handleEvents();
    void update();
    void render();
    void cleanUp();

    bool isRunning() const { return running; }
    int getWidth() const { return windowWidth; }
    int getHeight() const { return windowHeight; }
    SDL_Window* getSDLWindow() const { return sdlWindow; }

private:
    std::string windowTitle;
    int windowWidth;
    int windowHeight;
    
    SDL_Window* sdlWindow = nullptr;
    SDL_GLContext glContext = nullptr; // OpenGL context

    bool running;
};

#endif // WINDOW_H
