#include <iostream>
#include <cassert>
#include <vector>
#include <chrono>
#include "world/tectonic_simulator.h"
#include "world/tectonic_plate.h"

using namespace VoxelCastle;

// Test utility function declaration (implementation at end of file)
bool PrintTestResult(const std::string& testName, bool passed);

// Test TectonicPlate basic functionality
bool TestTectonicPlateBasics() {
    // Test default constructor
    TectonicPlate defaultPlate;
    if (defaultPlate.plateId != 0 || defaultPlate.type != PlateType::CONTINENTAL) {
        return false;
    }
    
    // Test parameterized constructor
    TectonicPlate plate(1, glm::vec2(50.0f, 50.0f), PlateType::OCEANIC);
    if (plate.plateId != 1 || plate.centerPosition != glm::vec2(50.0f, 50.0f) || plate.type != PlateType::OCEANIC) {
        return false;
    }
    
    // Test density calculation
    float oceanicDensity = CalculatePlateDensity(PlateType::OCEANIC, 100.0f);
    float continentalDensity = CalculatePlateDensity(PlateType::CONTINENTAL, 100.0f);
    if (oceanicDensity <= continentalDensity) {
        return false; // Oceanic should be denser
    }
    
    return true;
}

// Test TectonicPlate position containment
bool TestTectonicPlateContainment() {
    TectonicPlate plate(1, glm::vec2(50.0f, 50.0f), PlateType::CONTINENTAL);
    plate.area = 1000.0f; // 1000 km²
    
    // Test position near center (should be contained)
    bool containsCenter = plate.ContainsPosition(glm::vec2(50.0f, 50.0f));
    
    // Test position far away (should not be contained)
    bool containsFar = plate.ContainsPosition(glm::vec2(500.0f, 500.0f));
    
    return containsCenter && !containsFar;
}

// Test TectonicPlate serialization
bool TestTectonicPlateSerialization() {
    TectonicPlate originalPlate(42, glm::vec2(100.0f, 200.0f), PlateType::MICROPLATE);
    originalPlate.area = 1500.0f;
    originalPlate.age = 250.0f;
    originalPlate.stability = 0.7f;
    originalPlate.boundary = {
        glm::vec2(90.0f, 190.0f),
        glm::vec2(110.0f, 190.0f),
        glm::vec2(110.0f, 210.0f),
        glm::vec2(90.0f, 210.0f)
    };
    
    // Serialize
    std::vector<uint8_t> buffer;
    originalPlate.Serialize(buffer);
    
    // Deserialize
    TectonicPlate deserializedPlate;
    size_t offset = 0;
    deserializedPlate.Deserialize(buffer.data(), offset);
    
    // Verify data integrity
    return (originalPlate.plateId == deserializedPlate.plateId &&
            originalPlate.centerPosition == deserializedPlate.centerPosition &&
            originalPlate.type == deserializedPlate.type &&
            std::abs(originalPlate.area - deserializedPlate.area) < 0.001f &&
            std::abs(originalPlate.age - deserializedPlate.age) < 0.001f &&
            std::abs(originalPlate.stability - deserializedPlate.stability) < 0.001f &&
            originalPlate.boundary.size() == deserializedPlate.boundary.size());
}

// Test TectonicSimulator initialization
bool TestTectonicSimulatorInitialization() {
    TectonicSimulator simulator;
    
    // Test with small world
    simulator.InitializePlates(12345, 100.0f); // 100km x 100km world
    
    const auto& plates = simulator.GetPlates();
    if (plates.empty() || plates.size() > 20) {
        return false; // Should have reasonable number of plates
    }
    
    // Verify plates have valid properties
    for (const auto& plate : plates) {
        if (plate.centerPosition.x < 0 || plate.centerPosition.x > 100.0f ||
            plate.centerPosition.y < 0 || plate.centerPosition.y > 100.0f ||
            plate.area <= 0) {
            return false;
        }
    }
    
    return true;
}

// Test TectonicSimulator determinism
bool TestTectonicSimulatorDeterminism() {
    TectonicSimulator simulator1, simulator2;
    uint32_t seed = 54321;
    float worldSize = 200.0f;
    
    // Initialize both simulators with same parameters
    simulator1.InitializePlates(seed, worldSize);
    simulator2.InitializePlates(seed, worldSize);
    
    const auto& plates1 = simulator1.GetPlates();
    const auto& plates2 = simulator2.GetPlates();
    
    // Should have same number of plates
    if (plates1.size() != plates2.size()) {
        return false;
    }
    
    // Plates should be identical
    for (size_t i = 0; i < plates1.size(); i++) {
        if (plates1[i].plateId != plates2[i].plateId ||
            glm::distance(plates1[i].centerPosition, plates2[i].centerPosition) > 0.001f ||
            plates1[i].type != plates2[i].type) {
            return false;
        }
    }
    
    return true;
}

