#include "platform/Window.h" 
#include <SDL3/SDL_hints.h> // For SDL_SetHint and SDL_HINT_VIDEO_DRIVER

#include "platform/fullscreen_debug.h" 
#include "rendering/debug_render_mode.h" 

#include <iostream> 
#include <string> 
// glad.h is included via Window.h
// SDL_opengl.h is included via Window.h

Window::Window(const std::string &title, int width, int height)
    : windowTitle(title), windowWidth(width), windowHeight(height), 
      sdlWindow(nullptr), glContext(nullptr), running(false), fullscreen(false) {
        // Constructor body - can be empty if all initialization is done in the initializer list
        // FS_LOG can be called here if FullscreenDebug is initialized before window creation,
        // otherwise, move logging to init()
      }

Window::~Window() {
    FS_LOG("Window destructor called.");
    cleanUp(); // Ensure cleanup is called
    FullscreenDebug::closeLog(); 
}

bool Window::init() {
    std::string projectRoot = "/home/system-x1/Projects/Voxel Castle/"; 
    FullscreenDebug::openLog(projectRoot); // Open log at the beginning of init
    FS_LOG("Window::init() called.");

    FS_LOG("Attempting to set video driver hint to X11.");
    // Use SDL_SetHint and check for success (in SDL3, it returns a bool directly)
    if (!SDL_SetHint(SDL_HINT_VIDEO_DRIVER, "x11")) { 
        FS_LOG("Warning: Failed to set SDL_HINT_VIDEO_DRIVER to x11. SDL_GetError(): " + std::string(SDL_GetError()));
    } else {
        FS_LOG("Successfully set SDL_HINT_VIDEO_DRIVER to x11.");
    }
    SDL_ClearError(); // Clear any error from SDL_SetHint

    // Initialize SDL_video and SDL_events
    if (!SDL_Init(SDL_INIT_VIDEO | SDL_INIT_EVENTS)) { 
        FS_LOG("SDL_Init failed: " + std::string(SDL_GetError()));
        std::cerr << "[ERROR] Failed to initialize SDL: " << SDL_GetError() << std::endl;
        return false;
    }
    std::cout << "[INFO] Initializing OpenGL context..." << std::endl;
    FS_LOG("SDL_Init(SDL_INIT_VIDEO | SDL_INIT_EVENTS) successful.");

    // Restore OpenGL 3.3 Core attributes
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 3);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 3);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);
    SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1); 
    SDL_GL_SetAttribute(SDL_GL_DEPTH_SIZE, 24);
    FS_LOG("OpenGL attributes set (requesting 3.3 Core).");

    // Check for errors after setting attributes
    const char* attributeError = SDL_GetError();
    if (attributeError && *attributeError != '\0') {
        FS_LOG("SDL_Error after setting GL attributes: " + std::string(attributeError));
        // Potentially treat as fatal or handle, for now, just log
        SDL_ClearError(); // Clear it so it doesn't interfere with subsequent error checks
    }

    // Add system-level debugging before window creation
    FS_LOG("System debugging: Checking environment...");
    char* displayName = getenv("DISPLAY");
    FS_LOG("DISPLAY environment variable: " + std::string(displayName ? displayName : "NULL"));
    
    FS_LOG("Creating window: " + windowTitle + " " + std::to_string(windowWidth) + "x" + std::to_string(windowHeight));
    sdlWindow = SDL_CreateWindow(windowTitle.c_str(), windowWidth, windowHeight, SDL_WINDOW_OPENGL | SDL_WINDOW_RESIZABLE);
    if (!sdlWindow) {
        FS_LOG("Failed to create SDL window: " + std::string(SDL_GetError()));
        std::cerr << "[ERROR] Failed to create SDL window: " << SDL_GetError() << std::endl;
        SDL_Quit(); 
        return false;
    }
    FS_LOG("SDL window created successfully. Window pointer: " + std::to_string(reinterpret_cast<uintptr_t>(sdlWindow)));
    
    // Verify window properties
    int actualWidth, actualHeight;
    SDL_GetWindowSize(sdlWindow, &actualWidth, &actualHeight);
    FS_LOG("Window logical size: " + std::to_string(actualWidth) + "x" + std::to_string(actualHeight));
    
    Uint32 windowFlags = SDL_GetWindowFlags(sdlWindow);
    FS_LOG("Window flags: " + std::to_string(windowFlags) + " (OpenGL: " + std::string((windowFlags & SDL_WINDOW_OPENGL) ? "yes" : "no") + ")");

    // Proceed with OpenGL context creation
    FS_LOG("Proceeding with OpenGL context creation...");
    
    glContext = SDL_GL_CreateContext(sdlWindow);
    if (!glContext) {
        std::string errorMsg = SDL_GetError();
        FS_LOG("Failed to create OpenGL context: " + (errorMsg.empty() ? "No SDL error message." : errorMsg));
        std::cerr << "[ERROR] Failed to create OpenGL context: " << (errorMsg.empty() ? "No SDL error message." : errorMsg) << std::endl;
        SDL_DestroyWindow(sdlWindow);
        sdlWindow = nullptr;
        SDL_Quit();
        return false;
    }
    FS_LOG("OpenGL context created successfully. Context pointer: " + std::to_string(reinterpret_cast<uintptr_t>(glContext)));

    // Query and log created context attributes
    int majorVersion, minorVersion, profileMask;
    SDL_GL_GetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, &majorVersion);
    SDL_GL_GetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, &minorVersion);
    SDL_GL_GetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, &profileMask);
    FS_LOG("Created OpenGL Context Attributes - Major: " + std::to_string(majorVersion) + 
           ", Minor: " + std::to_string(minorVersion) + 
           ", Profile: " + 
           ((profileMask == SDL_GL_CONTEXT_PROFILE_CORE) ? "Core" : 
            ((profileMask == SDL_GL_CONTEXT_PROFILE_COMPATIBILITY) ? "Compatibility" : 
             ((profileMask == SDL_GL_CONTEXT_PROFILE_ES) ? "ES" : "Unknown"))));

    // Make window visible and give it a moment to settle
    FS_LOG("Making window visible...");
    SDL_ShowWindow(sdlWindow);
    SDL_RaiseWindow(sdlWindow);
    SDL_Delay(100); // Give window manager time to process
    FS_LOG("Window made visible, proceeding with context activation...");

    FS_LOG("Attempting to make context current with window: " + std::to_string(reinterpret_cast<uintptr_t>(sdlWindow)) + " and context: " + std::to_string(reinterpret_cast<uintptr_t>(glContext)));
    
    // Verify window and context are valid before making current
    if (!sdlWindow) {
        FS_LOG("Critical: sdlWindow is NULL before SDL_GL_MakeCurrent!");
        std::cerr << "[ERROR] Critical: sdlWindow is NULL before SDL_GL_MakeCurrent!" << std::endl;
        return false;
    }
    if (!glContext) {
        FS_LOG("Critical: glContext is NULL before SDL_GL_MakeCurrent!");
        std::cerr << "[ERROR] Critical: glContext is NULL before SDL_GL_MakeCurrent!" << std::endl;
        return false;
    }
    
    SDL_ClearError(); // Clear any previous SDL errors
    
    bool makeCurrentResult = SDL_GL_MakeCurrent(sdlWindow, glContext);
    FS_LOG("SDL_GL_MakeCurrent returned: " + std::string(makeCurrentResult ? "true (success)" : "false (failure)"));
    
    if (!makeCurrentResult) {
        const char* sdlErrorCStr = SDL_GetError(); // Get error message C-string
        std::string errorMsg = sdlErrorCStr ? sdlErrorCStr : "Unknown error (SDL_GetError returned NULL)";
        FS_LOG("Failed to make OpenGL context current. Return code: " + std::to_string(makeCurrentResult) + ", Error: " + errorMsg);
        std::cerr << "[ERROR] Failed to make OpenGL context current. Return code: " << makeCurrentResult << ", Error: " << errorMsg << std::endl;
        
        // Try to get more detailed error information
        FS_LOG("Additional debugging: Window valid: " + std::string(sdlWindow ? "yes" : "no") + ", Context valid: " + std::string(glContext ? "yes" : "no"));
        
        // Try fallback approach - recreate with compatibility profile
        FS_LOG("Attempting fallback with compatibility profile...");
        SDL_GL_DestroyContext(glContext);
        
        // Set more compatible OpenGL attributes
        SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 3);
        SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 0);
        SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_COMPATIBILITY);
        SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);
        SDL_GL_SetAttribute(SDL_GL_DEPTH_SIZE, 24);
        
        glContext = SDL_GL_CreateContext(sdlWindow);
        if (!glContext) {
            FS_LOG("Fallback context creation failed: " + std::string(SDL_GetError()));
            SDL_DestroyWindow(sdlWindow);
            sdlWindow = nullptr;
            SDL_Quit();
            return false;
        }
        
        SDL_ClearError();
        makeCurrentResult = SDL_GL_MakeCurrent(sdlWindow, glContext);
        if (!makeCurrentResult) {
            FS_LOG("Fallback context make current also failed: " + std::string(SDL_GetError()));
            SDL_GL_DestroyContext(glContext);
            SDL_DestroyWindow(sdlWindow);
            sdlWindow = nullptr;
            glContext = nullptr;
            SDL_Quit();
            return false;
        } else {
            FS_LOG("Fallback compatibility context succeeded!");
        }
    }
    FS_LOG("OpenGL context made current.");

    if (!gladLoadGLLoader((GLADloadproc)SDL_GL_GetProcAddress)) {
        FS_LOG("Failed to initialize GLAD.");
        std::cerr << "[ERROR] Failed to initialize GLAD" << std::endl;
        SDL_GL_DestroyContext(glContext); 
        SDL_DestroyWindow(sdlWindow);
        sdlWindow = nullptr;
        glContext = nullptr;
        SDL_Quit();
        return false;
    }
    FS_LOG("GLAD initialized successfully.");
    FS_LOG("OpenGL Version: " + std::string(reinterpret_cast<const char*>(glGetString(GL_VERSION))));
    FS_LOG("GLSL Version: " + std::string(reinterpret_cast<const char*>(glGetString(GL_SHADING_LANGUAGE_VERSION))));
    FS_LOG("Vendor: " + std::string(reinterpret_cast<const char*>(glGetString(GL_VENDOR))));
    FS_LOG("Renderer: " + std::string(reinterpret_cast<const char*>(glGetString(GL_RENDERER))));

    // Use SDL_GetWindowSizeInPixels for initial drawable size
    SDL_GetWindowSizeInPixels(sdlWindow, &windowWidth, &windowHeight);
    FS_LOG("Initial drawable dimensions (SDL_GetWindowSizeInPixels): " + std::to_string(windowWidth) + "x" + std::to_string(windowHeight));
    
    glViewport(0, 0, windowWidth, windowHeight);
    FS_LOG("Initial glViewport set to: 0, 0, " + std::to_string(windowWidth) + ", " + std::to_string(windowHeight));
    
    glClearColor(0.1f, 0.1f, 0.15f, 1.0f); // Dark grey-blue
    FS_LOG("glClearColor set.");

    running = true;
    FS_LOG("Window initialized successfully. Running state: true.");
    return true;
}

