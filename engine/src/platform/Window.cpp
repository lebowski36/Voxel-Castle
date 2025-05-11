#include "platform/Window.h" // Assuming Window.h is in platform directory relative to engine src
#include <iostream> // For basic error reporting

Window::Window(const std::string &title, int width, int height)
    : windowTitle(title), windowWidth(width), windowHeight(height), 
      sdlWindow(nullptr), sdlRenderer(nullptr), running(false) {}

Window::~Window() {
    cleanUp();
}

bool Window::init() {
    std::cout << "Initializing SDL..." << std::endl;

    // Initialize SDL video subsystem
    if (SDL_Init(SDL_INIT_VIDEO) < 0) {
        std::cerr << "Failed to initialize SDL video. SDL_Error: " << SDL_GetError() << std::endl;
        return false;
    }
    std::cout << "SDL video subsystem initialized successfully." << std::endl;

    // Create window
    sdlWindow = SDL_CreateWindow(
        windowTitle.c_str(),
        windowWidth,
        windowHeight,
        0 // Using 0 for basic window flags. Specific flags like SDL_WINDOW_OPENGL can be added later.
    );

    if (sdlWindow == nullptr) {
        std::cerr << "Window could not be created! SDL_Error: " << SDL_GetError() << std::endl;
        SDL_Quit(); // Clean up SDL subsystems if window creation fails
        return false;
    }
    std::cout << "Window created successfully." << std::endl;

    // Create renderer
    // The SDL3 version being used appears to expect only two arguments:
    // SDL_Window* and const char* (for renderer name, or nullptr for default).
    sdlRenderer = SDL_CreateRenderer(sdlWindow, nullptr); 
    if (sdlRenderer == nullptr) {
        std::cerr << "Basic renderer could not be created! SDL_Error: " << SDL_GetError() << std::endl;
        SDL_DestroyWindow(sdlWindow);
        sdlWindow = nullptr;
        SDL_Quit();
        return false;
    } else {
        std::cout << "Basic renderer created successfully." << std::endl;
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
            if (e.key.key == SDLK_ESCAPE) {
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
        std::cout << "Renderer destroyed." << std::endl;
    }
    if (sdlWindow) {
        SDL_DestroyWindow(sdlWindow);
        sdlWindow = nullptr;
        std::cout << "Window destroyed." << std::endl;
    }
    SDL_Quit(); // SDL_Quit should be called after destroying windows and renderers.
    std::cout << "SDL quit." << std::endl;
    running = false;
}