// Test TectonicSimulator simulation execution
bool TestTectonicSimulatorSimulation() {
    TectonicSimulator simulator;
    simulator.InitializePlates(98765, 150.0f);
    
    // Record initial state
    const auto& initialPlates = simulator.GetPlates();
    std::vector<glm::vec2> initialPositions;
    for (const auto& plate : initialPlates) {
        initialPositions.push_back(plate.centerPosition);
    }
    
    // Run simulation
    auto startTime = std::chrono::high_resolution_clock::now();
    simulator.SimulatePlateMovement(50.0f, 5); // 50 million years, 5 steps
    auto endTime = std::chrono::high_resolution_clock::now();
    
    // Check simulation completed
    if (!simulator.IsSimulationComplete()) {
        return false;
    }
    
    // Check performance (should complete quickly)
    auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(endTime - startTime);
    if (duration.count() > 5000) { // Should take less than 5 seconds
        return false;
    }
    
    // Generate boundary features
    simulator.GenerateBoundaryFeatures();
    
    // Check that we can query terrain types and stress
    TerrainType terrainType = simulator.GetTerrainTypeAtPosition(glm::vec2(75.0f, 75.0f));
    float stress = simulator.GetTectonicStress(glm::vec2(75.0f, 75.0f));
    float elevation = simulator.GetElevationModifier(glm::vec2(75.0f, 75.0f));
    
    // Values should be valid
    if (stress < 0.0f || stress > 1.0f) {
        return false;
    }
    
    return true;
}

// Test terrain type and stress queries
bool TestTectonicSimulatorQueries() {
    TectonicSimulator simulator;
    simulator.InitializePlates(11111, 100.0f);
    simulator.SimulatePlateMovement(100.0f, 10);
    simulator.GenerateBoundaryFeatures();
    
    // Test various positions
    std::vector<glm::vec2> testPositions = {
        glm::vec2(10.0f, 10.0f),
        glm::vec2(50.0f, 50.0f),
        glm::vec2(90.0f, 90.0f),
        glm::vec2(25.0f, 75.0f)
    };
    
    for (const auto& pos : testPositions) {
        TerrainType terrain = simulator.GetTerrainTypeAtPosition(pos);
        float stress = simulator.GetTectonicStress(pos);
        float elevation = simulator.GetElevationModifier(pos);
        const TectonicPlate* plate = simulator.GetDominantPlate(pos);
        
        // Basic validation with debug output
        if (stress < 0.0f || stress > 1.0f) {
            std::cerr << "DEBUG: Invalid stress value " << stress << " at position (" << pos.x << ", " << pos.y << ")" << std::endl;
            return false;
        }
        
        // Should find a dominant plate
        if (!plate) {
            std::cerr << "DEBUG: No dominant plate found at position (" << pos.x << ", " << pos.y << ")" << std::endl;
            auto& plates = simulator.GetPlates();
            std::cerr << "DEBUG: Number of plates: " << plates.size() << std::endl;
            
            // Debug plate information and test containment manually
            for (size_t i = 0; i < plates.size(); i++) {
                const auto& plateInfo = plates[i];
                bool contains = plateInfo.ContainsPosition(pos);
                float distance = glm::length(pos - plateInfo.centerPosition);
                float estimatedRadius = std::sqrt(plateInfo.area / 3.14159f);
                std::cerr << "DEBUG: Plate " << i << " (ID:" << plateInfo.plateId << ") center: (" 
                         << plateInfo.centerPosition.x << ", " << plateInfo.centerPosition.y 
                         << "), boundary points: " << plateInfo.boundary.size() 
                         << ", distance: " << distance 
                         << ", area: " << plateInfo.area
                         << ", radius: " << estimatedRadius
                         << ", contains: " << (contains ? "YES" : "NO") << std::endl;
            }
            return false;
        }
        
        // Terrain type should be valid
        if (static_cast<int>(terrain) < 0 || static_cast<int>(terrain) >= static_cast<int>(TerrainType::COUNT)) {
            std::cerr << "DEBUG: Invalid terrain type " << static_cast<int>(terrain) << " at position (" << pos.x << ", " << pos.y << ")" << std::endl;
            return false;
        }
    }
    
    return true;
}

