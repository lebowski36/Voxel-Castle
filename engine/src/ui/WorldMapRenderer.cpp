#include "ui/WorldMapRenderer.h"
#include "ui/UIRenderer.h"
#include "world/seed_world_generator.h"
#include "world/tectonic_simulator.h"
#include <iostream>
#include <algorithm>
#include <glad/glad.h>

namespace VoxelEngine::UI {

WorldMapRenderer::WorldMapRenderer()
    : worldTexture_(0)
    , vertexArray_(0)
    , vertexBuffer_(0)
    , elevationData_(nullptr)
    , temperatureData_(nullptr)
    , precipitationData_(nullptr)
    , resolution_(0)
    , textureValid_(false)
    , currentMode_(VisualizationMode::ELEVATION)
    , currentPhase_(GenerationPhase::TECTONICS)
    , renderCounter_(0) {
}

WorldMapRenderer::~WorldMapRenderer() {
    cleanupResources();
}

bool WorldMapRenderer::initialize(int resolution) {
    resolution_ = resolution;
    
    // Allocate data arrays
    int dataSize = resolution * resolution;
    elevationData_ = new float[dataSize];
    temperatureData_ = new float[dataSize];
    precipitationData_ = new float[dataSize];
    
    // Initialize with default values
    for (int i = 0; i < dataSize; i++) {
        elevationData_[i] = 0.0f;
        temperatureData_[i] = 15.0f;     // 15°C default
        precipitationData_[i] = 800.0f;  // 800mm/year default
    }
    
    std::cout << "[WorldMapRenderer] Initialized with resolution " << resolution << "x" << resolution << std::endl;
    return true;
}

void WorldMapRenderer::generateWorldMap(VoxelCastle::World::SeedWorldGenerator* generator,
                                       GenerationPhase phase,
                                       VisualizationMode mode,
                                       unsigned int worldSeed) {
    if (!generator || !elevationData_) {
        std::cout << "[WorldMapRenderer] Error: Generator or data arrays not available" << std::endl;
        return;
    }
    
    currentPhase_ = phase;
    currentMode_ = mode;
    
    std::cout << "[WorldMapRenderer] Generating world map - Phase: " << getGenerationPhaseName(phase) 
              << ", Mode: " << getVisualizationModeName(mode) << std::endl;
    
    // Generate data based on current phase and requirements
    if (phase == GenerationPhase::TECTONICS || phase == GenerationPhase::EROSION || 
        mode == VisualizationMode::ELEVATION) {
        generateElevationData(generator, worldSeed);
    }
    
    if (phase == GenerationPhase::CLIMATE || mode == VisualizationMode::TEMPERATURE) {
        generateTemperatureData(generator, worldSeed);
    }
    
    if (phase == GenerationPhase::CLIMATE || mode == VisualizationMode::PRECIPITATION) {
        generatePrecipitationData(generator, worldSeed);
    }
    
    // Convert world data to color texture
    int dataSize = resolution_ * resolution_ * 4; // RGBA
    unsigned char* colorData = new unsigned char[dataSize];
    worldDataToColorTexture(colorData);
    
    // Create OpenGL texture
    createTextureFromColorData(colorData, resolution_);
    
    delete[] colorData;
    textureValid_ = true;
    
    std::cout << "[WorldMapRenderer] World map generation complete" << std::endl;
}

void WorldMapRenderer::render(UIRenderer* renderer, int x, int y, int width, int height) {
    // Conditional logging to reduce console spam
    bool shouldLog = (renderCounter_++ % 300 == 0); // Log every 300 frames (~5 seconds at 60fps)
    
    if (shouldLog) {
        std::cout << "[WorldMapRenderer] Rendering world map at (" << x << ", " << y 
                  << ") size " << width << "x" << height << std::endl;
        std::cout << "[WorldMapRenderer] Mode: " << getVisualizationModeName(currentMode_)
                  << ", Phase: " << getGenerationPhaseName(currentPhase_) << std::endl;
    }
    
    // Check if we have valid world map data to render
    if (!textureValid_ || worldTexture_ == 0) {
        if (shouldLog) {
            std::cout << "[WorldMapRenderer] Rendering placeholder - no valid world data" << std::endl;
        }
        
        // Render placeholder with phase-appropriate color
        glm::vec4 placeholderColor;
        switch (currentPhase_) {
            case GenerationPhase::TECTONICS:
                placeholderColor = {0.2f, 0.2f, 0.4f, 1.0f}; // Blue tones
                break;
            case GenerationPhase::EROSION:
                placeholderColor = {0.3f, 0.2f, 0.1f, 1.0f}; // Brown tones
                break;
            case GenerationPhase::HYDROLOGY:
                placeholderColor = {0.1f, 0.3f, 0.4f, 1.0f}; // Cyan tones
                break;
            case GenerationPhase::CLIMATE:
                placeholderColor = {0.2f, 0.3f, 0.2f, 1.0f}; // Green tones
                break;
            case GenerationPhase::BIOMES:
                placeholderColor = {0.3f, 0.3f, 0.2f, 1.0f}; // Multi-colored
                break;
            default:
                placeholderColor = {0.3f, 0.3f, 0.3f, 1.0f}; // Gray
                break;
        }
        
        renderer->renderColoredQuad(x, y, width, height, placeholderColor);
        
        // Add text to show current generation state
        if (renderer->isTextRendererAvailable()) {
            std::string statusText = std::string("Generating ") + getGenerationPhaseName(currentPhase_) + "...";
            renderer->drawText(statusText, x + 10, y + height - 30, 0.9f, glm::vec3(1.0f, 1.0f, 1.0f));
            
            std::string modeText = std::string("View: ") + getVisualizationModeName(currentMode_);
            renderer->drawText(modeText, x + 10, y + height - 50, 0.8f, glm::vec3(0.9f, 0.9f, 0.9f));
        }
        return;
    }
    
    // Render the actual world map texture
    if (shouldLog) {
        std::cout << "[WorldMapRenderer] Rendering world texture ID " << worldTexture_ << std::endl;
    }
    
    // Use UIRenderer's textured quad rendering
    renderer->renderTexturedQuad(x, y, width, height, worldTexture_);
}

void WorldMapRenderer::generateElevationData(VoxelCastle::World::SeedWorldGenerator* generator, unsigned int seed) {
    // Suppress unused parameter warning for now - seed will be used when full terrain generation is implemented
    (void)seed;
    
    std::cout << "[WorldMapRenderer] Generating elevation data..." << std::endl;
    
    // Sample elevation across the world map area
    // Use a reasonable world area - let's say 8km x 8km for good detail
    float worldSize = 8000.0f; // 8km in meters
    
    // Get tectonic simulator for elevation modifiers
    const VoxelCastle::TectonicSimulator* tectonicSim = generator->getTectonicSimulator();
    
    for (int y = 0; y < resolution_; y++) {
        for (int x = 0; x < resolution_; x++) {
            // Convert screen coordinates to world coordinates
            float worldX = (x / (float)resolution_) * worldSize;
            float worldZ = (y / (float)resolution_) * worldSize;
            
            // Start with base terrain height
            int heightVoxels = generator->getTerrainHeightAt((int)worldX, (int)worldZ);
            float baseHeight = heightVoxels * 0.25f; // Convert voxels to meters (25cm per voxel)
            
            // Apply tectonic elevation modifiers if simulation is available
            float finalHeight = baseHeight;
            if (tectonicSim && tectonicSim->IsSimulationComplete()) {
                // Convert coordinates to kilometers for tectonic simulator
                glm::vec2 worldPosKm(worldX / 1000.0f, worldZ / 1000.0f);
                
                // Get tectonic elevation modifier (in meters)
                float tectonicModifier = tectonicSim->GetElevationModifier(worldPosKm);
                
                // Apply tectonic effects to terrain
                finalHeight = baseHeight + tectonicModifier;
                
                // Ensure realistic height bounds (prevent negative elevations for now)
                finalHeight = std::max(0.0f, finalHeight);
            }
            
            elevationData_[y * resolution_ + x] = finalHeight;
        }
    }
    
    // Find height range for debugging
    float minHeight = elevationData_[0];
    float maxHeight = elevationData_[0];
    
    for (int i = 1; i < resolution_ * resolution_; i++) {
        minHeight = std::min(minHeight, elevationData_[i]);
        maxHeight = std::max(maxHeight, elevationData_[i]);
    }
    
    std::cout << "[WorldMapRenderer] Elevation range: " << minHeight << "m to " << maxHeight << "m" << std::endl;
}

void WorldMapRenderer::generateTemperatureData(VoxelCastle::World::SeedWorldGenerator* generator, unsigned int seed) {
    // Suppress unused parameter warnings for now - these will be used when full climate simulation is implemented
    (void)generator;
    (void)seed;
    
    std::cout << "[WorldMapRenderer] Generating temperature data..." << std::endl;
    
    // For now, generate based on latitude and elevation
    // This is a placeholder until we have full climate simulation
    for (int y = 0; y < resolution_; y++) {
        for (int x = 0; x < resolution_; x++) {
            int index = y * resolution_ + x;
            
            // Base temperature decreases with latitude (from equator)
            float latitude = (y / (float)resolution_) * 180.0f - 90.0f; // -90 to +90
            float baseTemp = 30.0f - (abs(latitude) * 0.5f); // Hottest at equator
            
            // Temperature decreases with elevation
            float elevation = elevationData_[index];
            float elevationEffect = elevation * 0.0065f; // 6.5°C per 1000m lapse rate
            
            temperatureData_[index] = baseTemp - elevationEffect;
        }
    }
}

void WorldMapRenderer::generatePrecipitationData(VoxelCastle::World::SeedWorldGenerator* generator, unsigned int seed) {
    // Suppress unused parameter warnings for now - these will be used when full climate simulation is implemented
    (void)generator;
    (void)seed;
    
    std::cout << "[WorldMapRenderer] Generating precipitation data..." << std::endl;
    
    // For now, generate based on latitude and elevation
    // This is a placeholder until we have full climate simulation
    for (int y = 0; y < resolution_; y++) {
        for (int x = 0; x < resolution_; x++) {
            int index = y * resolution_ + x;
            
            // More precipitation near equator and in mountains
            float latitude = (y / (float)resolution_) * 180.0f - 90.0f;
            float basePrecip = 1500.0f - (abs(latitude) * 10.0f); // More at equator
            
            // Orographic precipitation - more in mountains
            float elevation = elevationData_[index];
            float elevationEffect = std::min(elevation * 2.0f, 800.0f); // Cap at 800mm bonus
            
            precipitationData_[index] = std::max(basePrecip + elevationEffect, 100.0f); // Min 100mm
        }
    }
}

void WorldMapRenderer::worldDataToColorTexture(unsigned char* colorData) {
    for (int i = 0; i < resolution_ * resolution_; i++) {
        unsigned char r, g, b;
        
        // Choose color mapping based on current visualization mode
        switch (currentMode_) {
            case VisualizationMode::ELEVATION:
                elevationToColor(elevationData_[i], currentPhase_, r, g, b);
                break;
            case VisualizationMode::TEMPERATURE:
                temperatureToColor(temperatureData_[i], currentPhase_, r, g, b);
                break;
            case VisualizationMode::PRECIPITATION:
                precipitationToColor(precipitationData_[i], currentPhase_, r, g, b);
                break;
            default:
                // Default to elevation
                elevationToColor(elevationData_[i], currentPhase_, r, g, b);
                break;
        }
        
        // Store in RGBA format
        colorData[i * 4 + 0] = r; // Red
        colorData[i * 4 + 1] = g; // Green  
        colorData[i * 4 + 2] = b; // Blue
        colorData[i * 4 + 3] = 255; // Alpha (fully opaque)
    }
}

void WorldMapRenderer::elevationToColor(float heightMeters, GenerationPhase phase, unsigned char& r, unsigned char& g, unsigned char& b) {
    // Base elevation color scheme (reused from WorldPreviewRenderer backup)
    if (heightMeters < -200.0f) {
        // Deep ocean
        r = 0; g = 0; b = 139; // Dark blue
    } else if (heightMeters < -50.0f) {
        // Ocean
        r = 0; g = 100; b = 200; // Medium blue
    } else if (heightMeters < 0.0f) {
        // Shallow water  
        r = 100; g = 150; b = 255; // Light blue
    } else if (heightMeters < 5.0f) {
        // Beach/coastline
        r = 238; g = 203; b = 173; // Tan
    } else if (heightMeters < 100.0f) {
        // Lowlands/plains
        r = 34; g = 139; b = 34; // Forest green
    } else if (heightMeters < 300.0f) {
        // Hills
        r = 107; g = 142; b = 35; // Olive drab
    } else if (heightMeters < 600.0f) {
        // Mountains
        r = 139; g = 101; b = 54; // Brown
    } else {
        // High peaks
        r = 255; g = 250; b = 250; // Snow white
    }
    
    // Apply phase-based color tinting
    switch (phase) {
        case GenerationPhase::TECTONICS:
            // Emphasize blue tones for geological phase
            b = std::min(255, (int)(b * 1.2f));
            break;
        case GenerationPhase::EROSION:
            // Emphasize brown/earth tones
            r = std::min(255, (int)(r * 1.1f));
            g = std::min(255, (int)(g * 0.9f));
            break;
        case GenerationPhase::HYDROLOGY:
            // Emphasize cyan/water tones
            g = std::min(255, (int)(g * 1.1f));
            b = std::min(255, (int)(b * 1.1f));
            break;
        default:
            // No tinting for other phases
            break;
    }
}

void WorldMapRenderer::temperatureToColor(float tempCelsius, GenerationPhase phase, unsigned char& r, unsigned char& g, unsigned char& b) {
    // Suppress unused parameter warning for now - phase will be used for phase-specific color tinting
    (void)phase;
    
    // Temperature color scale: Blue (cold) to Red (hot)
    if (tempCelsius < -20.0f) {
        r = 0; g = 0; b = 255; // Deep blue (arctic)
    } else if (tempCelsius < 0.0f) {
        r = 100; g = 150; b = 255; // Light blue (cold)
    } else if (tempCelsius < 15.0f) {
        r = 0; g = 255; b = 100; // Green (temperate)
    } else if (tempCelsius < 25.0f) {
        r = 255; g = 255; b = 0; // Yellow (warm)
    } else if (tempCelsius < 35.0f) {
        r = 255; g = 150; b = 0; // Orange (hot)
    } else {
        r = 255; g = 0; b = 0; // Red (very hot)
    }
}

void WorldMapRenderer::precipitationToColor(float precMmYear, GenerationPhase phase, unsigned char& r, unsigned char& g, unsigned char& b) {
    // Suppress unused parameter warning for now - phase will be used for phase-specific color tinting
    (void)phase;
    
    // Precipitation color scale: Brown (dry) to Blue (wet)
    if (precMmYear < 200.0f) {
        r = 139; g = 69; b = 19; // Brown (desert)
    } else if (precMmYear < 500.0f) {
        r = 255; g = 165; b = 0; // Orange (semi-arid)
    } else if (precMmYear < 1000.0f) {
        r = 255; g = 255; b = 0; // Yellow (moderate)
    } else if (precMmYear < 1500.0f) {
        r = 0; g = 255; b = 0; // Green (humid)
    } else if (precMmYear < 2000.0f) {
        r = 0; g = 150; b = 255; // Light blue (wet)
    } else {
        r = 0; g = 0; b = 255; // Deep blue (very wet)
    }
}

void WorldMapRenderer::createTextureFromColorData(const unsigned char* colorData, int resolution) {
    // Delete existing texture if present
    if (worldTexture_ != 0) {
        glDeleteTextures(1, &worldTexture_);
        worldTexture_ = 0;
    }
    
    // Generate new texture
    glGenTextures(1, &worldTexture_);
    glBindTexture(GL_TEXTURE_2D, worldTexture_);
    
    // Set texture parameters
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    
    // Upload texture data
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, resolution, resolution, 0, GL_RGBA, GL_UNSIGNED_BYTE, colorData);
    
