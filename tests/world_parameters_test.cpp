#include "world/world_parameters.h"
#include <iostream>
#include <iomanip>

using namespace VoxelCastle::World;

void testPresetApplication() {
    std::cout << "=== Testing Preset Application ===\n";
    
    WorldParameters params;
    
    // Test each preset
    std::vector<WorldPreset> presets = {
        WorldPreset::NORMAL,
        WorldPreset::FLAT,
        WorldPreset::AMPLIFIED,
        WorldPreset::ISLANDS
    };
    
    for (auto preset : presets) {
        params.applyPreset(preset, WorldSize::REGIONAL);
        
        std::cout << "\nPreset: " << getWorldPresetName(preset) << std::endl;
        std::cout << "  Description: " << WorldParameters::getPresetDescription(preset) << std::endl;
        std::cout << "  Base Height: " << params.terrain.baseHeight << std::endl;
        std::cout << "  Height Variation: " << params.terrain.heightVariation << std::endl;
        std::cout << "  Noise Scale: " << params.terrain.noiseScale << std::endl;
        std::cout << "  Water Level: " << params.terrain.waterLevel << std::endl;
        std::cout << "  Cave Frequency: " << params.caves.caveFrequency << std::endl;
    }
}

void testWorldSizeScaling() {
    std::cout << "\n=== Testing World Size Scaling ===\n";
    
    std::vector<WorldSize> sizes = {
        WorldSize::STARTER,
        WorldSize::REGIONAL,
        WorldSize::CONTINENTAL,
        WorldSize::MASSIVE
    };
    
    for (auto size : sizes) {
        WorldParameters params(WorldPreset::NORMAL, size);
        
        std::cout << "\nWorld Size: " << getWorldSizeName(size) << std::endl;
        std::cout << "  Description: " << WorldParameters::getWorldSizeDescription(size) << std::endl;
        std::cout << "  Temperature Scale: " << params.biomes.temperatureScale << std::endl;
        std::cout << "  Village Frequency: " << params.structures.villageFrequency << std::endl;
        std::cout << "  Simulation Years: " << params.history.simulationYears << std::endl;
        std::cout << "  Civilization Count: " << params.history.civilizationCount << std::endl;
    }
}

void testParameterValidation() {
    std::cout << "\n=== Testing Parameter Validation ===\n";
    
    WorldParameters params;
    
    // Test with valid parameters
    std::cout << "Valid parameters: " << (params.validateParameters() ? "PASS" : "FAIL") << std::endl;
    
    // Test with invalid parameters
    params.terrain.baseHeight = -10.0f;  // Invalid
    params.terrain.heightVariation = 500.0f;  // Invalid
    params.caves.caveFrequency = 1.0f;  // Invalid
    
    std::cout << "Invalid parameters: " << (params.validateParameters() ? "PASS" : "FAIL (expected)") << std::endl;
    
    // Check if values were clamped
    std::cout << "Base height after validation: " << params.terrain.baseHeight << std::endl;
    std::cout << "Height variation after validation: " << params.terrain.heightVariation << std::endl;
    std::cout << "Cave frequency after validation: " << params.caves.caveFrequency << std::endl;
}

void testParameterAccess() {
    std::cout << "\n=== Testing Parameter Access ===\n";
    
    WorldParameters params;
    
    // Test setting and getting terrain parameters
    params.setTerrainParameter("baseHeight", 100.0f);
    params.setTerrainParameter("heightVariation", 50.0f);
    params.setTerrainParameter("noiseScale", 0.025f);
    
    std::cout << "Set base height to 100.0, got: " << params.getTerrainParameter("baseHeight") << std::endl;
    std::cout << "Set height variation to 50.0, got: " << params.getTerrainParameter("heightVariation") << std::endl;
    std::cout << "Set noise scale to 0.025, got: " << params.getTerrainParameter("noiseScale") << std::endl;
    
    // Test invalid parameter name
    params.setTerrainParameter("invalidParam", 1.0f);
    float invalid = params.getTerrainParameter("invalidParam");
    std::cout << "Invalid parameter returned: " << invalid << " (should be 0.0)" << std::endl;
}

void testSerialization() {
    std::cout << "\n=== Testing Serialization ===\n";
    
    WorldParameters params(WorldPreset::AMPLIFIED, WorldSize::CONTINENTAL);
    
    std::string serialized = params.serialize();
    std::cout << "Serialized parameters:\n" << serialized << std::endl;
    
    // Test deserialization (simplified for now)
    WorldParameters newParams;
    bool success = newParams.deserialize(serialized);
    std::cout << "Deserialization: " << (success ? "SUCCESS" : "FAILED") << std::endl;
}

void testPresetDescriptions() {
    std::cout << "\n=== Testing Preset and Size Descriptions ===\n";
    
    auto presets = WorldParameters::getAvailablePresets();
    std::cout << "Available presets:\n";
    for (const auto& preset : presets) {
        std::cout << "  - " << preset << std::endl;
    }
    
    std::cout << "\nPreset descriptions:\n";
    std::vector<WorldPreset> presetEnums = {
        WorldPreset::NORMAL, WorldPreset::FLAT, WorldPreset::AMPLIFIED, 
        WorldPreset::ISLANDS, WorldPreset::CUSTOM
    };
    
    for (auto preset : presetEnums) {
        std::cout << "  " << getWorldPresetName(preset) << ": " 
                  << WorldParameters::getPresetDescription(preset) << std::endl;
    }
}

int main() {
    std::cout << "=== World Parameters System Test Suite ===\n";
    
    testPresetApplication();
    testWorldSizeScaling();
    testParameterValidation();
    testParameterAccess();
    testSerialization();
    testPresetDescriptions();
    
    std::cout << "\n=== All Tests Completed ===\n";
    std::cout << "World Parameters system is ready for integration with world generation.\n";
    
    return 0;
}
