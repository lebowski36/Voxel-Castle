#ifndef WINDOW_H
#define WINDOW_H

#include <string>
#include <SDL3/SDL.h> // Use SDL3 specific path

// Forward declaration if SDL_Window and SDL_Renderer are pointers
// struct SDL_Window;
// struct SDL_Renderer;

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
    SDL_Window* getSDLWindow() const { return sdlWindow; } // Optional: if needed externally

private:
    std::string windowTitle;
    int windowWidth;
    int windowHeight;
    
    SDL_Window* sdlWindow = nullptr;
    SDL_Renderer* sdlRenderer = nullptr; // For basic SDL rendering

    bool running; // Moved running to be after SDL objects
};

#endif // WINDOW_H
