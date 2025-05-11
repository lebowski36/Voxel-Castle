#include "platform/Window.h" // Assuming Window.h is in platform directory relative to engine src
#include <iostream> // For basic error reporting

Window::Window(const std::string &title, int width, int height)
    : windowTitle(title), windowWidth(width), windowHeight(height), running(false) {}

Window::~Window() {
    cleanUp();
}

bool Window::init() {
    // Just print that we're starting initialization
    std::cout << "Initializing window..." << std::endl;
    std::cout << "Current directory: ";
    system("pwd");
    std::cout << "Environment: ";
    system("env | grep -E 'DISPLAY|WAYLAND_DISPLAY|XDG_SESSION|SDL'");
    
    std::cout << "Graphics drivers: ";
    system("ls -l /dev/dri/ 2>&1");
    
    // Try initializing with just video subsystem
    if (SDL_Init(SDL_INIT_VIDEO) != 0) {
        std::cerr << "SDL could not initialize! SDL_Error: " << SDL_GetError() << std::endl;
        std::cerr << "Additional debug info: SDL initialization failed. This could be due to missing drivers or permissions." << std::endl;
        
        // Try fallback options (for testing purposes)
        std::cerr << "Attempting to initialize with minimal flags..." << std::endl;
        if (SDL_Init(0) != 0) {
            std::cerr << "Even minimal SDL initialization failed: " << SDL_GetError() << std::endl;
            return false;
        }
    }

    // Try creating a window with minimum flags
    sdlWindow = SDL_CreateWindow(windowTitle.c_str(), windowWidth, windowHeight, 0);
    if (sdlWindow == nullptr) {
        std::cerr << "Window could not be created! SDL_Error: " << SDL_GetError() << std::endl;
        SDL_Quit();
        return false;
    }

    // Try creating a basic renderer
    sdlRenderer = SDL_CreateRenderer(sdlWindow, NULL);
    if (sdlRenderer == nullptr) {
        std::cerr << "Renderer could not be created! SDL Error: " << SDL_GetError() << std::endl;
        std::cerr << "Will continue without a renderer for now." << std::endl;
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
