
#ifndef WINDOW_H
#define WINDOW_H

#include <string>

// GLAD must be included before any other OpenGL related headers (including SDL_opengl.h)
#include <glad/glad.h> 

// SDL main header
#include <SDL3/SDL.h> 
// SDL video header for video-specific functions like SDL_GL_GetDrawableSize
#include <SDL3/SDL_video.h> 
// SDL OpenGL header for context management
#include <SDL3/SDL_opengl.h> 

#include "rendering/debug_render_mode.h" // Moved inside include guard

class Window {
public:
    Window(const std::string& title, int width, int height);
    ~Window();

    bool init();
    void handleEvents(); // Though not used directly by game loop, keep for completeness
    void update();       // For window-specific updates
    void render();       // For swapping buffers
    bool toggleFullscreen();
    bool isFullscreen() const;
    void cleanUp();

    int getWidth() const;
    int getHeight() const;
    SDL_Window* getSDLWindow() const; // Made this getter const
    bool isRunning() const; // Getter for the running state

private:
    std::string windowTitle;
    int windowWidth;
    int windowHeight;
    SDL_Window* sdlWindow;
    SDL_GLContext glContext;
    bool running;
    bool fullscreen;
    // Add any other private helper methods or variables if needed later
};

#endif // WINDOW_H