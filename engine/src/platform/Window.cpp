#include "platform/Window.h" // Assuming Window.h is in platform directory relative to engine src
#include <iostream> // For basic error reporting

Window::Window(const std::string &title, int width, int height)
    : windowTitle(title), windowWidth(width), windowHeight(height), running(false) {}

Window::~Window() {
    cleanUp();
}

bool Window::init() {
    // SDL_Init returns 0 on success, or -1 on error.
    if (SDL_Init(SDL_INIT_VIDEO) != 0) { // Changed from < 0 to != 0 for SDL3 style/warning
        std::cerr << "SDL could not initialize! SDL_Error: " << SDL_GetError() << std::endl;
        std::cerr << "Additional debug info: SDL initialization failed. This could be due to missing drivers or permissions." << std::endl;
        return false;
    }

    sdlWindow = SDL_CreateWindow(windowTitle.c_str(), windowWidth, windowHeight, SDL_WINDOW_OPENGL);
    if (sdlWindow == nullptr) {
        std::cerr << "Window could not be created! SDL_Error: " << SDL_GetError() << std::endl;
        SDL_Quit();
        return false;
    }

    // Create renderer (optional, if not using OpenGL directly)
    // SDL3: SDL_CreateRenderer flags like SDL_RENDERER_ACCELERATED are generally not needed,
    // SDL will pick the best available. Pass NULL for the name for default.
    sdlRenderer = SDL_CreateRenderer(sdlWindow, NULL); // Removed SDL_RENDERER_ACCELERATED
    if (sdlRenderer == nullptr) {
        std::cerr << "Renderer could not be created! SDL Error: " << SDL_GetError() << std::endl;
        // We can still run without a renderer for basic windowing, or choose to fail
        // For now, let's allow it to proceed, but a real engine would likely fail or have a fallback.
    }
    
    running = true;
    return true;
}

void Window::handleEvents() {
    SDL_Event e;
    while (SDL_PollEvent(&e)) {
        if (e.type == SDL_EVENT_QUIT) {
            running = false;
        }

        // Example: Handle keyboard input
        if (e.type == SDL_EVENT_KEY_DOWN) {
            if (e.key.key == SDLK_ESCAPE) { // Changed from e.key.keysym.sym to e.key.key
                running = false;
            }
        }
    }
}

void Window::update() {
    // Game logic updates would go here
}

void Window::render() {
    if (sdlRenderer) {
        SDL_SetRenderDrawColor(sdlRenderer, 0x00, 0x33, 0x66, 0xFF); // A dark blue background
        SDL_RenderClear(sdlRenderer);

        // Rendering game objects would happen here

        SDL_RenderPresent(sdlRenderer);
    } else {
        // If no renderer (e.g. using OpenGL directly), swap buffers if applicable
        // SDL_GL_SwapWindow(sdlWindow); // This is an SDL2 function, SDL3 uses SDL_GL_SwapWindow or similar with context
    }
}

void Window::cleanUp() {
    if (sdlRenderer) {
        SDL_DestroyRenderer(sdlRenderer);
        sdlRenderer = nullptr;
    }
    if (sdlWindow) {
        SDL_DestroyWindow(sdlWindow);
        sdlWindow = nullptr;
    }
    SDL_Quit();
    running = false;
}
