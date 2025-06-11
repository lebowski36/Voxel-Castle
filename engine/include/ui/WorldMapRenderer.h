#pragma once

#include <cstddef>
#include <vector>

namespace VoxelCastle {
    namespace World {
        class SeedWorldGenerator;
        struct ContinentalPlate;
        struct OceanBasin;
        struct RiverTemplate;
        struct MountainRidge;
    }
}

namespace VoxelEngine {
    namespace UI {
        class UIRenderer;
    }
}

namespace VoxelEngine::UI {

/**
 * @brief World map renderer for WorldSimulationUI
 * 
 * Renders real-time world generation visualization with multiple data layers
 * following the Dwarf Fortress-style approach. Supports different visualization
 * modes (elevation, temperature, precipitation, biomes, etc.) and progressive
 * detail updates during generation.
 */
class WorldMapRenderer {
public:
    // Visualization modes matching WorldSimulationUI
    enum class VisualizationMode {
        ELEVATION,
        TEMPERATURE,
        PRECIPITATION,
        BIOMES,
        HYDROLOGY,
        GEOLOGY
    };

    // Generation phases for color-coded visualization
    enum class GenerationPhase {
        TECTONICS,      // Blue tones - elevation, rock types
        EROSION,        // Brown/Green tones - valleys, sediment
        HYDROLOGY,      // Cyan/Blue tones - rivers, lakes, watersheds
        CLIMATE,        // Temperature/precipitation gradients
        BIOMES,         // Multi-colored biome assignment
        CIVILIZATION,   // Historical overlay
        COMPLETE        // Final integrated view
    };

    WorldMapRenderer();
    ~WorldMapRenderer();

    /**
     * @brief Initialize the world map renderer
     * @param resolution Map resolution (e.g., 512x512 for detailed view)
     * @return true if initialization succeeded
     */
    bool initialize(int resolution = 512);

    /**
     * @brief Generate world map data from the generator
     * @param generator Source world generator
     * @param phase Current generation phase for color coding
     * @param mode Visualization mode to display
     * @param worldSeed Seed for reproducible generation
     * @param worldSizeKm World size in kilometers (default 1024km for continent-scale)
     */
    void generateWorldMap(VoxelCastle::World::SeedWorldGenerator* generator,
                         GenerationPhase phase,
                         VisualizationMode mode,
                         unsigned int worldSeed,
                         float worldSizeKm = 1024.0f);

    /**
     * @brief Render the world map at specified screen coordinates
     * @param renderer UI renderer for drawing
     * @param x Screen X coordinate
     * @param y Screen Y coordinate
     * @param width Display width
     * @param height Display height
     */
    void render(UIRenderer* renderer, int x, int y, int width, int height);

    /**
     * @brief Check if map data is ready for rendering
     * @return true if valid texture data exists
     */
    bool isReady() const { return (textureA_ != 0 && textureB_ != 0); }

    /**
     * @brief Set visualization mode for next generation
     * @param mode New visualization mode
     */
    void setVisualizationMode(VisualizationMode mode) { currentMode_ = mode; }

    /**
     * @brief Set generation phase for color coding
     * @param phase Current generation phase
     */
    void setGenerationPhase(GenerationPhase phase) { currentPhase_ = phase; }

    /**
     * @brief Set the zoom level and center position for detailed viewing
     * @param zoomLevel Zoom factor (1.0 = full world, 2.0 = 2x zoom, etc.)
     * @param centerX Center X coordinate in world space (0.0 to 1.0)
     * @param centerY Center Y coordinate in world space (0.0 to 1.0)
     */
    void setViewport(float zoomLevel, float centerX, float centerY);

    /**
     * @brief Handle mouse input for interactive zoom and pan
     * @param mouseX Mouse X coordinate relative to map area
     * @param mouseY Mouse Y coordinate relative to map area
     * @param mapWidth Width of the map display area
     * @param mapHeight Height of the map display area
     * @param wheelDelta Mouse wheel delta for zooming
     * @param isMouseDown Whether mouse button is pressed for panning
     * @return true if map needs to be regenerated due to significant zoom change
     */
    bool handleMouseInput(int mouseX, int mouseY, int mapWidth, int mapHeight, 
                         float wheelDelta, bool isMouseDown);