    // Unbind texture
    glBindTexture(GL_TEXTURE_2D, 0);
    
    std::cout << "[WorldMapRenderer] Created texture ID " << worldTexture_ 
              << " with resolution " << resolution << "x" << resolution << std::endl;
}

void WorldMapRenderer::cleanupResources() {
    if (worldTexture_ != 0) {
        glDeleteTextures(1, &worldTexture_);
        worldTexture_ = 0;
    }
    
    if (vertexBuffer_ != 0) {
        glDeleteBuffers(1, &vertexBuffer_);
        vertexBuffer_ = 0;
    }
    
    if (vertexArray_ != 0) {
        glDeleteVertexArrays(1, &vertexArray_);
        vertexArray_ = 0;
    }
    
    if (elevationData_) {
        delete[] elevationData_;
        elevationData_ = nullptr;
    }
    
    if (temperatureData_) {
        delete[] temperatureData_;
        temperatureData_ = nullptr;
    }
    
    if (precipitationData_) {
        delete[] precipitationData_;
        precipitationData_ = nullptr;
    }
    
    textureValid_ = false;
}

const char* WorldMapRenderer::getVisualizationModeName(VisualizationMode mode) {
    switch (mode) {
        case VisualizationMode::ELEVATION: return "Elevation";
        case VisualizationMode::TEMPERATURE: return "Temperature";
        case VisualizationMode::PRECIPITATION: return "Precipitation";
        case VisualizationMode::BIOMES: return "Biomes";
        case VisualizationMode::HYDROLOGY: return "Rivers & Lakes";
        case VisualizationMode::GEOLOGY: return "Geology";
        default: return "Unknown";
    }
}

const char* WorldMapRenderer::getGenerationPhaseName(GenerationPhase phase) {
    switch (phase) {
        case GenerationPhase::TECTONICS: return "Tectonic Formation";
        case GenerationPhase::EROSION: return "Erosion & Weathering";
        case GenerationPhase::HYDROLOGY: return "Water Systems";
        case GenerationPhase::CLIMATE: return "Climate Simulation";
        case GenerationPhase::BIOMES: return "Biome Assignment";
        case GenerationPhase::CIVILIZATION: return "Civilization History";
        case GenerationPhase::COMPLETE: return "World Complete";
        default: return "Unknown Phase";
    }
}

} // namespace VoxelEngine::UI
