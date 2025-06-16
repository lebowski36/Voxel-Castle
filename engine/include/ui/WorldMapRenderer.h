#pragma once

#include <cstddef>
#include <vector>
#include <array>

namespace VoxelCastle {
    namespace World {
        class SeedWorldGenerator;
        class HybridGeologicalSimulator;
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

    // NEW: Step 1.4 - Water System Visualization Integration
    
    /**
     * @brief Enable/disable water system overlay visualization
     * @param showWaterFlow Show water flow direction arrows
     * @param showAquifers Show aquifer levels as color overlay
     * @param showRivers Highlight river/stream paths
     * @param showSprings Show spring location indicators
     */
    void setWaterVisualization(bool showWaterFlow = true, bool showAquifers = true, 
                              bool showRivers = true, bool showSprings = true);

    /**
     * @brief Get current water visualization settings
     */
    void getWaterVisualization(bool& showWaterFlow, bool& showAquifers, 
                              bool& showRivers, bool& showSprings) const;

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

    /**
     * @brief Get geological simulator from current generator (for snapshot navigation)
     */
    const VoxelCastle::World::HybridGeologicalSimulator* getGeologicalSimulator() const;

    /**
     * @brief Track elevation change magnitude for adaptive updates
     * @param newElevationData New elevation data to compare
     * @return Change magnitude (0.0 = no change, 1.0 = maximum change)
     */
    float calculateChangeMagnitude(const float* newElevationData) const;

    /**
     * @brief Check if update is needed based on adaptive thresholds
     * @param changeMagnitude Current change magnitude
     * @param timeSinceLastUpdate Time since last update in seconds
     * @return true if update should be performed
     */
    bool shouldUpdate(float changeMagnitude, float timeSinceLastUpdate) const;

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
    
    // World generator reference for snapshot access
    VoxelCastle::World::SeedWorldGenerator* currentGenerator_;  // Current generator (not owned)
    
    // Current visualization state
    VisualizationMode currentMode_;
    GenerationPhase currentPhase_;
    
    // Adaptive update system (NEW for 1.3)
    float* previousElevationData_;     // Previous elevation data for change tracking
    float changeThreshold_;            // Minimum change magnitude to trigger update (0.1%)
    float maxUpdateInterval_;          // Maximum time between updates (2.0 seconds)
    mutable float lastUpdateTime_;     // Time of last update
    mutable float accumulatedChange_;  // Accumulated change magnitude
    
    // Zoom and pan state
    float zoomLevel_;        // Current zoom level (1.0 = full world view)
    float centerX_;          // Center X coordinate in world space (0.0 to 1.0)
    float centerY_;          // Center Y coordinate in world space (0.0 to 1.0)
    float lastMouseX_;       // Last mouse position for drag calculation
    float lastMouseY_;       // Last mouse position for drag calculation
    bool isDragging_;        // Whether user is currently dragging the map
    float minZoom_;          // Minimum zoom level (1.0 = full world)
    float maxZoom_;          // Maximum zoom level (64.0 = very detailed)
    
    // NEW: Step 1.4 - Water System Visualization settings
    bool showWaterFlow_;     // Show water flow direction arrows
    bool showAquifers_;      // Show aquifer levels as color overlay  
    bool showRivers_;        // Highlight river/stream paths
    bool showSprings_;       // Show spring location indicators
    
    // Internal helper methods
    void cleanupResources();
    void generateElevationData(VoxelCastle::World::SeedWorldGenerator* generator, unsigned int seed);
    void generateTemperatureData(VoxelCastle::World::SeedWorldGenerator* generator, unsigned int seed);
    void generatePrecipitationData(VoxelCastle::World::SeedWorldGenerator* generator, unsigned int seed);
    
    // CRITICAL FIX: Seed-based terrain generation for immediate preview variation
    float generateSeedBasedTerrain(float worldX, float worldZ, unsigned int seed);
    
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
    
    // NEW: Step 1.4 - Water System Visualization methods
    void overlayWaterSystemFeatures(unsigned char* colorData, VoxelCastle::World::SeedWorldGenerator* generator);
    void drawWaterFlowArrows(unsigned char* colorData, VoxelCastle::World::SeedWorldGenerator* generator);
    void overlayAquiferLevels(unsigned char* colorData, VoxelCastle::World::SeedWorldGenerator* generator);
    void highlightRiverPaths(unsigned char* colorData, VoxelCastle::World::SeedWorldGenerator* generator);
    void drawSpringIndicators(unsigned char* colorData, VoxelCastle::World::SeedWorldGenerator* generator);
    
    // Helper method for drawing lines
    void drawLine(unsigned char* colorData, int x1, int y1, int x2, int y2, const std::array<unsigned char, 4>& color);
    
    // Frame counter for logging control
    mutable int renderCounter_;
};

} // namespace VoxelEngine::UI
