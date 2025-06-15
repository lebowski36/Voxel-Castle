#include "ui/SeedDebugPanel.h"
#include "ui/TextRenderer.h"
#include "world/world_seed.h"
#include <sstream>
#include <iomanip>
#include <iostream>

namespace VoxelEngine {
namespace UI {

SeedDebugPanel::SeedDebugPanel() {
    // Set default size and position (top-left corner)
    setPosition(10.0f, 10.0f);
    setSize(400.0f, 300.0f);
    setVisible(false); // Hidden by default, toggle with key
}

void SeedDebugPanel::update(float deltaTime) {
    if (!isVisible() || !worldSeed_) return;
    
    updateTimer_ += deltaTime;
    if (updateTimer_ >= updateInterval_) {
        updateTimer_ = 0.0f;
        
        if (showSeedInfo_) {
            updateSeedInfo();
        }
        
        if (showCoordinateSeeds_) {
            updateCoordinateSeeds();
        }
        
        if (showFeatureBreakdown_) {
            updateFeatureBreakdown();
        }
    }
}

void SeedDebugPanel::render() {
    if (!isVisible() || !worldSeed_) return;
    renderPanel();
}

bool SeedDebugPanel::handleInput(float mouseX, float mouseY, bool clicked) {
    // Check if click is within panel bounds
    if (containsPoint(mouseX, mouseY) && clicked) {
        // Toggle feature breakdown on click
        showFeatureBreakdown_ = !showFeatureBreakdown_;
        return true; // Consume the input
    }
    return false;
}

void SeedDebugPanel::updateSeedInfo() {
    std::ostringstream oss;
    oss << "Master Seed: " << worldSeed_->getMasterSeed() << "\\n";
    oss << "Seed String: \"" << worldSeed_->getSeedString() << "\"\\n";
    masterSeedText_ = oss.str();
}

void SeedDebugPanel::updateCoordinateSeeds() {
    // Player position seeds
    std::ostringstream playerOss;
    playerOss << "Player Position: " << formatCoordinateInfo(playerPosition_) << "\\n";
    playerOss << getSeedDebugInfo(playerPosition_);
    playerSeedText_ = playerOss.str();
    
    // Cursor position seeds
    std::ostringstream cursorOss;
    cursorOss << "Cursor Position: " << formatCoordinateInfo(cursorPosition_) << "\\n";
    cursorOss << getSeedDebugInfo(cursorPosition_);
    cursorSeedText_ = cursorOss.str();
}

void SeedDebugPanel::updateFeatureBreakdown() {
    if (!showFeatureBreakdown_) {
        featureBreakdownText_ = "";
        return;
    }
    
    std::ostringstream oss;
    oss << "Feature Seeds at Cursor:\\n";
    
    // Show all feature types
    const VoxelCastle::World::FeatureType features[] = {
        VoxelCastle::World::FeatureType::TERRAIN,
        VoxelCastle::World::FeatureType::CAVES,
        VoxelCastle::World::FeatureType::ORES,
        VoxelCastle::World::FeatureType::STRUCTURES,
        VoxelCastle::World::FeatureType::BIOMES,
        VoxelCastle::World::FeatureType::WEATHER,
        VoxelCastle::World::FeatureType::WATER,
        VoxelCastle::World::FeatureType::VEGETATION
    };
    
    for (const auto& feature : features) {
        uint64_t seed = worldSeed_->getFeatureSeed(
            static_cast<int64_t>(cursorPosition_.x),
            static_cast<int64_t>(cursorPosition_.y),
            static_cast<int64_t>(cursorPosition_.z),
            feature
        );
        oss << "  " << VoxelCastle::World::getFeatureTypeName(feature) 
            << ": " << seed << "\\n";
    }
    
    featureBreakdownText_ = oss.str();
}

// Enhanced testing functionality
void SeedDebugPanel::setTestSeed(const std::string& seedString) {
    testSeedString_ = seedString;
    testSeed_.setSeedFromString(seedString);
    isTestingSeed_ = true;
    updateTestSeedDisplay();
}

void SeedDebugPanel::setTestSeed(uint64_t seed) {
    testSeed_.setSeed(seed);
    testSeedString_ = std::to_string(seed);
    isTestingSeed_ = true;
    updateTestSeedDisplay();
}

void SeedDebugPanel::resetToWorldSeed() {
    isTestingSeed_ = false;
    testSeedText_ = "";
}

void SeedDebugPanel::setComparisonSeed(const std::string& seedString) {
    comparisonSeedString_ = seedString;
    comparisonSeed_.setSeedFromString(seedString);
    updateComparisonDisplay();
}

void SeedDebugPanel::updateTestSeedDisplay() {
    if (!isTestingSeed_) {
        testSeedText_ = "";
        return;
    }
    
    std::ostringstream oss;
    oss << "TEST SEED: " << testSeed_.getMasterSeed() << "\\n";
    oss << "String: \"" << testSeedString_ << "\"\\n";
    oss << "Test Position: " << formatCoordinateInfo(cursorPosition_) << "\\n";
    
    // Show test seed values at cursor position
    uint64_t terrainSeed = testSeed_.getFeatureSeed(
        static_cast<int64_t>(cursorPosition_.x),
        static_cast<int64_t>(cursorPosition_.y),
        static_cast<int64_t>(cursorPosition_.z),
        VoxelCastle::World::FeatureType::TERRAIN
    );
    oss << "Terrain Seed: " << terrainSeed << "\\n";
    
    testSeedText_ = oss.str();
}

void SeedDebugPanel::updateComparisonDisplay() {
    if (!comparisonMode_) {
        comparisonText_ = "";
        return;
    }
    
    std::ostringstream oss;
    oss << "COMPARISON:\\n";
    oss << "Main: " << (worldSeed_ ? worldSeed_->getMasterSeed() : 0) << "\\n";
    oss << "Test: " << comparisonSeed_.getMasterSeed() << "\\n";
    
    // Show difference in terrain generation at current position
    if (worldSeed_) {
        uint64_t mainSeed = worldSeed_->getFeatureSeed(
            static_cast<int64_t>(cursorPosition_.x),
            static_cast<int64_t>(cursorPosition_.y),
            static_cast<int64_t>(cursorPosition_.z),
            VoxelCastle::World::FeatureType::TERRAIN
        );
        uint64_t compSeed = comparisonSeed_.getFeatureSeed(
            static_cast<int64_t>(cursorPosition_.x),
            static_cast<int64_t>(cursorPosition_.y),
            static_cast<int64_t>(cursorPosition_.z),
            VoxelCastle::World::FeatureType::TERRAIN
        );
        
        oss << "Terrain Diff: " << (mainSeed != compSeed ? "DIFFERENT" : "SAME") << "\\n";
    }
    
    comparisonText_ = oss.str();
}

void SeedDebugPanel::exportSeedVisualization() {
    // TODO: Export current seed visualization as an image
    // This would create a heightmap or other visualization for documentation
    std::cout << "[SeedDebugPanel] Exporting visualization for seed: " 
              << (worldSeed_ ? worldSeed_->getSeedString() : "none") << std::endl;
}

void SeedDebugPanel::logSeedInformation() {
    if (!worldSeed_) return;
    
    std::cout << "=== SEED DEBUG INFO ===" << std::endl;
    std::cout << "Master Seed: " << worldSeed_->getMasterSeed() << std::endl;
    std::cout << "Seed String: " << worldSeed_->getSeedString() << std::endl;
    std::cout << "Player Position: (" << playerPosition_.x << ", " 
              << playerPosition_.y << ", " << playerPosition_.z << ")" << std::endl;
    std::cout << "Cursor Position: (" << cursorPosition_.x << ", " 
              << cursorPosition_.y << ", " << cursorPosition_.z << ")" << std::endl;
              
    // Log feature seeds at cursor position
    const VoxelCastle::World::FeatureType features[] = {
        VoxelCastle::World::FeatureType::TERRAIN,
        VoxelCastle::World::FeatureType::CAVES,
        VoxelCastle::World::FeatureType::ORES,
        VoxelCastle::World::FeatureType::STRUCTURES,
        VoxelCastle::World::FeatureType::BIOMES
    };
    
    for (const auto& feature : features) {
        uint64_t seed = worldSeed_->getFeatureSeed(
            static_cast<int64_t>(cursorPosition_.x),
            static_cast<int64_t>(cursorPosition_.y),
            static_cast<int64_t>(cursorPosition_.z),
            feature
        );
        std::cout << VoxelCastle::World::getFeatureTypeName(feature) 
                  << " Seed: " << seed << std::endl;
    }
    std::cout << "=======================" << std::endl;
}

float SeedDebugPanel::getSeedValueAtPosition(const glm::vec3& pos, VoxelCastle::World::FeatureType feature) {
    if (!worldSeed_) return 0.0f;
    
    uint64_t seed = worldSeed_->getFeatureSeed(
        static_cast<int64_t>(pos.x),
        static_cast<int64_t>(pos.y),
        static_cast<int64_t>(pos.z),
        feature
    );
    
    // Convert seed to 0.0-1.0 range for visualization
    return VoxelCastle::World::WorldSeed::seedToFloat(seed);
}

glm::vec3 SeedDebugPanel::seedValueToColor(float value) {
    // Create a heat map color scheme
    if (value < 0.25f) {
        return glm::vec3(0.0f, 0.0f, 1.0f); // Blue
    } else if (value < 0.5f) {
        return glm::vec3(0.0f, 1.0f, 0.0f); // Green
    } else if (value < 0.75f) {
        return glm::vec3(1.0f, 1.0f, 0.0f); // Yellow
    } else {
        return glm::vec3(1.0f, 0.0f, 0.0f); // Red
    }
}

void SeedDebugPanel::renderPanel() {
    glm::vec2 pos = getAbsolutePosition();
    
    // Render semi-transparent background
    // TODO: Add background rendering using UI renderer
    
    float y = pos.y + 20.0f;
    const float lineHeight = 20.0f;
    
    // Render title
    renderText("World Seed Debug Info", pos.x + 10.0f, y, 1.2f);
    y += lineHeight * 1.5f;
    
    // Render master seed info
    if (showSeedInfo_ && !masterSeedText_.empty()) {
        renderText(masterSeedText_, pos.x + 10.0f, y);
        y += lineHeight * 2.0f;
    }
    
    // Render player position info
    if (showCoordinateSeeds_ && !playerSeedText_.empty()) {
        renderText(playerSeedText_, pos.x + 10.0f, y);
        y += lineHeight * 3.0f;
    }
    
    // Render cursor position info
    if (showCoordinateSeeds_ && !cursorSeedText_.empty()) {
        renderText(cursorSeedText_, pos.x + 10.0f, y);
        y += lineHeight * 3.0f;
    }
    
    // Render feature breakdown if requested
    if (showFeatureBreakdown_ && !featureBreakdownText_.empty()) {
        renderText(featureBreakdownText_, pos.x + 10.0f, y, 0.8f);
    }
    
    // Render instructions
    renderText("Click panel to toggle feature breakdown", pos.x + 10.0f, pos.y + size_.y - 25.0f, 0.7f);
}

void SeedDebugPanel::renderText(const std::string& text, float x, float y, float scale) {
    // TODO: Use the actual TextRenderer to render text
    // For now, this is a placeholder that would integrate with the existing text rendering system
    // TextRenderer::getInstance().renderText(text, x, y, scale);
    (void)text; (void)x; (void)y; (void)scale; // Suppress warnings
}

std::string SeedDebugPanel::formatCoordinateInfo(const glm::vec3& pos) {
    std::ostringstream oss;
    oss << "(" << std::fixed << std::setprecision(1) 
        << pos.x << ", " << pos.y << ", " << pos.z << ")";
    
    // Add chunk coordinates
    int64_t chunkX = static_cast<int64_t>(pos.x) / 32;
    int64_t chunkY = static_cast<int64_t>(pos.y) / 32;
    int64_t chunkZ = static_cast<int64_t>(pos.z) / 32;
    oss << " [Chunk: " << chunkX << "," << chunkY << "," << chunkZ << "]";
    
    return oss.str();
}

std::string SeedDebugPanel::getSeedDebugInfo(const glm::vec3& pos, VoxelCastle::World::FeatureType feature) {
    std::ostringstream oss;
    
    int64_t x = static_cast<int64_t>(pos.x);
    int64_t y = static_cast<int64_t>(pos.y);
    int64_t z = static_cast<int64_t>(pos.z);
    
    // Get debug info from WorldSeed
    auto debugInfo = worldSeed_->getDebugInfo(x, y, z, VoxelCastle::World::ScaleLevel::CHUNK, feature);
    
    oss << "Block Seed: " << debugInfo.finalSeed << "\\n";
    oss << "Feature: " << debugInfo.featureName << " (" << debugInfo.scaleName << ")";
    
    return oss.str();
}

} // namespace UI
} // namespace VoxelEngine
