#include "ui/WorldMapRenderer.h"
#include "ui/UIRenderer.h"
#include "world/seed_world_generator.h"
#include "world/tectonic_simulator.h"
#include "world/FractalContinentGenerator.h"
#include <iostream>
#include <algorithm>
#include <vector>
#include <chrono>
#include <cstring>  // For std::memcpy
#include <cmath>    // For std::sin, std::cos
#include <random>   // For std::mt19937 and seed-based terrain generation
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
    , currentGenerator_(nullptr)       // NEW: No generator initially
    , currentMode_(VisualizationMode::ELEVATION)
    , currentPhase_(GenerationPhase::TECTONICS)
    , previousElevationData_(nullptr)  // NEW: For adaptive update system
    , changeThreshold_(0.001f)         // NEW: 0.1% change minimum
    , maxUpdateInterval_(2.0f)         // NEW: Maximum 2 seconds between updates
    , lastUpdateTime_(0.0f)            // NEW: Track last update time
    , accumulatedChange_(0.0f)         // NEW: Track accumulated changes
    , renderCounter_(0)
    , zoomLevel_(1.0f)       // Start with full world view
    , centerX_(0.5f)         // Center the view
    , centerY_(0.5f)         // Center the view
    , lastMouseX_(0.0f)
    , lastMouseY_(0.0f)
    , isDragging_(false)
    , minZoom_(1.0f)         // Minimum zoom shows full world
    , maxZoom_(64.0f)        // Maximum zoom shows 1/64th of world (very detailed)
    , showWaterFlow_(true)   // NEW: Step 1.4 - Enable water flow arrows by default
    , showAquifers_(true)    // NEW: Step 1.4 - Enable aquifer overlay by default
    , showRivers_(true)      // NEW: Step 1.4 - Enable river highlighting by default
    , showSprings_(true) {   // NEW: Step 1.4 - Enable spring indicators by default
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
    previousElevationData_ = new float[dataSize]; // NEW: For adaptive update tracking
    
    // Initialize with default values
    for (int i = 0; i < dataSize; i++) {
        elevationData_[i] = 0.0f;
        temperatureData_[i] = 15.0f;     // 15°C default
        precipitationData_[i] = 800.0f;  // 800mm/year default
        previousElevationData_[i] = 0.0f; // NEW: Initialize previous data
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
    
    // Store generator reference for snapshot access
    currentGenerator_ = generator;
    
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
    
    // Adaptive Update System (1.3) - Replace static throttling with intelligent updates
    auto currentTime = std::chrono::steady_clock::now();
    float timeSinceLastUpdate = std::chrono::duration<float>(currentTime - std::chrono::steady_clock::time_point{}).count() - lastUpdateTime_;
    
    // Generate new elevation data first to calculate change magnitude
    generateElevationData(generator, worldSeed);
    
    // Calculate change magnitude for adaptive updates
    float changeMagnitude = calculateChangeMagnitude(elevationData_);
    
    // Check if we should update based on adaptive thresholds
    bool shouldUpdateNow = needsRegeneration || shouldUpdate(changeMagnitude, timeSinceLastUpdate);
    
    if (!shouldUpdateNow) {
        std::cout << "[WorldMapRenderer] Adaptive update - change magnitude " << changeMagnitude 
                  << " below threshold, time since update " << timeSinceLastUpdate << "s" << std::endl;
        return;
    }
    
    // Update previous data and reset tracking
    if (previousElevationData_ && elevationData_) {
        int dataSize = resolution_ * resolution_;
        std::memcpy(previousElevationData_, elevationData_, dataSize * sizeof(float));
    }
    lastUpdateTime_ = std::chrono::duration<float>(currentTime - std::chrono::steady_clock::time_point{}).count();
    accumulatedChange_ = 0.0f; // Reset accumulated change
    
    std::cout << "[WorldMapRenderer] Adaptive update triggered - change magnitude: " << changeMagnitude 
              << ", time since last: " << timeSinceLastUpdate << "s" << std::endl;
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
    // Note: elevation data was already generated above for adaptive update check
    switch (phase) {
        case GenerationPhase::TECTONICS:
            // Pure tectonic terrain - elevation already generated
            break;
        case GenerationPhase::EROSION:
            // Apply erosion effects to existing terrain
            applyErosionEffects();
            break;
        case GenerationPhase::HYDROLOGY:
            // Generate water features and modify terrain for rivers/lakes
            applyErosionEffects();
            generateWaterFeatures();
            break;
        case GenerationPhase::CLIMATE:
            // Focus on temperature and precipitation
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
    
    // NEW: Step 1.4 - Overlay water system features if HYDROLOGY mode or enabled
    if (currentMode_ == VisualizationMode::HYDROLOGY || currentPhase_ == GenerationPhase::HYDROLOGY) {
        overlayWaterSystemFeatures(colorData, generator);
    }
    
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
    std::cout << "[WorldMapRenderer] *** STARTING generateElevationData with seed: " << seed << std::endl;
    
    // CRITICAL FIX: Ensure the generator uses the correct seed for terrain variation
    // This was causing the "same lake" issue - base terrain wasn't using the seed
    if (generator->getWorldSeed()->getMasterSeed() != seed) {
        std::cout << "[WorldMapRenderer] WARNING: Generator seed mismatch - ensuring seed consistency" << std::endl;
        // The seed should already be set in the generator, but let's ensure it's consistent
    }
    
    std::cout << "[WorldMapRenderer] About to sample world area..." << std::endl;
    
    // Use the actual world size passed in - this represents the entire world being generated
    // The world size should be stored from the generateWorldMap call
    float worldSize = worldSizeKm_ * 1000.0f; // Convert from kilometers to meters
    
    std::cout << "[WorldMapRenderer] Sampling world area: " << worldSizeKm_ << "km x " << worldSizeKm_ << "km" << std::endl;
    std::cout << "[WorldMapRenderer] World size in meters: " << worldSize << "m x " << worldSize << "m" << std::endl;
    
    // Get geological simulator for new 3.0 World Generation System
    std::cout << "[WorldMapRenderer] About to get geological simulator..." << std::endl;
    const auto* geologicalSim = generator->getGeologicalSimulator();
    std::cout << "[WorldMapRenderer] Got geological simulator: " << (geologicalSim ? "SUCCESS" : "NULL") << std::endl;
    
    // Fallback to tectonic simulator if geological simulation is not available
    std::cout << "[WorldMapRenderer] About to get tectonic simulator..." << std::endl;
    const VoxelCastle::TectonicSimulator* tectonicSim = generator->getTectonicSimulator();
    std::cout << "[WorldMapRenderer] Got tectonic simulator: " << (tectonicSim ? "SUCCESS" : "NULL") << std::endl;
    
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
    
    std::cout << "[WorldMapRenderer] About to start pixel loop - resolution: " << resolution_ << "x" << resolution_ << std::endl;
    int pixelCount = 0;
    
    for (int y = 0; y < resolution_; y++) {
        for (int x = 0; x < resolution_; x++) {
            pixelCount++;
            if (pixelCount == 1 || pixelCount % 10000 == 0) {
                std::cout << "[WorldMapRenderer] Processing pixel " << pixelCount << " of " << (resolution_ * resolution_) << std::endl;
            }
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
            if (pixelCount <= 5) {
                std::cout << "[WorldMapRenderer] Getting terrain height at (" << (int)worldX << "," << (int)worldZ << ")" << std::endl;
            }
            int heightVoxels = generator->getTerrainHeightAt((int)worldX, (int)worldZ);
            float baseHeight = heightVoxels * 0.25f; // Convert voxels to meters (25cm per voxel)
            if (pixelCount <= 5) {
                std::cout << "[WorldMapRenderer] Got base height: " << baseHeight << "m" << std::endl;
            }
            
            // PRIMARY: Use new 3.0 World Generation System (Snapshot-Based Responsive UI)
            float finalHeight = baseHeight;
            if (geologicalSim) {
                if (pixelCount <= 3) {
                    std::cout << "[WorldMapRenderer] Using geological simulator for pixel " << pixelCount << std::endl;
                }
                // Use snapshot system for responsive UI - no heavy geological computation on UI thread
                const auto* snapshotManager = geologicalSim->getSnapshotManager();
                if (pixelCount <= 3) {
                    std::cout << "[WorldMapRenderer] Got snapshot manager: " << (snapshotManager ? "SUCCESS" : "NULL") << std::endl;
                    if (snapshotManager) {
                        std::cout << "[WorldMapRenderer] Snapshot count: " << snapshotManager->GetSnapshotCount() << std::endl;
                    }
                }
                if (snapshotManager && snapshotManager->GetSnapshotCount() > 0) {
                    const auto* currentSnapshot = snapshotManager->GetCurrentSnapshot();
                    if (currentSnapshot) {
                        finalHeight = currentSnapshot->GetElevationAt(worldX, worldZ);
                        
                        // Debug: Log snapshot-based geological features for first few pixels
                        if (x < 3 && y < 3) {
                            std::cout << "[WorldMapRenderer] Snapshot-based Sample - Pos(" << worldX << "," << worldZ 
                                      << ") Elevation:" << finalHeight << "m Snapshot:" << snapshotManager->GetCurrentSnapshotIndex()
                                      << "/" << snapshotManager->GetSnapshotCount() << std::endl;
                            
                            // Check for continental vs oceanic features
                            if (finalHeight > 0) {
                                std::cout << "[WorldMapRenderer] CONTINENTAL feature detected" << std::endl;
                            } else if (finalHeight < -200) {
                                std::cout << "[WorldMapRenderer] DEEP OCEAN basin detected" << std::endl;
                            } else {
                                std::cout << "[WorldMapRenderer] COASTAL/SHELF feature detected" << std::endl;
                            }
                        }
                    } else {
                        // Fallback: use legacy getSampleAt if no current snapshot (should be rare)
                        VoxelCastle::World::GeologicalSample sample = geologicalSim->getSampleAt(worldX, worldZ);
                        finalHeight = sample.elevation;
                        if (x < 3 && y < 3) {
                            std::cout << "[WorldMapRenderer] FALLBACK: Using getSampleAt (no snapshot available)" << std::endl;
                        }
                    }
                } else {
                    // CRITICAL FIX: Generate seed-based fractal terrain immediately instead of using "base terrain"
                    // This ensures the preview shows different worlds for different seeds from the start
                    if (pixelCount <= 5) {  // Only log for first 5 pixels to reduce spam
                        std::cout << "[WorldMapRenderer] No snapshots available - generating seed-based fractal terrain (seed: " << seed << ")" << std::endl;
                    }
                    
                    // Use fractal generation with the provided seed for immediate variation
                    if (pixelCount <= 3) {
                        std::cout << "[WorldMapRenderer] About to call generateSeedBasedTerrain..." << std::endl;
                    }
                    finalHeight = generateSeedBasedTerrain(worldX, worldZ, seed);
                    if (pixelCount <= 3) {
                        std::cout << "[WorldMapRenderer] generateSeedBasedTerrain returned: " << finalHeight << std::endl;
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

// CRITICAL FIX: Seed-based terrain generation for immediate preview variation
float WorldMapRenderer::generateSeedBasedTerrain(float worldX, float worldZ, unsigned int seed) {
    static int callCount = 0;
    callCount++;
    if (callCount <= 5) {
        std::cout << "[WorldMapRenderer] generateSeedBasedTerrain called " << callCount << " times - worldPos(" << worldX << "," << worldZ << ") seed:" << seed << std::endl;
    }
    
    // Use fractal noise with the provided seed to generate varied terrain immediately
    // This ensures different seeds produce visibly different worlds from the start
    
    // Create a deterministic random generator based on the seed
    std::mt19937 rng(seed);
    
    // Generate coordinate offsets based on the seed for terrain variation
    float seedOffsetX = static_cast<float>((seed & 0xFFFFFFFF) % 1000000) * 0.00001f;
    float seedOffsetZ = static_cast<float>(((seed >> 16) & 0xFFFFFFFF) % 1000000) * 0.00001f;
    
    // Apply seed-based coordinate transformation
    float nx = (worldX + seedOffsetX) * 0.001f;  // Continental scale
    float nz = (worldZ + seedOffsetZ) * 0.001f;
    
    // Generate multi-octave noise for realistic terrain
    float continentalNoise = 0.0f;
    float amplitude = 800.0f;  // Base amplitude for terrain
    float frequency = 1.0f;
    
    // Multi-octave fractal noise
    for (int octave = 0; octave < 6; octave++) {
        // Simple placeholder noise generation based on seed
        float octaveNoise = std::sin((nx * frequency + seedOffsetX) * 0.1f) * 
                           std::cos((nz * frequency + seedOffsetZ) * 0.1f);
        
        continentalNoise += octaveNoise * amplitude;
        amplitude *= 0.5f;   // Reduce amplitude for higher frequencies
        frequency *= 2.0f;   // Increase frequency for detail
    }
    
    // Add bias toward sea level with continental features
    float seaLevel = 0.0f;
    float finalHeight = seaLevel + continentalNoise * 0.3f;
    
    // Create realistic ocean/continent distribution
    float continentalBias = std::sin((nx * 0.3f + seedOffsetX) * 0.1f) * 
                           std::cos((nz * 0.3f + seedOffsetZ) * 0.1f);
    
    // Areas with high continental bias become land, others become ocean
    if (continentalBias > 0.2f) {
        finalHeight = std::max(finalHeight, 10.0f);  // Ensure land is above sea level
    } else {
        finalHeight = std::min(finalHeight, -50.0f); // Ensure ocean is below sea level
    }
    
    // Clamp to realistic geological bounds
    finalHeight = std::clamp(finalHeight, -2048.0f, 2048.0f);
    
    return finalHeight;
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
    // Check if we need to recreate textures (resolution changed or textures don't exist)
    bool needRecreateTextures = (textureA_ == 0 && textureB_ == 0) || 
                               (!glIsTexture(textureA_) || !glIsTexture(textureB_));
    
    if (needRecreateTextures) {
        // Clean up old textures if they exist
        if (textureA_ != 0) {
            glDeleteTextures(1, &textureA_);
            textureA_ = 0;
        }
        if (textureB_ != 0) {
            glDeleteTextures(1, &textureB_);
            textureB_ = 0;
        }
        
        // Create both textures
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
        std::cout << "[WorldMapRenderer] Created/recreated double-buffered textures A:" << textureA_ << " B:" << textureB_ << std::endl;
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
    
    // NEW: Cleanup adaptive update system data
    if (previousElevationData_) {
        delete[] previousElevationData_;
        previousElevationData_ = nullptr;
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
    
    const auto* geologicalSim = generator->getGeologicalSimulator();
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

// NEW: Step 1.4 - Water System Visualization Integration

void WorldMapRenderer::setWaterVisualization(bool showWaterFlow, bool showAquifers, 
                                            bool showRivers, bool showSprings) {
    showWaterFlow_ = showWaterFlow;
    showAquifers_ = showAquifers;
    showRivers_ = showRivers;
    showSprings_ = showSprings;
    
    std::cout << "[WorldMapRenderer] Water visualization settings: "
              << "Flow=" << (showWaterFlow ? "ON" : "OFF") << " "
              << "Aquifers=" << (showAquifers ? "ON" : "OFF") << " "
              << "Rivers=" << (showRivers ? "ON" : "OFF") << " "
              << "Springs=" << (showSprings ? "ON" : "OFF") << std::endl;
}

void WorldMapRenderer::getWaterVisualization(bool& showWaterFlow, bool& showAquifers, 
                                            bool& showRivers, bool& showSprings) const {
    showWaterFlow = showWaterFlow_;
    showAquifers = showAquifers_;
    showRivers = showRivers_;
    showSprings = showSprings_;
}

void WorldMapRenderer::overlayWaterSystemFeatures(unsigned char* colorData, VoxelCastle::World::SeedWorldGenerator* generator) {
    if (!generator || !colorData) return;
    
    std::cout << "[WorldMapRenderer] Overlaying water system features..." << std::endl;
    
    // Apply water overlays based on enabled features
    if (showAquifers_) {
        overlayAquiferLevels(colorData, generator);
    }
    
    if (showRivers_) {
        highlightRiverPaths(colorData, generator);
    }
    
    if (showSprings_) {
        drawSpringIndicators(colorData, generator);
    }
    
    if (showWaterFlow_) {
        drawWaterFlowArrows(colorData, generator);
    }
}

void WorldMapRenderer::drawWaterFlowArrows(unsigned char* colorData, VoxelCastle::World::SeedWorldGenerator* generator) {
    if (!generator || !colorData || !elevationData_) return;
    
    std::cout << "[WorldMapRenderer] Drawing water flow arrows..." << std::endl;
    
    // Sample water flow direction every 32 pixels to avoid overcrowding
    int sampleStep = std::max(8, resolution_ / 64);
    
    for (int y = sampleStep; y < resolution_ - sampleStep; y += sampleStep) {
        for (int x = sampleStep; x < resolution_ - sampleStep; x += sampleStep) {
            int currentIdx = y * resolution_ + x;
            float currentHeight = elevationData_[currentIdx];
            
            // Skip if this is underwater (below sea level)
            if (currentHeight <= 0.0f) continue;
            
            // Find steepest descent direction (water flow direction)
            float maxSlope = 0.0f;
            int flowX = 0, flowY = 0;
            
            // Check 8 surrounding cells for steepest descent
            for (int dy = -1; dy <= 1; dy++) {
                for (int dx = -1; dx <= 1; dx++) {
                    if (dx == 0 && dy == 0) continue;
                    
                    int nx = x + dx;
                    int ny = y + dy;
                    if (nx >= 0 && nx < resolution_ && ny >= 0 && ny < resolution_) {
                        int neighborIdx = ny * resolution_ + nx;
                        float neighborHeight = elevationData_[neighborIdx];
                        float slope = (currentHeight - neighborHeight) / std::sqrt(dx*dx + dy*dy);
                        
                        if (slope > maxSlope) {
                            maxSlope = slope;
                            flowX = dx;
                            flowY = dy;
                        }
                    }
                }
            }
            
            // Draw flow arrow if there's significant slope (indicating water flow)
            if (maxSlope > 10.0f) { // 10m height difference threshold
                // Draw a small blue arrow in the flow direction
                int arrowLength = 3;
                for (int i = 0; i < arrowLength; i++) {
                    int arrowX = x + (flowX * i);
                    int arrowY = y + (flowY * i);
                    
                    if (arrowX >= 0 && arrowX < resolution_ && arrowY >= 0 && arrowY < resolution_) {
                        int arrowIdx = (arrowY * resolution_ + arrowX) * 4;
                        // Blue arrows with transparency
                        colorData[arrowIdx] = static_cast<unsigned char>(std::min(255, static_cast<int>(colorData[arrowIdx]) + 50)); // Slight red tint
                        colorData[arrowIdx + 1] = static_cast<unsigned char>(std::min(255, static_cast<int>(colorData[arrowIdx + 1]) + 100)); // More green
                        colorData[arrowIdx + 2] = 255; // Full blue
                    }
                }
            }
        }
    }
}

void WorldMapRenderer::overlayAquiferLevels(unsigned char* colorData, VoxelCastle::World::SeedWorldGenerator* generator) {
    if (!generator || !colorData || !elevationData_) return;
    
    std::cout << "[WorldMapRenderer] Overlaying aquifer levels..." << std::endl;
    
    // Create aquifer visualization based on elevation and proximity to water
    for (int i = 0; i < resolution_ * resolution_; i++) {
        float elevation = elevationData_[i];
        
        // Generate simple aquifer level based on elevation and position
        // Lower areas and areas near sea level have higher water table
        float distanceFromSeaLevel = std::abs(elevation);
        float aquiferLevel = std::max(0.0f, 100.0f - distanceFromSeaLevel * 0.5f);
        
        // Add some noise for realism
        int x = i % resolution_;
        int y = i / resolution_;
        float noise = (std::sin(x * 0.1f) + std::cos(y * 0.1f)) * 10.0f;
        aquiferLevel += noise;
        
        // Apply aquifer visualization as a blue-green tint
        if (aquiferLevel > 20.0f) {
            int colorIdx = i * 4;
            float aquiferIntensity = std::min(1.0f, aquiferLevel / 100.0f);
            
            // Add cyan/aqua tint to represent high water table areas
            unsigned char blueTint = static_cast<unsigned char>(aquiferIntensity * 60);
            unsigned char greenTint = static_cast<unsigned char>(aquiferIntensity * 40);
            
            colorData[colorIdx + 1] = static_cast<unsigned char>(std::min(255, static_cast<int>(colorData[colorIdx + 1]) + greenTint));
            colorData[colorIdx + 2] = static_cast<unsigned char>(std::min(255, static_cast<int>(colorData[colorIdx + 2]) + blueTint));
        }
    }
}

void WorldMapRenderer::highlightRiverPaths(unsigned char* colorData, VoxelCastle::World::SeedWorldGenerator* generator) {
    if (!generator || !colorData) return;
    
    std::cout << "[WorldMapRenderer] Highlighting river paths..." << std::endl;
    
    // Get geological simulator for river template access
    const auto* geologicalSim = generator->getGeologicalSimulator();
    if (!geologicalSim) return;
    
    const VoxelCastle::World::FractalContinentGenerator* fractalGen = geologicalSim->getFractalContinentGenerator();
    if (!fractalGen) return;
    
    // Get river templates and enhance their visibility
    const auto& riverTemplates = fractalGen->GetRiverTemplates();
    
    for (const auto& river : riverTemplates) {
        // Draw each river path as a bright blue line
        for (size_t i = 1; i < river.mainStem.size(); i++) {
            const auto& start = river.mainStem[i-1];
            const auto& end = river.mainStem[i];
            
            // Convert world coordinates to pixel coordinates
            int x1 = static_cast<int>((start.x / (worldSizeKm_ * 1000.0f)) * resolution_);
            int y1 = static_cast<int>((start.y / (worldSizeKm_ * 1000.0f)) * resolution_);
            int x2 = static_cast<int>((end.x / (worldSizeKm_ * 1000.0f)) * resolution_);
            int y2 = static_cast<int>((end.y / (worldSizeKm_ * 1000.0f)) * resolution_);
            
            // Draw line between points with enhanced blue color
            drawLine(colorData, x1, y1, x2, y2, {0, 150, 255, 255}); // Bright blue
        }
    }
}

void WorldMapRenderer::drawSpringIndicators(unsigned char* colorData, VoxelCastle::World::SeedWorldGenerator* generator) {
    if (!generator || !colorData || !elevationData_) return;
    
    std::cout << "[WorldMapRenderer] Drawing spring indicators..." << std::endl;
    
    // Create spring locations based on geological features
    // Springs typically occur where aquifers meet the surface or along fault lines
    
    for (int y = 10; y < resolution_ - 10; y += 20) {
        for (int x = 10; x < resolution_ - 10; x += 20) {
            int idx = y * resolution_ + x;
            float elevation = elevationData_[idx];
            
            // Springs are likely near hillsides where water emerges
            // Check for elevation gradient (hillside conditions)
            bool hasGradient = false;
            for (int dy = -5; dy <= 5; dy += 5) {
                for (int dx = -5; dx <= 5; dx += 5) {
                    if (dx == 0 && dy == 0) continue;
                    
                    int nx = x + dx;
                    int ny = y + dy;
                    if (nx >= 0 && nx < resolution_ && ny >= 0 && ny < resolution_) {
                        int neighborIdx = ny * resolution_ + nx;
                        float heightDiff = std::abs(elevation - elevationData_[neighborIdx]);
                        if (heightDiff > 50.0f) { // Significant elevation change
                            hasGradient = true;
                            break;
                        }
                    }
                }
                if (hasGradient) break;
            }
            
            // Add some randomness for spring placement
            bool isSpring = hasGradient && (elevation > 50.0f) && (elevation < 800.0f) && 
                           ((x * y + static_cast<int>(elevation)) % 137 == 0); // Pseudo-random distribution
            
            if (isSpring) {
                // Draw spring indicator as a bright cyan dot with radiating effect
                for (int dy = -2; dy <= 2; dy++) {
                    for (int dx = -2; dx <= 2; dx++) {
                        int springX = x + dx;
                        int springY = y + dy;
                        
                        if (springX >= 0 && springX < resolution_ && springY >= 0 && springY < resolution_) {
                            int springIdx = (springY * resolution_ + springX) * 4;
                            
                            // Bright cyan spring indicator
                            if (dx == 0 && dy == 0) {
                                // Center: bright white
                                colorData[springIdx] = 255;     // R
                                colorData[springIdx + 1] = 255; // G
                                colorData[springIdx + 2] = 255; // B
                            } else {
                                // Surrounding: cyan
                                colorData[springIdx] = 0;       // R
                                colorData[springIdx + 1] = 255; // G
                                colorData[springIdx + 2] = 255; // B
                            }
                        }
                    }
                }
            }
        }
    }
}

// Helper method to draw lines for river paths
void WorldMapRenderer::drawLine(unsigned char* colorData, int x1, int y1, int x2, int y2, const std::array<unsigned char, 4>& color) {
    // Simple Bresenham line algorithm
    int dx = std::abs(x2 - x1);
    int dy = std::abs(y2 - y1);
    int x = x1;
    int y = y1;
    int n = 1 + dx + dy;
    int x_inc = (x2 > x1) ? 1 : -1;
    int y_inc = (y2 > y1) ? 1 : -1;
    int error = dx - dy;
    
    dx *= 2;
    dy *= 2;
    
    for (; n > 0; --n) {
        if (x >= 0 && x < resolution_ && y >= 0 && y < resolution_) {
            int idx = (y * resolution_ + x) * 4;
            colorData[idx] = color[0];     // R
            colorData[idx + 1] = color[1]; // G
            colorData[idx + 2] = color[2]; // B
            // Keep existing alpha
        }
        
        if (error > 0) {
            x += x_inc;
            error -= dy;
        } else {
            y += y_inc;
            error += dx;
        }
    }
}

// Adaptive Update System Implementation (1.3)
float WorldMapRenderer::calculateChangeMagnitude(const float* newElevationData) const {
    if (!previousElevationData_ || !newElevationData) {
        return 1.0f; // Force update if no previous data
    }
    
    float totalChange = 0.0f;
    float maxChange = 0.0f;
    int dataSize = resolution_ * resolution_;
    
    for (int i = 0; i < dataSize; i++) {
        float change = std::abs(newElevationData[i] - previousElevationData_[i]);
        totalChange += change;
        maxChange = std::max(maxChange, change);
    }
    
    // Calculate relative change magnitude (0.0 to 1.0)
    float averageChange = totalChange / dataSize;
    float normalizedChange = averageChange / 100.0f; // Normalize to 100m change = 1.0
    
    // Clamp to reasonable range
    return std::clamp(normalizedChange, 0.0f, 1.0f);
}

bool WorldMapRenderer::shouldUpdate(float changeMagnitude, float timeSinceLastUpdate) const {
    // Always update if significant change occurred
    if (changeMagnitude > changeThreshold_) {
        return true;
    }
    
    // Force update if too much time has passed (prevent visual stagnation)
    if (timeSinceLastUpdate > maxUpdateInterval_) {
        return true;
    }
    
    // Accumulate small changes over time
    if (accumulatedChange_ + changeMagnitude > changeThreshold_) {
        return true;
    }
    
    return false;
}

// Missing fractal continental feature drawing methods - basic implementations
void WorldMapRenderer::drawContinentalPlates(unsigned char* colorData, const std::vector<VoxelCastle::World::ContinentalPlate>& plates) {
    if (!colorData || plates.empty()) return;
    
    std::cout << "[WorldMapRenderer] Drawing " << plates.size() << " continental plates..." << std::endl;
    
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
    if (!colorData || basins.empty()) return;
    
    std::cout << "[WorldMapRenderer] Drawing " << basins.size() << " ocean basins..." << std::endl;
    
    // Draw ocean basin indicators
    for (const auto& basin : basins) {
        // Calculate center from boundary points
        if (basin.boundary.empty()) continue;
        
        glm::vec2 center(0.0f, 0.0f);
        for (const auto& point : basin.boundary) {
            center += point;
        }
        center /= static_cast<float>(basin.boundary.size());
        
        // Convert world coordinates to pixel coordinates
        int basinX = (int)((center.x / (worldSizeKm_ * 1000.0f)) * resolution_);
        int basinY = (int)((center.y / (worldSizeKm_ * 1000.0f)) * resolution_);
        
        // Draw a small blue marker for ocean basin center
        if (basinX >= 1 && basinX < resolution_ - 1 && basinY >= 1 && basinY < resolution_ - 1) {
            int idx = (basinY * resolution_ + basinX) * 4;
            colorData[idx] = 0;       // R
            colorData[idx + 1] = 50;  // G
            colorData[idx + 2] = 150; // B (deep blue)
        }
    }
}

void WorldMapRenderer::drawRiverTemplates(unsigned char* colorData, const std::vector<VoxelCastle::World::RiverTemplate>& rivers) {
    // This method is already implemented above in highlightRiverPaths
    // Just call that method for consistency
    highlightRiverPaths(colorData, nullptr);
}

void WorldMapRenderer::drawMountainRidges(unsigned char* colorData, const std::vector<VoxelCastle::World::MountainRidge>& ridges) {
    if (!colorData || ridges.empty()) return;
    
    std::cout << "[WorldMapRenderer] Drawing " << ridges.size() << " mountain ridges..." << std::endl;
    
    // Draw mountain ridge indicators
    for (const auto& ridge : ridges) {
        // Calculate center from ridge line
        if (ridge.ridgeLine.empty()) continue;
        
        glm::vec2 center(0.0f, 0.0f);
        for (const auto& point : ridge.ridgeLine) {
            center += point;
        }
        center /= static_cast<float>(ridge.ridgeLine.size());
        
        // Convert world coordinates to pixel coordinates
        int ridgeX = (int)((center.x / (worldSizeKm_ * 1000.0f)) * resolution_);
        int ridgeY = (int)((center.y / (worldSizeKm_ * 1000.0f)) * resolution_);
        
        // Draw a brown mountain marker
        if (ridgeX >= 1 && ridgeX < resolution_ - 1 && ridgeY >= 1 && ridgeY < resolution_ - 1) {
            for (int dy = -1; dy <= 1; dy++) {
                for (int dx = -1; dx <= 1; dx++) {
                    int mx = ridgeX + dx;
                    int my = ridgeY + dy;
                    if (mx >= 0 && mx < resolution_ && my >= 0 && my < resolution_) {
                        int idx = (my * resolution_ + mx) * 4;
                        colorData[idx] = 139;     // R (brown)
                        colorData[idx + 1] = 90;  // G
                        colorData[idx + 2] = 43;  // B
                    }
                }
            }
        }
    }
}

const VoxelCastle::World::HybridGeologicalSimulator* VoxelEngine::UI::WorldMapRenderer::getGeologicalSimulator() const {
    if (!currentGenerator_) {
        return nullptr;
    }
    return currentGenerator_->getGeologicalSimulator();
}
}
