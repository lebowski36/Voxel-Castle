#include "ui/WorldMapRenderer.h"
#include "ui/UIRenderer.h"
#include "world/seed_world_generator.h"
#include "world/tectonic_simulator.h"
#include "world/FractalContinentGenerator.h"
#include <iostream>
#include <algorithm>
#include <vector>
#include <chrono>
#include <glad/glad.h>

namespace VoxelEngine::UI {

WorldMapRenderer::WorldMapRenderer()
    : textureA_(0)
    , textureB_(0)
    , useTextureA_(true)
    , vertexArray_(0)
    , vertexBuffer_(0)
    , elevationData_(nullptr)
    , temperatureData_(nullptr)
    , precipitationData_(nullptr)
    , resolution_(0)
    , worldSizeKm_(1024.0f)  // Default to 1024km (continent scale)
    , currentMode_(VisualizationMode::ELEVATION)
    , currentPhase_(GenerationPhase::TECTONICS)
    , renderCounter_(0)
    , zoomLevel_(1.0f)       // Start with full world view
    , centerX_(0.5f)         // Center the view
    , centerY_(0.5f)         // Center the view
    , lastMouseX_(0.0f)
    , lastMouseY_(0.0f)
    , isDragging_(false)
    , minZoom_(1.0f)         // Minimum zoom shows full world
    , maxZoom_(64.0f) {      // Maximum zoom shows 1/64th of world (very detailed)
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
                                       unsigned int worldSeed,
                                       float worldSizeKm) {
    if (!generator || !elevationData_) {
        std::cout << "[WorldMapRenderer] Error: Generator or data arrays not available" << std::endl;
        return;
    }
    
    // Check if we need to regenerate based on changed parameters
    static GenerationPhase lastPhase = GenerationPhase::TECTONICS;
    static VisualizationMode lastMode = VisualizationMode::ELEVATION;
    static unsigned int lastSeed = 0;
    static float lastWorldSize = 0.0f;
    
    bool needsRegeneration = (phase != lastPhase) || 
                            (mode != lastMode) || 
                            (worldSeed != lastSeed) || 
                            (worldSizeKm != lastWorldSize) ||
                            (textureA_ == 0 && textureB_ == 0);  // No textures exist yet
    
    // Throttle texture regeneration to prevent excessive updates, but allow important changes
    static auto lastGenerationTime = std::chrono::steady_clock::now();
    auto currentTime = std::chrono::steady_clock::now();
    auto timeSinceGeneration = std::chrono::duration_cast<std::chrono::milliseconds>(currentTime - lastGenerationTime).count();
    
    // Only skip if it's too frequent AND nothing important has changed
    if (!needsRegeneration && timeSinceGeneration < 500) {
        std::cout << "[WorldMapRenderer] Skipping regeneration - no changes needed (last: " << timeSinceGeneration << "ms ago)" << std::endl;
        return;
    }
    
    lastGenerationTime = currentTime;
    lastPhase = phase;
    lastMode = mode;
    lastSeed = worldSeed;
    lastWorldSize = worldSizeKm;

    // Store the world size for proper sampling
    worldSizeKm_ = worldSizeKm;
    
    currentPhase_ = phase;
    currentMode_ = mode;
    
    std::cout << "[WorldMapRenderer] Generating world map - Phase: " << getGenerationPhaseName(phase) 
              << ", Mode: " << getVisualizationModeName(mode) << std::endl;
    std::cout << "[WorldMapRenderer] World size: " << worldSizeKm_ << "km x " << worldSizeKm_ << "km" << std::endl;
    
    // Generate data based on current phase - make visualization dynamic
    switch (phase) {
        case GenerationPhase::TECTONICS:
            // Pure tectonic terrain - dramatic geological features
            generateElevationData(generator, worldSeed);
            break;
        case GenerationPhase::EROSION:
            // Apply erosion effects to existing terrain
            generateElevationData(generator, worldSeed);
            applyErosionEffects();
            break;
        case GenerationPhase::HYDROLOGY:
            // Generate water features and modify terrain for rivers/lakes
            generateElevationData(generator, worldSeed);
            applyErosionEffects();
            generateWaterFeatures();
            break;
        case GenerationPhase::CLIMATE:
            // Focus on temperature and precipitation
            generateElevationData(generator, worldSeed);
            applyErosionEffects();
            generateWaterFeatures();
            generateTemperatureData(generator, worldSeed);
            generatePrecipitationData(generator, worldSeed);
            break;
        case GenerationPhase::BIOMES:
            // Generate biome-based terrain modifications
            generateElevationData(generator, worldSeed);
            applyErosionEffects();
            generateWaterFeatures();
            generateTemperatureData(generator, worldSeed);
            generatePrecipitationData(generator, worldSeed);
            break;
        case GenerationPhase::CIVILIZATION:
            // Final terrain with structure placement areas
            generateElevationData(generator, worldSeed);
            applyErosionEffects();
            generateWaterFeatures();
            generateTemperatureData(generator, worldSeed);
            generatePrecipitationData(generator, worldSeed);
            break;
        case GenerationPhase::COMPLETE:
            // Final integrated view - all features combined
            generateElevationData(generator, worldSeed);
            applyErosionEffects();
            generateWaterFeatures();
            generateTemperatureData(generator, worldSeed);
            generatePrecipitationData(generator, worldSeed);
            break;
    }
    
    // Handle additional visualization modes
    if (mode == VisualizationMode::TEMPERATURE && phase != GenerationPhase::CLIMATE) {
        generateTemperatureData(generator, worldSeed);
    }
    
    if (mode == VisualizationMode::PRECIPITATION && phase != GenerationPhase::CLIMATE) {
        generatePrecipitationData(generator, worldSeed);
    }
    
    // Convert world data to color texture
    int dataSize = resolution_ * resolution_ * 4; // RGBA
    unsigned char* colorData = new unsigned char[dataSize];
    worldDataToColorTexture(colorData);
    
    // OVERLAY 3.0 World Generation System fractal continental features
    // This adds visual markers for continental plates, rivers, and mountain ridges
    overlayFractalContinentalFeatures(colorData, generator);
    
    // Create OpenGL texture
    createTextureFromColorData(colorData, resolution_);
    
    delete[] colorData;
    
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
    unsigned int currentTexture = useTextureA_ ? textureA_ : textureB_;
    
    // Validate that the texture is still valid in OpenGL
    bool textureValid = (currentTexture != 0) && glIsTexture(currentTexture);
    
    if (!textureValid) {
        if (shouldLog) {
            std::cout << "[WorldMapRenderer] Rendering placeholder - texture ID " << currentTexture 
                      << " is invalid (exists=" << (currentTexture != 0) 
                      << ", glValid=" << (currentTexture != 0 ? glIsTexture(currentTexture) : false) << ")" << std::endl;
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
        std::cout << "[WorldMapRenderer] Rendering world texture ID " << currentTexture 
                  << " (using texture " << (useTextureA_ ? "A" : "B") << ")" << std::endl;
    }
    
    // Verify texture is still valid before rendering
    if (!glIsTexture(currentTexture)) {
        std::cerr << "[WorldMapRenderer] Error: Texture ID " << currentTexture << " became invalid during rendering!" << std::endl;
        return;
    }
    
    // Use UIRenderer's textured quad rendering
    renderer->renderTexturedQuad(x, y, width, height, currentTexture);
}

void WorldMapRenderer::generateElevationData(VoxelCastle::World::SeedWorldGenerator* generator, unsigned int seed) {
    // Suppress unused parameter warning for now - seed will be used when full terrain generation is implemented
    (void)seed;
    
    std::cout << "[WorldMapRenderer] Generating elevation data..." << std::endl;
    
    // Use the actual world size passed in - this represents the entire world being generated
    // The world size should be stored from the generateWorldMap call
    float worldSize = worldSizeKm_ * 1000.0f; // Convert from kilometers to meters
    
    std::cout << "[WorldMapRenderer] Sampling world area: " << worldSizeKm_ << "km x " << worldSizeKm_ << "km" << std::endl;
    std::cout << "[WorldMapRenderer] World size in meters: " << worldSize << "m x " << worldSize << "m" << std::endl;
    
    // Get geological simulator for new 3.0 World Generation System
    const VoxelCastle::World::GeologicalSimulator* geologicalSim = generator->getGeologicalSimulator();
    
    // Fallback to tectonic simulator if geological simulation is not available
    const VoxelCastle::TectonicSimulator* tectonicSim = generator->getTectonicSimulator();
    
    std::cout << "[WorldMapRenderer] === 3.0 World Generation System Integration ===" << std::endl;
    std::cout << "[WorldMapRenderer] Geological simulator: " << (geologicalSim ? "Available (PRIMARY)" : "Not available") << std::endl;
    std::cout << "[WorldMapRenderer] Tectonic simulator: " << (tectonicSim ? "Available (FALLBACK)" : "Not available") << std::endl;
    if (tectonicSim) {
        std::cout << "[WorldMapRenderer] Tectonic simulation complete: " << (tectonicSim->IsSimulationComplete() ? "Yes" : "No") << std::endl;
        std::cout << "[WorldMapRenderer] Number of plates: " << tectonicSim->GetPlates().size() << std::endl;
    }
    
    // Prioritize geological simulator (3.0 World Gen) over legacy tectonic system
    bool usingNewSystem = geologicalSim != nullptr;
    std::cout << "[WorldMapRenderer] Using " << (usingNewSystem ? "3.0 World Generation System (Fractal Continental)" : "Legacy Tectonic System") << std::endl;
    
    for (int y = 0; y < resolution_; y++) {
        for (int x = 0; x < resolution_; x++) {
            // Apply zoom and pan to determine world coordinates
            // Calculate the world area being viewed based on zoom level
            float viewAreaSize = worldSize / zoomLevel_;  // Size of area being viewed
            
            // Calculate the corner of the viewed area
            float viewStartX = (centerX_ - 0.5f / zoomLevel_) * worldSize;
            float viewStartY = (centerY_ - 0.5f / zoomLevel_) * worldSize;
            
            // Clamp to world bounds
            viewStartX = std::max(0.0f, std::min(viewStartX, worldSize - viewAreaSize));
            viewStartY = std::max(0.0f, std::min(viewStartY, worldSize - viewAreaSize));
            
            // Convert screen coordinates to world coordinates within the zoomed view
            float worldX = viewStartX + (x / (float)resolution_) * viewAreaSize;
            float worldZ = viewStartY + (y / (float)resolution_) * viewAreaSize;
            
            // Ensure coordinates stay within world bounds
            worldX = std::max(0.0f, std::min(worldX, worldSize - 1.0f));
            worldZ = std::max(0.0f, std::min(worldZ, worldSize - 1.0f));
            
            // Start with base terrain height
            int heightVoxels = generator->getTerrainHeightAt((int)worldX, (int)worldZ);
            float baseHeight = heightVoxels * 0.25f; // Convert voxels to meters (25cm per voxel)
            
            // PRIMARY: Use new 3.0 World Generation System (Fractal Continental Foundation)
            float finalHeight = baseHeight;
            if (geologicalSim) {
                // Use new geological simulation system with fractal continental generation
                VoxelCastle::World::GeologicalSample sample = geologicalSim->getSampleAt(worldX, worldZ);
                finalHeight = sample.elevation;
                
                // Debug: Log fractal continental features for first few pixels
                if (x < 3 && y < 3) {
                    std::cout << "[WorldMapRenderer] 3.0 WorldGen Sample - Pos(" << worldX << "," << worldZ 
                              << ") Elevation:" << sample.elevation << "m RockType:" << (int)sample.rockType 
                              << " Hardness:" << sample.rockHardness << std::endl;
                    
                    // Check for continental vs oceanic features
                    if (sample.elevation > 0) {
                        std::cout << "[WorldMapRenderer] CONTINENTAL feature detected" << std::endl;
                    } else if (sample.elevation < -200) {
                        std::cout << "[WorldMapRenderer] DEEP OCEAN basin detected" << std::endl;
                    } else {
                        std::cout << "[WorldMapRenderer] COASTAL/SHELF feature detected" << std::endl;
                    }
                }
                
                // Clamp elevation to expanded World Gen 3.0 range (±2048m as per spec)
                finalHeight = std::clamp(finalHeight, -2048.0f, 2048.0f);
                
            } else if (tectonicSim && tectonicSim->IsSimulationComplete()) {
                // FALLBACK: Use legacy tectonic system
                // Convert coordinates to kilometers for tectonic simulator
                glm::vec2 worldPosKm(worldX / 1000.0f, worldZ / 1000.0f);
                
                // Debug: Check world bounds and sampling pattern
                float tectonicWorldSize = tectonicSim->GetWorldSize();
                if (x < 5 && y < 5) {
                    std::cout << "[WorldMapRenderer] Tectonic world size: " << tectonicWorldSize << " km" << std::endl;
                    std::cout << "[WorldMapRenderer] Sampling position (" << worldPosKm.x << "," << worldPosKm.y << ") km" << std::endl;
                    std::cout << "[WorldMapRenderer] World bounds: 0 to " << tectonicWorldSize << " km" << std::endl;
                    std::cout << "[WorldMapRenderer] Pixel (" << x << "," << y << ") = World (" << worldX << "," << worldZ << ") m" << std::endl;
                    std::cout << "[WorldMapRenderer] Within bounds: " << (worldPosKm.x >= 0 && worldPosKm.y >= 0 && worldPosKm.x < tectonicWorldSize && worldPosKm.y < tectonicWorldSize) << std::endl;
                }
                
                // Get tectonic elevation modifier (in meters)
                float tectonicModifier = tectonicSim->GetElevationModifier(worldPosKm);
                
                // Apply tectonic effects to terrain
                finalHeight = baseHeight + tectonicModifier;
                
                // Debug: log first few values to see if tectonic data is working
                if (x < 5 && y < 5) {
                    std::cout << "[WorldMapRenderer] Pos(" << worldX << "," << worldZ 
                              << ") Base:" << baseHeight << "m Tectonic:" << tectonicModifier 
                              << "m Final:" << finalHeight << "m" << std::endl;
                }
                
                // Clamp elevation to geological simulation range (±1800m)
                finalHeight = std::clamp(finalHeight, -1800.0f, 1800.0f);
            } else {
                // Debug: log that we're using base terrain only
                if (x < 3 && y < 3) {
                    std::cout << "[WorldMapRenderer] Using base terrain only - Pos(" << worldX << "," << worldZ 
                              << ") Height:" << baseHeight << "m" << std::endl;
                }
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

void WorldMapRenderer::applyErosionEffects() {
    if (!elevationData_) return;
    
    std::cout << "[WorldMapRenderer] Applying erosion effects..." << std::endl;
    
    // Apply erosion smoothing - reduce peak heights and fill valleys slightly
    int dataSize = resolution_ * resolution_;
    float* tempData = new float[dataSize];
    
    // Copy original data
    for (int i = 0; i < dataSize; i++) {
        tempData[i] = elevationData_[i];
    }
    
    // Apply erosion: smooth terrain by averaging with neighbors and reducing extremes
    for (int y = 1; y < resolution_ - 1; y++) {
        for (int x = 1; x < resolution_ - 1; x++) {
            int index = y * resolution_ + x;
            
            // Get neighboring heights
            float center = tempData[index];
            float neighbors = tempData[(y-1) * resolution_ + x] +     // North
                             tempData[(y+1) * resolution_ + x] +     // South  
                             tempData[y * resolution_ + (x-1)] +     // West
                             tempData[y * resolution_ + (x+1)];      // East
            
            float avgNeighbor = neighbors / 4.0f;
            
            // Erosion effect: blend with neighbors and reduce extreme heights
            float erosionFactor = 0.15f; // 15% erosion effect
            float erodedHeight = center * (1.0f - erosionFactor) + avgNeighbor * erosionFactor;
            
            // Additional height reduction for very high peaks (simulating weathering)
            if (erodedHeight > 600.0f) {
                float peakReduction = (erodedHeight - 600.0f) * 0.1f; // 10% reduction above 600m
                erodedHeight -= peakReduction;
            }
            
            elevationData_[index] = erodedHeight;
        }
    }
    
    delete[] tempData;
    
    // Find new height range after erosion
    float minHeight = elevationData_[0];
    float maxHeight = elevationData_[0];
    for (int i = 1; i < dataSize; i++) {
        minHeight = std::min(minHeight, elevationData_[i]);
        maxHeight = std::max(maxHeight, elevationData_[i]);
    }
    
    std::cout << "[WorldMapRenderer] Erosion complete - new range: " << minHeight << "m to " << maxHeight << "m" << std::endl;
}

void WorldMapRenderer::generateWaterFeatures() {
    if (!elevationData_) return;
    
    std::cout << "[WorldMapRenderer] Generating water features..." << std::endl;
    
    // Create lakes and rivers by lowering elevation in certain areas
    int dataSize = resolution_ * resolution_;
    
    // Find the lowest 15% of terrain for potential water features
    std::vector<std::pair<float, int>> heightIndices;
    for (int i = 0; i < dataSize; i++) {
        heightIndices.push_back({elevationData_[i], i});
    }
    
    std::sort(heightIndices.begin(), heightIndices.end());
    
    // Convert the lowest 10% of areas to water bodies (lakes/rivers)
    int waterPoints = static_cast<int>(dataSize * 0.10f);
    
    for (int i = 0; i < waterPoints; i++) {
        int index = heightIndices[i].second;
        float currentHeight = heightIndices[i].first;
        
        // Lower these areas to create water bodies
        // Create significant depressions (50-150m below original)
        float waterDepth = 50.0f + (i % 100); // Varying depths 50-150m
        elevationData_[index] = currentHeight - waterDepth;
        
        // Also lower nearby areas to create natural water boundaries
        int y = index / resolution_;
        int x = index % resolution_;
        
        for (int dy = -1; dy <= 1; dy++) {
            for (int dx = -1; dx <= 1; dx++) {
                int ny = y + dy;
                int nx = x + dx;
                if (ny >= 0 && ny < resolution_ && nx >= 0 && nx < resolution_) {
                    int neighborIndex = ny * resolution_ + nx;
                    float neighborDepth = waterDepth * 0.3f; // Gentler slope around water
                    elevationData_[neighborIndex] = std::min(elevationData_[neighborIndex], 
                                                           currentHeight - neighborDepth);
                }
            }
        }
    }
    
    // Find new height range after water feature generation
    float minHeight = elevationData_[0];
    float maxHeight = elevationData_[0];
    for (int i = 0; i < dataSize; i++) {
        minHeight = std::min(minHeight, elevationData_[i]);
        maxHeight = std::max(maxHeight, elevationData_[i]);
    }
    
    std::cout << "[WorldMapRenderer] Water features complete - new range: " << minHeight << "m to " << maxHeight << "m" << std::endl;
}

void WorldMapRenderer::generateTemperatureData(VoxelCastle::World::SeedWorldGenerator* generator, unsigned int seed) {
    // Suppress unused parameter warnings for now - phase will be used for phase-specific color tinting
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
    // Updated elevation color scheme for 3.0 World Generation System (±2048m range)
    // Designed to highlight fractal continental features, coastlines, and ocean basins
    
    if (heightMeters < -1800.0f) {
        // Abyssal ocean trenches (deepest 250m of range)
        r = 0; g = 0; b = 60; // Very dark navy blue
    } else if (heightMeters < -1200.0f) {
        // Deep ocean basins (continental margin)
        r = 0; g = 0; b = 100; // Dark blue
    } else if (heightMeters < -600.0f) {
        // Ocean basins (typical deep ocean)
        r = 0; g = 30; b = 140; // Ocean blue
    } else if (heightMeters < -200.0f) {
        // Continental shelf edge / slope
        r = 0; g = 60; b = 180; // Medium blue
    } else if (heightMeters < -50.0f) {
        // Continental shelf (shallow ocean)
        r = 20; g = 100; b = 220; // Lighter blue
    } else if (heightMeters < 0.0f) {
        // Coastal waters / tidal zones
        r = 80; g = 140; b = 255; // Light blue
    } else if (heightMeters < 10.0f) {
        // Immediate coastline / beaches
        r = 255; g = 218; b = 185; // Sand color
    } else if (heightMeters < 100.0f) {
        // Coastal plains / river deltas
        r = 144; g = 238; b = 144; // Light green
    } else if (heightMeters < 300.0f) {
        // Lowlands / river valleys
        r = 34; g = 139; b = 34; // Forest green
    } else if (heightMeters < 600.0f) {
        // Rolling hills / plateaus
        r = 107; g = 142; b = 35; // Olive drab
    } else if (heightMeters < 1000.0f) {
        // Lower mountain ranges
        r = 139; g = 101; b = 54; // Brown
    } else if (heightMeters < 1400.0f) {
        // Higher mountains
        r = 160; g = 140; b = 120; // Light brown
    } else if (heightMeters < 1800.0f) {
        // High peaks (snow line)
        r = 200; g = 200; b = 200; // Light gray
    } else {
        // Extreme mountain peaks (1800m-2048m range)
        r = 255; g = 250; b = 250; // Snow white
    }
    
    // Apply phase-based color tinting for 3.0 World Generation System
    switch (phase) {
        case GenerationPhase::TECTONICS:
            // Emphasize blue tones for fractal continental foundation phase
            b = std::min(255, (int)(b * 1.3f));
            // Enhance contrast for continental vs oceanic features
            if (heightMeters > 0) {
                r = std::min(255, (int)(r * 1.1f)); // Enhance continental features
            }
            break;
        case GenerationPhase::EROSION:
            // Emphasize brown/earth tones for erosion effects
            r = std::min(255, (int)(r * 1.1f));
            g = std::min(255, (int)(g * 0.9f));
            break;
        case GenerationPhase::HYDROLOGY:
            // Emphasize cyan/water tones for river networks
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
    // Initialize textures if this is the first time
    if (textureA_ == 0 && textureB_ == 0) {
        // Create both textures initially
        glGenTextures(1, &textureA_);
        glGenTextures(1, &textureB_);
        
        // Initialize both textures with basic parameters
        for (GLuint texture : {textureA_, textureB_}) {
            glBindTexture(GL_TEXTURE_2D, texture);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
            
            // Allocate texture storage (empty initially)
            glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, resolution, resolution, 0, GL_RGBA, GL_UNSIGNED_BYTE, nullptr);
        }
        
        glBindTexture(GL_TEXTURE_2D, 0);
        std::cout << "[WorldMapRenderer] Initialized double-buffered textures A:" << textureA_ << " B:" << textureB_ << std::endl;
    }
    
    // Update the inactive texture (the one NOT currently being displayed)
    GLuint updateTexture = useTextureA_ ? textureB_ : textureA_;
    
    // Check for OpenGL errors before proceeding
    GLenum error = glGetError();
    if (error != GL_NO_ERROR) {
        std::cerr << "[WorldMapRenderer] OpenGL error before texture update: " << error << std::endl;
        return;
    }
    
    // Verify texture is valid
    GLboolean isTexture = glIsTexture(updateTexture);
    if (!isTexture) {
        std::cerr << "[WorldMapRenderer] Error: Texture ID " << updateTexture << " is not a valid texture!" << std::endl;
        return;
    }
    
    // Bind and update the inactive texture
    glBindTexture(GL_TEXTURE_2D, updateTexture);
    glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, resolution, resolution, GL_RGBA, GL_UNSIGNED_BYTE, colorData);
    
    error = glGetError();
    if (error != GL_NO_ERROR) {
        std::cerr << "[WorldMapRenderer] Error updating texture data: " << error << std::endl;
        glBindTexture(GL_TEXTURE_2D, 0);
        return;
    }
    
    // Unbind texture
    glBindTexture(GL_TEXTURE_2D, 0);
    
    // Atomic swap: flip which texture is displayed (no texture deletion needed!)
    useTextureA_ = !useTextureA_;
    
    GLuint currentTexture = useTextureA_ ? textureA_ : textureB_;
    std::cout << "[WorldMapRenderer] Updated texture buffer, now displaying ID " << currentTexture 
              << " (swapped from " << updateTexture << ") with resolution " << resolution << "x" << resolution << std::endl;
}

void WorldMapRenderer::cleanupResources() {
    if (textureA_ != 0) {
        glDeleteTextures(1, &textureA_);
        textureA_ = 0;
    }
    
    if (textureB_ != 0) {
        glDeleteTextures(1, &textureB_);
        textureB_ = 0;
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
    
    // Note: Textures are now persistent, no need to invalidate
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

void WorldMapRenderer::setViewport(float zoomLevel, float centerX, float centerY) {
    zoomLevel_ = std::clamp(zoomLevel, minZoom_, maxZoom_);
    centerX_ = std::clamp(centerX, 0.0f, 1.0f);
    centerY_ = std::clamp(centerY, 0.0f, 1.0f);
    
    // Adjust center to prevent viewing outside world bounds
    float halfViewSize = 0.5f / zoomLevel_;
    centerX_ = std::clamp(centerX_, halfViewSize, 1.0f - halfViewSize);
    centerY_ = std::clamp(centerY_, halfViewSize, 1.0f - halfViewSize);
    
    // Note: With double-buffered textures, regeneration is handled automatically
}

bool WorldMapRenderer::handleMouseInput(int mouseX, int mouseY, int mapWidth, int mapHeight, 
                                       float wheelDelta, bool isMouseDown) {
    bool needsRegeneration = false;
    
    // Convert mouse coordinates to normalized map coordinates (0.0 to 1.0)
    float normalizedX = (float)mouseX / mapWidth;
    float normalizedY = (float)mouseY / mapHeight;
    
    // Handle zoom with mouse wheel
    if (wheelDelta != 0.0f) {
        float oldZoom = zoomLevel_;
        float zoomFactor = 1.0f + (wheelDelta * 0.1f); // 10% zoom per wheel step
        float newZoom = zoomLevel_ * zoomFactor;
        newZoom = std::clamp(newZoom, minZoom_, maxZoom_);
        
        if (std::abs(newZoom - oldZoom) > 0.01f) {
            // Zoom towards mouse cursor position
            float worldX = centerX_ + (normalizedX - 0.5f) / oldZoom;
            float worldY = centerY_ + (normalizedY - 0.5f) / oldZoom;
            
            zoomLevel_ = newZoom;
            
            // Adjust center to keep the point under mouse cursor stable
            centerX_ = worldX - (normalizedX - 0.5f) / newZoom;
            centerY_ = worldY - (normalizedY - 0.5f) / newZoom;
            
            // Apply viewport constraints
            setViewport(zoomLevel_, centerX_, centerY_);
            needsRegeneration = true;
        }
    }
    
    // Handle panning with mouse drag
    if (isMouseDown) {
        if (isDragging_) {
            // Calculate drag delta
            float deltaX = (normalizedX - lastMouseX_) / zoomLevel_;
            float deltaY = (normalizedY - lastMouseY_) / zoomLevel_;
            
            // Update center position (inverted because we're moving the world, not the viewport)
            float newCenterX = centerX_ - deltaX;
            float newCenterY = centerY_ - deltaY;
            
            setViewport(zoomLevel_, newCenterX, newCenterY);
            needsRegeneration = true;
        } else {
            isDragging_ = true;
        }
        
        lastMouseX_ = normalizedX;
        lastMouseY_ = normalizedY;
    } else {
        isDragging_ = false;
    }
    
    return needsRegeneration;
}

void WorldMapRenderer::getViewportInfo(float& outZoomLevel, float& outCenterX, float& outCenterY, 
                                      const char*& outScale) const {
    outZoomLevel = zoomLevel_;
    outCenterX = centerX_;
    outCenterY = centerY_;
    
    // Calculate scale description based on zoom level
    float kmPerPixel = (worldSizeKm_ / zoomLevel_) / resolution_;
    
    if (kmPerPixel >= 10.0f) {
        outScale = "Continental Scale";
    } else if (kmPerPixel >= 1.0f) {
        outScale = "Regional Scale";
    } else if (kmPerPixel >= 0.1f) {
        outScale = "Local Scale";
    } else {
        outScale = "Detailed Scale";
    }
}

// 3.0 World Generation System - Fractal Continental Feature Visualization
void WorldMapRenderer::overlayFractalContinentalFeatures(unsigned char* colorData, VoxelCastle::World::SeedWorldGenerator* generator) {
    if (!generator) return;
    
    const VoxelCastle::World::GeologicalSimulator* geologicalSim = generator->getGeologicalSimulator();
    if (!geologicalSim) return;
    
    // Get fractal continental generator from geological simulator
    const VoxelCastle::World::FractalContinentGenerator* fractalGen = geologicalSim->getFractalContinentGenerator();
    if (!fractalGen) return;
    
    std::cout << "[WorldMapRenderer] Overlaying fractal continental features..." << std::endl;
    
    // Overlay continental plates (subtle boundary highlighting)
    drawContinentalPlates(colorData, fractalGen->GetContinentalPlates());
    
    // Overlay ocean basins (depth enhancement)
    drawOceanBasins(colorData, fractalGen->GetOceanBasins());
    
    // Overlay river templates (blue pathways)
    drawRiverTemplates(colorData, fractalGen->GetRiverTemplates());
    
    // Overlay mountain ridges (elevation highlighting)
    drawMountainRidges(colorData, fractalGen->GetMountainRidges());
}

void WorldMapRenderer::drawContinentalPlates(unsigned char* colorData, const std::vector<VoxelCastle::World::ContinentalPlate>& plates) {
    // Draw subtle boundaries around continental plates
    for (const auto& plate : plates) {
        // Convert world coordinates to pixel coordinates
        int plateX = (int)((plate.center.x / (worldSizeKm_ * 1000.0f)) * resolution_);
        int plateY = (int)((plate.center.y / (worldSizeKm_ * 1000.0f)) * resolution_);
        
        // Draw continental plate center marker (small cross)
        if (plateX >= 2 && plateX < resolution_ - 2 && plateY >= 2 && plateY < resolution_ - 2) {
            for (int dx = -2; dx <= 2; dx++) {
                for (int dy = -2; dy <= 2; dy++) {
                    if ((dx == 0 || dy == 0) && (plateX + dx >= 0) && (plateX + dx < resolution_) && 
                        (plateY + dy >= 0) && (plateY + dy < resolution_)) {
                        int idx = ((plateY + dy) * resolution_ + (plateX + dx)) * 4;
                        // Highlight continental centers with white markers
                        colorData[idx] = 255;     // R
                        colorData[idx + 1] = 255; // G  
                        colorData[idx + 2] = 255; // B
                        // Keep alpha as is
                    }
                }
            }
        }
    }
}

void WorldMapRenderer::drawOceanBasins(unsigned char* colorData, const std::vector<VoxelCastle::World::OceanBasin>& basins) {
    // Enhance ocean basin visualization (already handled by elevation coloring)
    // This could add special features like ridge highlighting in the future
    for (const auto& basin : basins) {
        // Ocean basins are primarily defined by their depth in the elevation field
        // Future: Could add mid-ocean ridge visualization here
        (void)basin; // Suppress unused parameter warning for now
    }
}

void WorldMapRenderer::drawRiverTemplates(unsigned char* colorData, const std::vector<VoxelCastle::World::RiverTemplate>& rivers) {
    // Draw river pathways as blue lines
    for (const auto& river : rivers) {
        // Draw main stem
        for (size_t i = 0; i < river.mainStem.size(); i++) {
            glm::vec2 point = river.mainStem[i];
            int riverX = (int)((point.x / (worldSizeKm_ * 1000.0f)) * resolution_);
            int riverY = (int)((point.y / (worldSizeKm_ * 1000.0f)) * resolution_);
            
            if (riverX >= 0 && riverX < resolution_ && riverY >= 0 && riverY < resolution_) {
                int idx = (riverY * resolution_ + riverX) * 4;
                // Draw river as bright blue line
                colorData[idx] = 0;       // R
                colorData[idx + 1] = 150; // G
                colorData[idx + 2] = 255; // B
                // Keep alpha as is
            }
        }
        
        // Draw tributaries
        for (const auto& tributary : river.tributaries) {
            for (const auto& point : tributary) {
                int riverX = (int)((point.x / (worldSizeKm_ * 1000.0f)) * resolution_);
                int riverY = (int)((point.y / (worldSizeKm_ * 1000.0f)) * resolution_);
                
                if (riverX >= 0 && riverX < resolution_ && riverY >= 0 && riverY < resolution_) {
                    int idx = (riverY * resolution_ + riverX) * 4;
                    // Draw tributaries as lighter blue
                    colorData[idx] = 50;      // R
                    colorData[idx + 1] = 180; // G
                    colorData[idx + 2] = 255; // B
                    // Keep alpha as is
                }
            }
        }
    }
}

void WorldMapRenderer::drawMountainRidges(unsigned char* colorData, const std::vector<VoxelCastle::World::MountainRidge>& ridges) {
    // Draw mountain ridges as elevated pathways
    for (const auto& ridge : ridges) {
        for (const auto& point : ridge.ridgeLine) {
            int ridgeX = (int)((point.x / (worldSizeKm_ * 1000.0f)) * resolution_);
            int ridgeY = (int)((point.y / (worldSizeKm_ * 1000.0f)) * resolution_);
            
            if (ridgeX >= 0 && ridgeX < resolution_ && ridgeY >= 0 && ridgeY < resolution_) {
                int idx = (ridgeY * resolution_ + ridgeX) * 4;
                // Enhance mountain ridges with brighter colors
                colorData[idx] = std::min(255, (int)(colorData[idx] * 1.3f));     // R (enhance existing)
                colorData[idx + 1] = std::min(255, (int)(colorData[idx + 1] * 1.2f)); // G
                colorData[idx + 2] = std::min(255, (int)(colorData[idx + 2] * 1.1f)); // B
                // Keep alpha as is
            }
        }
    }
}

} // namespace VoxelEngine::UI