    /**
     * @brief Get current zoom information for UI display
     * @param outZoomLevel Current zoom level
     * @param outCenterX Current center X (0.0 to 1.0)
     * @param outCenterY Current center Y (0.0 to 1.0)
     * @param outScale Scale description string
     */
    void getViewportInfo(float& outZoomLevel, float& outCenterX, float& outCenterY, 
                        const char*& outScale) const;

    /**
     * @brief Get display name for visualization mode
     * @param mode Visualization mode
     * @return Human-readable name
     */
    static const char* getVisualizationModeName(VisualizationMode mode);

    /**
     * @brief Get display name for generation phase
     * @param phase Generation phase
     * @return Human-readable name
     */
    static const char* getGenerationPhaseName(GenerationPhase phase);

private:
    // OpenGL resources - Double-buffered textures
    unsigned int textureA_;      // First texture buffer
    unsigned int textureB_;      // Second texture buffer  
    bool useTextureA_;           // Which texture is currently displayed
    unsigned int vertexArray_;
    unsigned int vertexBuffer_;
    
    // World map data
    float* elevationData_;
    float* temperatureData_;
    float* precipitationData_;
    int resolution_;
    float worldSizeKm_;  // World size in kilometers for proper scale sampling
    
    // Current visualization state
    VisualizationMode currentMode_;
    GenerationPhase currentPhase_;
    
    // Zoom and pan state
    float zoomLevel_;        // Current zoom level (1.0 = full world view)
    float centerX_;          // Center X coordinate in world space (0.0 to 1.0)
    float centerY_;          // Center Y coordinate in world space (0.0 to 1.0)
    float lastMouseX_;       // Last mouse position for drag calculation
    float lastMouseY_;       // Last mouse position for drag calculation
    bool isDragging_;        // Whether user is currently dragging the map
    float minZoom_;          // Minimum zoom level (1.0 = full world)
    float maxZoom_;          // Maximum zoom level (64.0 = very detailed)
    
    // Internal helper methods
    void cleanupResources();
    void generateElevationData(VoxelCastle::World::SeedWorldGenerator* generator, unsigned int seed);
    void generateTemperatureData(VoxelCastle::World::SeedWorldGenerator* generator, unsigned int seed);
    void generatePrecipitationData(VoxelCastle::World::SeedWorldGenerator* generator, unsigned int seed);
    
    // Phase-specific terrain modification methods for dynamic visualization
    void applyErosionEffects();
    void generateWaterFeatures();
    
    void worldDataToColorTexture(unsigned char* colorData);
    void createTextureFromColorData(const unsigned char* colorData, int resolution);
    
    // Color mapping functions for different visualization modes
    void elevationToColor(float heightMeters, GenerationPhase phase, unsigned char& r, unsigned char& g, unsigned char& b);
    void temperatureToColor(float tempCelsius, GenerationPhase phase, unsigned char& r, unsigned char& g, unsigned char& b);
    void precipitationToColor(float precMmYear, GenerationPhase phase, unsigned char& r, unsigned char& g, unsigned char& b);
    
    // 3.0 World Generation System visualization helpers
    void overlayFractalContinentalFeatures(unsigned char* colorData, VoxelCastle::World::SeedWorldGenerator* generator);
    void drawContinentalPlates(unsigned char* colorData, const std::vector<VoxelCastle::World::ContinentalPlate>& plates);
    void drawOceanBasins(unsigned char* colorData, const std::vector<VoxelCastle::World::OceanBasin>& basins);
    void drawRiverTemplates(unsigned char* colorData, const std::vector<VoxelCastle::World::RiverTemplate>& rivers);
    void drawMountainRidges(unsigned char* colorData, const std::vector<VoxelCastle::World::MountainRidge>& ridges);
    
    // Frame counter for logging control
    mutable int renderCounter_;
};

} // namespace VoxelEngine::UI
