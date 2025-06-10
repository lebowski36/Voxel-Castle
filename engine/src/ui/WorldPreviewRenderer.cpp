#include "ui/WorldPreviewRenderer.h"
#include "ui/UIRenderer.h"
#include "world/seed_world_generator.h"
#include "world/world_seed.h" 
#include "world/world_parameters.h"
#include <iostream>
#include <algorithm>
#include <cmath>

namespace VoxelEngine::UI {

WorldPreviewRenderer::WorldPreviewRenderer()
    : previewTexture_(0)
    , previewFramebuffer_(0)
    , vertexBuffer_(0)
    , vertexArray_(0)
    , resolution_(256)
    , textureValid_(false)
    , heightData_(nullptr)
    , colorData_(nullptr) {
}

WorldPreviewRenderer::~WorldPreviewRenderer() {
    cleanup();
}

bool WorldPreviewRenderer::initialize(int previewResolution) {
    resolution_ = previewResolution;
    
    // Allocate data buffers
    heightData_ = std::make_unique<float[]>(resolution_ * resolution_);
    colorData_ = std::make_unique<unsigned char[]>(resolution_ * resolution_ * 4); // RGBA
    
    // Setup OpenGL resources
    setupRenderingResources();
    
    std::cout << "[WorldPreviewRenderer] Initialized with resolution " << resolution_ << "x" << resolution_ << std::endl;
    return true;
}

void WorldPreviewRenderer::generatePreview(std::shared_ptr<VoxelCastle::World::WorldSeed> worldSeed,
                                         std::shared_ptr<VoxelCastle::World::WorldParameters> worldParameters,
                                         int centerX, int centerZ, int sampleRadius) {
    if (!worldSeed || !worldParameters) {
        std::cerr << "[WorldPreviewRenderer] Error: Invalid seed or parameters" << std::endl;
        return;
    }
    
    // Create a temporary world generator for preview generation
    VoxelCastle::World::SeedWorldGenerator generator(worldSeed, worldParameters);
    
    // Generate heightmap data
    try {
        generator.generatePreviewHeightmap(centerX, centerZ, sampleRadius, resolution_, heightData_.get());
        
        // Convert height data to color texture
        heightmapToColorTexture(heightData_.get(), resolution_, colorData_.get());
        
        // Create OpenGL texture from color data
        createTextureFromColorData(colorData_.get(), resolution_);
        
        textureValid_ = true;
        std::cout << "[WorldPreviewRenderer] Generated preview for center (" << centerX << ", " << centerZ 
                  << ") with radius " << sampleRadius << std::endl;
                  
    } catch (const std::exception& e) {
        std::cerr << "[WorldPreviewRenderer] Error generating preview: " << e.what() << std::endl;
        textureValid_ = false;
    }
}

void WorldPreviewRenderer::render(VoxelEngine::UI::UIRenderer* renderer, float x, float y, float width, float height) {
    if (!renderer) {
        std::cout << "[WorldPreviewRenderer] ERROR: renderer is null" << std::endl;
        return;
    }
    
    std::cout << "[WorldPreviewRenderer] render() called at (" << x << ", " << y << ") size " << width << "x" << height << std::endl;
    std::cout << "[WorldPreviewRenderer] textureValid_=" << textureValid_ << ", heightData_=" << (heightData_ ? "valid" : "null") << std::endl;
    
    // Check if we have valid heightmap data to render
    if (!textureValid_ || !heightData_) {
        std::cout << "[WorldPreviewRenderer] Rendering placeholder - no valid data" << std::endl;
        // Render a placeholder colored rectangle when no data available
        glm::vec4 placeholderColor(0.3f, 0.3f, 0.3f, 1.0f); // Gray placeholder
        renderer->renderColoredQuad(x, y, width, height, placeholderColor);
        
        // Add text to show it's a placeholder
        if (renderer->isTextRendererAvailable()) {
            renderer->drawText("Generating Preview...", x + 10, y + height - 20, 0.8f, glm::vec3(1.0f, 1.0f, 1.0f));
        }
        return;
    }
    
    std::cout << "[WorldPreviewRenderer] Rendering heightmap texture" << std::endl;
    
    // Check if we have a valid texture to render
    if (previewTexture_ == 0) {
        std::cout << "[WorldPreviewRenderer] Warning: previewTexture_ is 0, rendering magenta fallback" << std::endl;
        // Fallback to magenta debug quad if texture creation failed
        glm::vec4 errorColor(1.0f, 0.0f, 1.0f, 1.0f);
        renderer->renderColoredQuad(x, y, width, height, errorColor);
        return;
    }
    
    // Render the actual heightmap texture
    std::cout << "[WorldPreviewRenderer] Rendering texture ID " << previewTexture_ 
              << " at (" << x << ", " << y << ") size " << width << "x" << height << std::endl;
    
    // Use full texture coordinates (0,0,1,1) to display the entire texture
    glm::vec4 fullTexCoords(0.0f, 0.0f, 1.0f, 1.0f);
    renderer->renderTexturedQuad(x, y, width, height, previewTexture_, fullTexCoords);
    
    // Calculate average height for display text
    float totalHeight = 0.0f;
    int sampleCount = std::min(100, resolution_ * resolution_);
    for (int i = 0; i < sampleCount; i++) {
        totalHeight += heightData_[i];
    }
    float avgHeight = totalHeight / sampleCount;
    
    // Add a text overlay to show the height information
    if (renderer->isTextRendererAvailable()) {
        std::string heightText = "Height: " + std::to_string((int)avgHeight) + "m";
        renderer->drawText(heightText, x + 10, y + height - 20, 0.8f, glm::vec3(1.0f, 1.0f, 1.0f));
    }
}

void WorldPreviewRenderer::heightmapToColorTexture(const float* heightData, int resolution, unsigned char* colorData) {
    // Find height range for color mapping
    float minHeight = heightData[0];
    float maxHeight = heightData[0];
    
    for (int i = 1; i < resolution * resolution; i++) {
        minHeight = std::min(minHeight, heightData[i]);
        maxHeight = std::max(maxHeight, heightData[i]);
    }
    
    std::cout << "[WorldPreviewRenderer] Height range: " << minHeight << "m to " << maxHeight << "m" << std::endl;
    
    // Convert each height value to RGBA color
    for (int i = 0; i < resolution * resolution; i++) {
        unsigned char r, g, b;
        heightToColor(heightData[i], r, g, b);
        
        // Store in RGBA format
        colorData[i * 4 + 0] = r; // Red
        colorData[i * 4 + 1] = g; // Green  
        colorData[i * 4 + 2] = b; // Blue
        colorData[i * 4 + 3] = 255; // Alpha (fully opaque)
    }
}

void WorldPreviewRenderer::createTextureFromColorData(const unsigned char* colorData, int resolution) {
    // Delete existing texture if present
    if (previewTexture_ != 0) {
        glDeleteTextures(1, &previewTexture_);
        previewTexture_ = 0;
    }
    
    // Generate new texture
    glGenTextures(1, &previewTexture_);
    glBindTexture(GL_TEXTURE_2D, previewTexture_);
    
    // Set texture parameters
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    
    // Upload texture data
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, resolution, resolution, 0, GL_RGBA, GL_UNSIGNED_BYTE, colorData);
    
