#include "platform/Window.h"
#include "rendering/debug_render_mode.h"

#include <iostream> 
#include <glad/glad.h> // Should be included before SDL_opengl.h
#include <SDL3/SDL_opengl.h>

Window::Window(const std::string &title, int width, int height)
    : windowTitle(title), windowWidth(width), windowHeight(height), 
      sdlWindow(nullptr), glContext(nullptr), running(false), fullscreen(false) {}

Window::~Window() {
    cleanUp();
}

bool Window::init() {
    std::cout << "Initializing SDL..." << std::endl;

    if (SDL_Init(SDL_INIT_VIDEO) < 0) {
        std::cerr << "Failed to initialize SDL video. SDL_Error: " << SDL_GetError() << std::endl;
        return false;
    }
    std::cout << "SDL video subsystem initialized successfully." << std::endl;

    // Set OpenGL attributes
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 3);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 3);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);
    // SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1); // Usually enabled by default
    // SDL_GL_SetAttribute(SDL_GL_DEPTH_SIZE, 24); // For depth testing

    std::cout << "Creating window with OpenGL support..." << std::endl;
    sdlWindow = SDL_CreateWindow(
        windowTitle.c_str(),
        windowWidth,
        windowHeight,
        SDL_WINDOW_OPENGL | SDL_WINDOW_RESIZABLE // Use OpenGL flag and add RESIZABLE
    );

    if (sdlWindow == nullptr) {
        std::cerr << "Window could not be created! SDL_Error: " << SDL_GetError() << std::endl;
        SDL_Quit(); 
        return false;
    }
    std::cout << "Window created successfully." << std::endl;

    glContext = SDL_GL_CreateContext(sdlWindow);
    if (glContext == nullptr) {
        std::cerr << "OpenGL context could not be created! SDL_Error: " << SDL_GetError() << std::endl;
        SDL_DestroyWindow(sdlWindow);
        sdlWindow = nullptr;
        SDL_Quit();
        return false;
    }
    std::cout << "OpenGL context created successfully." << std::endl;

    // Initialize GLAD
    // Note: SDL_GL_GetProcAddress is the correct way to get GL function pointers with SDL
    if (!gladLoadGLLoader((GLADloadproc)SDL_GL_GetProcAddress)) {
        SDL_Log("Failed to initialize GLAD: error code %u", glGetError()); // Changed from %s to %u
        SDL_GL_DestroyContext(glContext); // Changed from SDL_GL_DeleteContext
        SDL_DestroyWindow(sdlWindow);
        sdlWindow = nullptr;
        SDL_Quit();
        return false;
    }
    std::cout << "GLAD initialized successfully." << std::endl;
    std::cout << "OpenGL Version: " << glGetString(GL_VERSION) << std::endl;
    std::cout << "GLSL Version: " << glGetString(GL_SHADING_LANGUAGE_VERSION) << std::endl;
    std::cout << "Vendor: " << glGetString(GL_VENDOR) << std::endl;
    std::cout << "Renderer: " << glGetString(GL_RENDERER) << std::endl;


    // Set up the viewport
    glViewport(0, 0, windowWidth, windowHeight);
    // Set the clear color to black for debug contrast
    glClearColor(0.0f, 0.0f, 0.0f, 1.0f); // Black

    running = true;
    return true;
}

void Window::handleEvents() {
    // SDL_Event e;
    // while (SDL_PollEvent(&e)) {
    //     if (e.type == SDL_EVENT_QUIT) {
    //         running = false;
    //     }

    //     if (e.type == SDL_EVENT_KEY_DOWN) {
    //         if (e.key.scancode == SDL_SCANCODE_ESCAPE) {
    //             running = false;
    //         }
    //     }
    // }
    // This method is currently not called by the main game loop,
    // event handling is done in InputManager.cpp.
    // If this were to be used, it would need to be integrated
    // with the Game class's event loop.
    // For now, let's keep it empty to avoid confusion or potential conflicts.
}

void Window::update() {
    // Game logic updates would go here
    // This method can be used for window-specific updates
    // For now, it's empty as there's nothing specific to update
}

void Window::render() {
    // Rendering game objects would happen here using OpenGL calls

    // Swap buffers
    SDL_GL_SwapWindow(sdlWindow);
    // This method is called by the Game class to render each frame
}

bool Window::toggleFullscreen() {
    if (!sdlWindow) {
        std::cerr << "Cannot toggle fullscreen: Window is null" << std::endl;
        return false;
    }
    
    // Toggle fullscreen state
    fullscreen = !fullscreen;
    
    std::cout << "Toggling fullscreen mode: " << (fullscreen ? "ON" : "OFF") << std::endl;
    
    // Set the new fullscreen state
    SDL_WindowFlags flags = fullscreen ? 
        SDL_WINDOW_FULLSCREEN : 
        SDL_WINDOW_RESIZABLE;
        
    if (SDL_SetWindowFullscreen(sdlWindow, fullscreen) != 0) {
        std::cerr << "Failed to toggle fullscreen mode: " << SDL_GetError() << std::endl;
        fullscreen = !fullscreen; // Revert the state
        return false;
    }
    
    // When returning from fullscreen, restore the window size
    if (!fullscreen) {
        SDL_SetWindowSize(sdlWindow, windowWidth, windowHeight);
        // Reset viewport in case size changed
        glViewport(0, 0, windowWidth, windowHeight);
    }
    
    return true;
}

bool Window::isFullscreen() const {
    return fullscreen;
}

void Window::cleanUp() {
    if (glContext) {
        SDL_GL_DestroyContext(glContext); // Changed from SDL_GL_DeleteContext
        glContext = nullptr;
        std::cout << "OpenGL context destroyed." << std::endl;
    }
    if (sdlWindow) {
        SDL_DestroyWindow(sdlWindow);
        sdlWindow = nullptr;
        std::cout << "Window destroyed." << std::endl;
    }
    SDL_Quit(); 
    std::cout << "SDL quit." << std::endl;
}
