#include "platform/Window.h" // Adjust path as necessary based on include directories
#include <iostream>

// GLM Headers - Will be used by other engine systems, keep includes for now if generally useful
#define GLM_FORCE_SILENT_WARNINGS // Optional: To suppress GLM warnings if any
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

int main(int argc, char* argv[]) {
    Window gameWindow("Voxel Fortress - Alpha", 800, 600);

    if (!gameWindow.init()) {
        std::cerr << "Failed to initialize the game window!" << std::endl;
        return -1;
    }

    while (gameWindow.isRunning()) {
        gameWindow.handleEvents();
        gameWindow.update();
        gameWindow.render();
    }

    // cleanUp is called by Window destructor
    return 0;
}