// Test plate boundary detection
bool TestPlateBoundaryDetection() {
    TectonicSimulator simulator;
    simulator.InitializePlates(22222, 200.0f);
    simulator.SimulatePlateMovement(1.0f, 1); // Minimal simulation to establish boundaries
    
    const auto& boundaries = simulator.GetBoundaries();
    
    // Should have detected some boundaries
    if (boundaries.empty()) {
        std::cerr << "DEBUG: No boundaries detected. Number of plates: " << simulator.GetPlates().size() << std::endl;
        return false;
    }
    
    std::cerr << "DEBUG: Found " << boundaries.size() << " boundaries between " << simulator.GetPlates().size() << " plates" << std::endl;
    
    // Verify boundary data
    for (const auto& boundary : boundaries) {
        if (boundary.plate1Id == boundary.plate2Id) {
            return false; // Plates should be different
        }
        
        if (boundary.interactionStrength < 0.0f || boundary.interactionStrength > 1.0f) {
            return false; // Interaction strength should be normalized
        }
        
        if (static_cast<int>(boundary.type) < 0 || static_cast<int>(boundary.type) >= static_cast<int>(BoundaryType::PASSIVE) + 1) {
            return false; // Boundary type should be valid
        }
    }
    
    return true;
}

// Test utility functions
bool TestUtilityFunctions() {
    // Test string conversion functions
    if (std::string(PlateTypeToString(PlateType::OCEANIC)) != "Oceanic") {
        return false;
    }
    
    if (StringToPlateType("Continental") != PlateType::CONTINENTAL) {
        return false;
    }
    
    if (std::string(TerrainTypeToString(TerrainType::MOUNTAIN)) != "Mountain") {
        return false;
    }
    
    if (StringToTerrainType("Rift") != TerrainType::RIFT) {
        return false;
    }
    
    // Test movement generation
    glm::vec2 movement1 = GenerateTypicalMovement(PlateType::OCEANIC, 12345);
    glm::vec2 movement2 = GenerateTypicalMovement(PlateType::OCEANIC, 12345);
    
    // Should be deterministic
    if (glm::distance(movement1, movement2) > 0.001f) {
        return false;
    }
    
    return true;
}

// Performance test
bool TestTectonicSimulatorPerformance() {
    TectonicSimulator simulator;
    
    // Test with large world
    auto startTime = std::chrono::high_resolution_clock::now();
    simulator.InitializePlates(99999, 1000.0f); // 1000km x 1000km world
    simulator.SimulatePlateMovement(200.0f, 20); // 200 million years, 20 steps
    simulator.GenerateBoundaryFeatures();
    auto endTime = std::chrono::high_resolution_clock::now();
    
    auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(endTime - startTime);
    std::cout << "Large world simulation took: " << duration.count() << " ms" << std::endl;
    
    // Should complete within reasonable time (30 seconds for large world)
    return duration.count() < 30000;
}

int main() {
    std::cout << "=== Tectonic Simulation System Tests ===" << std::endl;
    
    bool allTestsPassed = true;
    
    // Run all tests
    allTestsPassed &= PrintTestResult("TectonicPlate Basics", TestTectonicPlateBasics());
    allTestsPassed &= PrintTestResult("TectonicPlate Containment", TestTectonicPlateContainment());
    allTestsPassed &= PrintTestResult("TectonicPlate Serialization", TestTectonicPlateSerialization());
    allTestsPassed &= PrintTestResult("TectonicSimulator Initialization", TestTectonicSimulatorInitialization());
    allTestsPassed &= PrintTestResult("TectonicSimulator Determinism", TestTectonicSimulatorDeterminism());
    allTestsPassed &= PrintTestResult("TectonicSimulator Simulation", TestTectonicSimulatorSimulation());
    allTestsPassed &= PrintTestResult("TectonicSimulator Queries", TestTectonicSimulatorQueries());
    allTestsPassed &= PrintTestResult("Plate Boundary Detection", TestPlateBoundaryDetection());
    allTestsPassed &= PrintTestResult("Utility Functions", TestUtilityFunctions());
    allTestsPassed &= PrintTestResult("Performance Test", TestTectonicSimulatorPerformance());
    
    std::cout << std::endl;
    if (allTestsPassed) {
        std::cout << "🎉 ALL TESTS PASSED! Tectonic simulation system is working correctly." << std::endl;
        return 0;
    } else {
        std::cout << "❌ SOME TESTS FAILED! Please check the implementation." << std::endl;
        return 1;
    }
}

// Helper function to return test result
bool PrintTestResult(const std::string& testName, bool passed) {
    std::cout << "[" << (passed ? "PASS" : "FAIL") << "] " << testName << std::endl;
    if (!passed) {
        std::cerr << "TEST FAILED: " << testName << std::endl;
    }
    return passed;
}