    glBindTexture(GL_TEXTURE_2D, 0);
    
    std::cout << "[WorldPreviewRenderer] Created OpenGL texture " << previewTexture_ 
              << " with resolution " << resolution << "x" << resolution << std::endl;
}

void WorldPreviewRenderer::setupRenderingResources() {
    // For now, we'll use immediate mode rendering in the render() method
    // This could be enhanced with modern OpenGL VAOs/VBOs if needed
}

void WorldPreviewRenderer::cleanup() {
    if (previewTexture_ != 0) {
        glDeleteTextures(1, &previewTexture_);
        previewTexture_ = 0;
    }
    
    if (previewFramebuffer_ != 0) {
        glDeleteFramebuffers(1, &previewFramebuffer_);
        previewFramebuffer_ = 0;
    }
    
    if (vertexBuffer_ != 0) {
        glDeleteBuffers(1, &vertexBuffer_);
        vertexBuffer_ = 0;
    }
    
    if (vertexArray_ != 0) {
        glDeleteVertexArrays(1, &vertexArray_);
        vertexArray_ = 0;
    }
    
    textureValid_ = false;
}

void WorldPreviewRenderer::heightToColor(float heightMeters, unsigned char& r, unsigned char& g, unsigned char& b) {
    // Color scheme based on elevation:
    // Deep water: Dark blue  
    // Shallow water: Light blue
    // Beach/coast: Tan
    // Lowlands: Green
    // Hills: Light brown
    // Mountains: Dark brown
    // High peaks: White/gray
    
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
}

} // namespace VoxelEngine::UI