void Window::handleEvents() {
    // Event handling is primarily managed by InputManager.
    // This function can be used for window-specific events if necessary in the future.
    // For example, handling SDL_WINDOWEVENT_RESIZED explicitly if not covered by game loop.
}

void Window::updateWindowSize() {
    if (!sdlWindow) return;
    
    // Get the actual current window size from SDL
    int currentWidth, currentHeight;
    SDL_GetWindowSize(sdlWindow, &currentWidth, &currentHeight);
    
    // Only update if the size has actually changed
    if (currentWidth != windowWidth || currentHeight != windowHeight) {
        std::cout << "[Window] Size updated from " << windowWidth << "x" << windowHeight 
                  << " to " << currentWidth << "x" << currentHeight << std::endl;
        
        windowWidth = currentWidth;
        windowHeight = currentHeight;
    }
}

void Window::update() {
    // Update internal window size to match actual SDL window size
    updateWindowSize();
    
    // Window-specific update logic (e.g., title updates, etc.)
    // Currently, no specific logic here.
}

void Window::render() {
    SDL_GL_SwapWindow(sdlWindow);
}

bool Window::toggleFullscreen() {
    FS_LOG("Window::toggleFullscreen() called. Current fullscreen state: " + std::string(fullscreen ? "true" : "false"));
    if (!sdlWindow) {
        FS_LOG("Cannot toggle fullscreen: Window is null");
        std::cerr << "Cannot toggle fullscreen: Window is null" << std::endl;
        return false;
    }
    
    // Debug: Check available display modes
    SDL_DisplayID displayID = SDL_GetDisplayForWindow(sdlWindow);
    FS_LOG("Current display ID: " + std::to_string(displayID));
    
    SDL_DisplayMode **modes = SDL_GetFullscreenDisplayModes(displayID, nullptr);
    if (modes) {
        FS_LOG("Available fullscreen display modes:");
        for (int i = 0; modes[i]; i++) {
            FS_LOG("  Mode " + std::to_string(i) + ": " + 
                   std::to_string(modes[i]->w) + "x" + std::to_string(modes[i]->h) + 
                   " @ " + std::to_string((int)modes[i]->refresh_rate) + "Hz");
        }
        SDL_free(modes);
    }
    
    int preToggleWidth, preToggleHeight;
    SDL_GetWindowSize(sdlWindow, &preToggleWidth, &preToggleHeight); 
    FS_LOG("Pre-toggle logical window dimensions: " + std::to_string(preToggleWidth) + "x" + std::to_string(preToggleHeight));

    fullscreen = !fullscreen;
    FS_LOG("Attempting to set fullscreen to: " + std::string(fullscreen ? "true" : "false"));
    
    if (fullscreen) {
        // Get the desktop display mode (native resolution)
        const SDL_DisplayMode *desktopMode = SDL_GetDesktopDisplayMode(displayID);
        if (desktopMode) {
            FS_LOG("Desktop mode: " + std::to_string(desktopMode->w) + "x" + std::to_string(desktopMode->h) + 
                   " @ " + std::to_string((int)desktopMode->refresh_rate) + "Hz");
            
            // Create a display mode structure for fullscreen
            SDL_DisplayMode fullscreenMode = *desktopMode;
            FS_LOG("Setting fullscreen mode to: " + std::to_string(fullscreenMode.w) + "x" + std::to_string(fullscreenMode.h));
            
            // Set the fullscreen mode explicitly
            if (!SDL_SetWindowFullscreenMode(sdlWindow, &fullscreenMode)) {
                FS_LOG("SDL_SetWindowFullscreenMode failed: " + std::string(SDL_GetError()));
            } else {
                FS_LOG("SDL_SetWindowFullscreenMode successful");
            }
        }
    } else {
        // For windowed mode, clear the fullscreen mode
        if (!SDL_SetWindowFullscreenMode(sdlWindow, nullptr)) {
            FS_LOG("SDL_SetWindowFullscreenMode(clear) failed: " + std::string(SDL_GetError()));
        } else {
            FS_LOG("Cleared fullscreen mode for windowed");
        }
        
        // CRITICAL FIX: Explicitly restore the window to its original windowed dimensions
        const int WINDOWED_WIDTH = 1920;
        const int WINDOWED_HEIGHT = 1080;
        FS_LOG("Explicitly restoring window to initial dimensions: " + std::to_string(WINDOWED_WIDTH) + "x" + std::to_string(WINDOWED_HEIGHT));
        SDL_SetWindowSize(sdlWindow, WINDOWED_WIDTH, WINDOWED_HEIGHT);
    }
        
    // Use SDL_WINDOW_FULLSCREEN for exclusive fullscreen, 0 for windowed
    Uint32 fullscreenFlag = fullscreen ? SDL_WINDOW_FULLSCREEN : 0;
    FS_LOG("Using SDL fullscreen flag: " + std::to_string(fullscreenFlag));
    
    if (!SDL_SetWindowFullscreen(sdlWindow, fullscreenFlag)) {
        FS_LOG("SDL_SetWindowFullscreen failed: " + std::string(SDL_GetError()));
        std::cerr << "Failed to set SDL_WindowFullscreen state: " << SDL_GetError() << std::endl;
        fullscreen = !fullscreen; 
        return false;
    }
    FS_LOG("SDL_SetWindowFullscreen successful.");
    
    // It's often good to add a small delay to let the window manager and display driver settle after a mode change.
    // This can prevent issues where subsequent calls to get window/drawable size return stale values.
    SDL_Delay(100); // 100ms delay
    FS_LOG("SDL_Delay(100) after SDL_SetWindowFullscreen.");

    // Debug: Check multiple ways to get window size
    int logicalWidth, logicalHeight;
    SDL_GetWindowSize(sdlWindow, &logicalWidth, &logicalHeight);
    FS_LOG("Post-toggle SDL_GetWindowSize (logical): " + std::to_string(logicalWidth) + "x" + std::to_string(logicalHeight));

    int newDrawableWidth, newDrawableHeight;
    // Use SDL_GetWindowSizeInPixels instead of SDL_GL_GetDrawableSize
    SDL_GetWindowSizeInPixels(sdlWindow, &newDrawableWidth, &newDrawableHeight);
    FS_LOG("SDL_GetWindowSizeInPixels reported: " + std::to_string(newDrawableWidth) + "x" + std::to_string(newDrawableHeight));
    
    // CRITICAL FIX: SDL3 on Linux reports wrong size after fullscreen. Query OpenGL directly!
    GLint viewport[4];
    glGetIntegerv(GL_VIEWPORT, viewport);
    FS_LOG("Current OpenGL viewport: " + std::to_string(viewport[0]) + ", " + std::to_string(viewport[1]) + 
           ", " + std::to_string(viewport[2]) + ", " + std::to_string(viewport[3]));
    
    // Query the actual OpenGL framebuffer size (this should be correct even if SDL lies)
    GLint framebufferWidth, framebufferHeight;
    glGetIntegerv(GL_FRAMEBUFFER_BINDING, &framebufferWidth); // Get current framebuffer
    glGetIntegerv(GL_VIEWPORT, viewport); // Get current viewport again
    
    // In fullscreen mode, trust the desktop mode dimensions instead of SDL
    if (fullscreen) {
        const SDL_DisplayMode *desktopMode = SDL_GetDesktopDisplayMode(displayID);
        if (desktopMode) {
            FS_LOG("OVERRIDE: Using desktop mode dimensions instead of SDL reported size");
            newDrawableWidth = desktopMode->w;
            newDrawableHeight = desktopMode->h;
            FS_LOG("Corrected drawable size to: " + std::to_string(newDrawableWidth) + "x" + std::to_string(newDrawableHeight));
        }
    } else {
        // CRITICAL FIX: In windowed mode, use the fixed window size we set earlier
        const int WINDOWED_WIDTH = 1920;
        const int WINDOWED_HEIGHT = 1080;
        FS_LOG("OVERRIDE: Using fixed windowed dimensions: " + std::to_string(WINDOWED_WIDTH) + "x" + std::to_string(WINDOWED_HEIGHT));
        newDrawableWidth = WINDOWED_WIDTH;
        newDrawableHeight = WINDOWED_HEIGHT;
    }
    
    // Also try getting drawable size for comparison (removed SDL_GL_GetDrawableSize as it doesn't exist in SDL3)
    FS_LOG("Using drawable size: " + std::to_string(newDrawableWidth) + "x" + std::to_string(newDrawableHeight));
    
    // Check actual window flags
    Uint32 windowFlags = SDL_GetWindowFlags(sdlWindow);
    FS_LOG("Window flags after toggle: " + std::to_string(windowFlags) + 
           " (FULLSCREEN=" + std::to_string(SDL_WINDOW_FULLSCREEN) + ")");
    
    windowWidth = newDrawableWidth; 
    windowHeight = newDrawableHeight;
    FS_LOG("Internal window dimensions updated to (drawable size): " + std::to_string(windowWidth) + "x" + std::to_string(windowHeight));
        
    glViewport(0, 0, windowWidth, windowHeight);
    FS_LOG("glViewport called with: 0, 0, " + std::to_string(windowWidth) + ", " + std::to_string(windowHeight));
    
    GLint viewportParams[4];
    glGetIntegerv(GL_VIEWPORT, viewportParams);
    FS_LOG("glGetIntegerv(GL_VIEWPORT) returned: " + 
           std::to_string(viewportParams[0]) + ", " + 
           std::to_string(viewportParams[1]) + ", " + 
           std::to_string(viewportParams[2]) + ", " + 
           std::to_string(viewportParams[3]));

    return true;
}

bool Window::isFullscreen() const {
    // FS_LOG("Window::isFullscreen() called, returning: " + std::string(fullscreen ? "true" : "false")); // Too noisy for frequent calls
    return fullscreen;
}

void Window::cleanUp() {
    FS_LOG("Window::cleanUp() called.");
    if (glContext) {
        SDL_GL_DestroyContext(glContext); 
        FS_LOG("SDL_GL_DestroyContext called.");
        glContext = nullptr; 
    }
    if (sdlWindow) {
        SDL_DestroyWindow(sdlWindow);
        FS_LOG("SDL_Window destroyed.");
        sdlWindow = nullptr;
    }
    SDL_Quit();
    FS_LOG("SDL_Quit() called.");
    // Note: FullscreenDebug::closeLog() is in the destructor to ensure it's the last thing.
}

// Getter implementations
int Window::getWidth() const {
    return windowWidth;
}

int Window::getHeight() const {
    return windowHeight;
}

SDL_Window* Window::getSDLWindow() const { // Added const to match declaration
    return sdlWindow;
}

bool Window::isRunning() const {
    return running;
}

// Ensure there's a final newline if this is the end of the file.
