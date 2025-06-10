#pragma once

#include <cstddef>
#include <vector>

namespace VoxelCastle {
    namespace World {
        class SeedWorldGenerator;
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
    bool isReady() const { return textureValid_; }

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
    // OpenGL texture management
    unsigned int worldTexture_;
    unsigned int vertexArray_;
    unsigned int vertexBuffer_;
    
    // World map data
    float* elevationData_;
    float* temperatureData_;
    float* precipitationData_;
    int resolution_;
    bool textureValid_;
    float worldSizeKm_;  // World size in kilometers for proper scale sampling
    
    // Current visualization state
    VisualizationMode currentMode_;
    GenerationPhase currentPhase_;
    
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
    
    // Frame counter for logging control
    mutable int renderCounter_;
};

} // namespace VoxelEngine::UI
