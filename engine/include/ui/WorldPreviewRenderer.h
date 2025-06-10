#pragma once

#include <glad/glad.h>
#include <memory>

// Forward declarations
namespace VoxelCastle::World {
    class SeedWorldGenerator;
    class WorldSeed;
    class WorldParameters;
}

namespace VoxelEngine::UI {
    class UIRenderer;
}

namespace VoxelEngine::UI {

/**
 * @brief Renders world preview visualizations for the World Configuration UI
 * 
 * This class handles OpenGL texture generation and rendering for world previews,
 * creating real-time heightmap visualizations that update as users change
 * world generation parameters.
 */
class WorldPreviewRenderer {
public:
    /**
     * @brief Constructor
     */
    WorldPreviewRenderer();
    
    /**
     * @brief Destructor - cleans up OpenGL resources
     */
    ~WorldPreviewRenderer();
    
    /**
     * @brief Initialize the preview renderer
     * @param previewResolution Size of the preview texture (resolution x resolution)
     * @return true if initialization succeeded, false otherwise
     */
    bool initialize(int previewResolution = 256);
    
    /**
     * @brief Generate a new world preview based on world generation parameters
     * @param worldSeed World seed to use for generation
     * @param worldParameters World parameters to use for generation  
     * @param centerX World center X coordinate for preview
     * @param centerZ World center Z coordinate for preview
     * @param sampleRadius Radius in world coordinates to preview
     */
    void generatePreview(std::shared_ptr<VoxelCastle::World::WorldSeed> worldSeed,
                        std::shared_ptr<VoxelCastle::World::WorldParameters> worldParameters,
                        int centerX = 0, int centerZ = 0, int sampleRadius = 1000);
    
    /**
     * @brief Render the preview texture to the screen using the provided UI renderer
     * @param renderer UI renderer to use for drawing
     * @param x Screen X position to render at
     * @param y Screen Y position to render at
     * @param width Width to render the preview
     * @param height Height to render the preview
     */
    void render(VoxelEngine::UI::UIRenderer* renderer, float x, float y, float width, float height);
    
    /**
     * @brief Check if a valid preview is available
     * @return true if preview texture is ready for rendering
     */
    bool hasValidPreview() const { return textureValid_; }
    
    /**
     * @brief Get the OpenGL texture ID for external rendering
     * @return OpenGL texture ID, or 0 if no valid texture
     */
    GLuint getTextureId() const { return previewTexture_; }

private:
    // OpenGL resources
    GLuint previewTexture_;
    GLuint previewFramebuffer_;
    GLuint vertexBuffer_;
    GLuint vertexArray_;
    
    // Preview generation state
    int resolution_;
    bool textureValid_;
    
    // Height data buffer for texture generation
    std::unique_ptr<float[]> heightData_;
    std::unique_ptr<unsigned char[]> colorData_;
    
    /**
     * @brief Convert height data to colored texture data
     * @param heightData Array of height values in meters
     * @param resolution Size of the height data array (resolution x resolution)
     * @param colorData Output array for RGBA color data (must be 4 * resolution * resolution size)
     */
    void heightmapToColorTexture(const float* heightData, int resolution, unsigned char* colorData);
    
    /**
     * @brief Create OpenGL texture from color data
     * @param colorData RGBA color data array
     * @param resolution Texture resolution (resolution x resolution)
     */
    void createTextureFromColorData(const unsigned char* colorData, int resolution);
    
    /**
     * @brief Setup OpenGL resources for texture rendering
     */
    void setupRenderingResources();
    
    /**
     * @brief Clean up OpenGL resources
     */
    void cleanup();
    
    /**
     * @brief Map height value to color using elevation-based color scheme
     * @param heightMeters Height in meters
     * @param r Output red component (0-255)
     * @param g Output green component (0-255)  
     * @param b Output blue component (0-255)
     */
    void heightToColor(float heightMeters, unsigned char& r, unsigned char& g, unsigned char& b);
};

} // namespace VoxelEngine::UI
