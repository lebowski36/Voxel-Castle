#include "SDL3/SDL.h"
#include <iostream>

int main(int argc, char *argv[]) {
    // Initialize SDL with just video subsystem
    if (SDL_Init(SDL_INIT_VIDEO) != 0) {
        std::cerr << "SDL_Init Error: " << SDL_GetError() << std::endl;
        std::cerr << "Additional debug information: This could be due to missing display server support, driver issues, or permissions." << std::endl;
        return 1;
    }
    
    // Create a minimal window
    SDL_Window *win = SDL_CreateWindow("SDL3 Test", 640, 480, 0);
    if (win == nullptr) {
        std::cerr << "SDL_CreateWindow Error: " << SDL_GetError() << std::endl;
        SDL_Quit();
        return 1;
    }
    
    // Wait for 2 seconds
    SDL_Delay(2000);
    
    // Clean up
    SDL_DestroyWindow(win);
    SDL_Quit();
    
    return 0;
}
