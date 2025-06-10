#include <gtest/gtest.h>
#include "world/GeologicalSimulator.h"
#include "world/GeologicalTypes.h"
#include <chrono>
#include <iostream>
#include <iomanip>

using namespace VoxelCastle::World;

class GeologicalSimulationTest : public ::testing::Test {
protected:
    void SetUp() override {
        // Setup balanced quality configuration
        config.preset = GeologicalPreset::PERFORMANCE; // Use fastest preset for testing
        
        simulator = std::make_unique<GeologicalSimulator>(32000, config); // 32000 chunks = 256km world (32000 × 8m = 256km)
        simulator->initialize(12345); // Fixed seed for reproducible results
    }
    
    GeologicalConfig config;
    std::unique_ptr<GeologicalSimulator> simulator;
};

TEST_F(GeologicalSimulationTest, ThreePhaseSimulation) {
    std::vector<PhaseInfo> phaseUpdates;
    
    // Create progress callback for detailed console output
    auto progressCallback = [&phaseUpdates](const PhaseInfo& info) {
        phaseUpdates.push_back(info);
        
        // Real-time progress feedback to console
        std::string phaseName;
        switch(info.currentPhase) {
            case GeologicalPhase::TECTONICS: phaseName = "TECTONICS"; break;
            case GeologicalPhase::MOUNTAIN_BUILDING: phaseName = "MOUNTAIN_BUILDING"; break;
            case GeologicalPhase::EROSION: phaseName = "EROSION"; break;
            case GeologicalPhase::DETAIL: phaseName = "DETAIL"; break;
            default: phaseName = "UNKNOWN"; break;
        }
        
        std::cout << std::flush; // Ensure immediate output
        std::cout << "[GEOLOGICAL_SIM] Phase: " << phaseName 
                  << " | Progress: " << std::fixed << std::setprecision(1) << (info.phaseProgress * 100) << "%"
                  << " | Total: " << std::fixed << std::setprecision(1) << (info.totalProgress * 100) << "%"
                  << " | Process: " << info.currentProcess;
        
        if (info.timeRemaining > 0) {
            std::cout << " | ETA: " << std::fixed << std::setprecision(0) << info.timeRemaining << "s";
        }
        
        std::cout << std::endl << std::flush;
    };
    
    auto startTime = std::chrono::high_resolution_clock::now();
    
    // Run the complete three-phase simulation with progress callback
    simulator->runFullSimulation(progressCallback);
    
    auto endTime = std::chrono::high_resolution_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(endTime - startTime);
    
    std::cout << "Total simulation time: " << duration.count() << "ms" << std::endl;
    
    // Verify we received progress updates
    EXPECT_GT(phaseUpdates.size(), 0) << "Should have received progress updates";
    
    // Verify simulation completed (should reach 100% progress)
    if (!phaseUpdates.empty()) {
        const auto& lastUpdate = phaseUpdates.back();
        EXPECT_EQ(lastUpdate.totalProgress, 1.0f) << "Simulation should complete at 100%";
    }
    
    // Test geological sample generation
    GeologicalSample sample = simulator->getSampleAt(1000.0f, 1000.0f); // Sample at 1km, 1km
    
    // Verify sample contains realistic geological data
    EXPECT_GE(sample.elevation, -2000.0f) << "Elevation should be above ocean floor";
    EXPECT_LE(sample.elevation, 8000.0f) << "Elevation should be below Mt. Everest";
    EXPECT_GE(sample.rockHardness, 0.0f) << "Rock hardness should be non-negative";
    EXPECT_LE(sample.rockHardness, 10.0f) << "Rock hardness should be within realistic range";
    
    std::cout << "Sample at (1000, 1000): Elevation=" << sample.elevation 
              << "m, RockType=" << static_cast<int>(sample.rockType)
              << ", Hardness=" << sample.rockHardness << std::endl;
}

TEST_F(GeologicalSimulationTest, TectonicPhaseOnly) {
    // Test individual phase execution
    std::vector<PhaseInfo> phaseUpdates;
    
    std::cout << "[TEST] Running Tectonic Phase Only test..." << std::endl;
    
    simulator->setProgressCallback([&phaseUpdates](const PhaseInfo& info) {
        phaseUpdates.push_back(info);
        std::cout << "[TECTONIC_TEST] Progress: " << (info.phaseProgress * 100) 
                  << "% | Process: " << info.currentProcess << std::endl;
    });
    
    // Run only the tectonic phase
    std::cout << "[TEST] Starting tectonic simulation (50 million years)..." << std::endl;
    simulator->simulateTectonicPhase(50.0f); // 50 million years
    std::cout << "[TEST] Tectonic simulation complete." << std::endl;
    
    // Verify tectonic effects
    GeologicalSample sample1 = simulator->getSampleAt(0.0f, 0.0f);
    GeologicalSample sample2 = simulator->getSampleAt(1000.0f, 1000.0f);
    
    // After tectonics, we should see elevation variation
    EXPECT_TRUE(std::abs(sample1.elevation - sample2.elevation) > 1.0f) 
        << "Tectonic activity should create elevation differences";
}

TEST_F(GeologicalSimulationTest, ErosionPhaseOnly) {
    std::cout << "[TEST] Running Erosion Phase Only test..." << std::endl;
    
    // First create some elevation variation with tectonics
    std::cout << "[TEST] Creating initial terrain with tectonics..." << std::endl;
    simulator->simulateTectonicPhase(20.0f);
    
    // Get initial elevation at test point
    GeologicalSample beforeErosion = simulator->getSampleAt(500.0f, 500.0f);
    std::cout << "[TEST] Initial elevation: " << beforeErosion.elevation << "m" << std::endl;
    
    // Run erosion phase
    std::cout << "[TEST] Starting erosion simulation (10 thousand years)..." << std::endl;
    simulator->simulateErosionPhase(10.0f); // 10 thousand years
    std::cout << "[TEST] Erosion simulation complete." << std::endl;
    
    // Run erosion phase
    simulator->simulateErosionPhase(10.0f); // 10 thousand years
    
    // Get elevation after erosion
    GeologicalSample afterErosion = simulator->getSampleAt(500.0f, 500.0f);
    
    // Erosion should generally reduce elevation (but not always due to complex processes)
    std::cout << "Before erosion: " << beforeErosion.elevation 
              << "m, After erosion: " << afterErosion.elevation << "m" << std::endl;
    
    // Water flow should be present after erosion simulation
    EXPECT_GE(afterErosion.waterFlow, 0.0f) << "Water flow should be non-negative";
}

TEST_F(GeologicalSimulationTest, GeologicalDataExport) {
    std::cout << "[TEST] Running Geological Data Export test..." << std::endl;
    
    // Run a quick simulation
    std::cout << "[TEST] Running quick simulation for export test..." << std::endl;
    simulator->simulateTectonicPhase(10.0f);
    simulator->simulateErosionPhase(5.0f);
    std::cout << "[TEST] Simulation complete, testing export..." << std::endl;
    
    // Test geological data export (currently returns nullptr, this is expected for Phase 2)
    auto geologicalData = simulator->exportGeologicalData();
    
    // For now, exportGeologicalData returns nullptr as it's not implemented yet
    // This will be implemented in Phase 3 (HybridDetailGenerator)
    EXPECT_EQ(geologicalData, nullptr) << "exportGeologicalData not yet implemented - will be done in Phase 3";
    
    // Instead, test individual sample access which is implemented
    GeologicalSample sample = simulator->getSampleAt(100.0f, 100.0f);
    
    // Verify the sample is reasonable
    EXPECT_GE(sample.elevation, -5000.0f) << "Sample elevation should be within reasonable range";
    EXPECT_LE(sample.elevation, 10000.0f) << "Sample elevation should be within reasonable range";
    
    std::cout << "Sample at (100, 100): Elevation=" << sample.elevation 
              << "m, RockType=" << static_cast<int>(sample.rockType)
              << ", Hardness=" << sample.rockHardness << std::endl;
}

TEST_F(GeologicalSimulationTest, PerformanceMetrics) {
    std::cout << "[TEST] Running Performance Metrics test..." << std::endl;
    
    auto startTime = std::chrono::high_resolution_clock::now();
    
    // Run a performance test with timing
    std::cout << "[TEST] Starting performance simulation..." << std::endl;
    simulator->simulateTectonicPhase(5.0f); // Short simulation for performance test
    std::cout << "[TEST] Performance simulation complete." << std::endl;
    
    auto endTime = std::chrono::high_resolution_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(endTime - startTime);
    
    // Get performance metrics
    const auto& metrics = simulator->getPerformanceMetrics();
    
    std::cout << "Performance Test Results:" << std::endl;
    std::cout << "  Duration: " << duration.count() << "ms" << std::endl;
    std::cout << "  Memory Usage: " << metrics.memoryUsage << "GB" << std::endl;
    std::cout << "  Processing Rate: " << metrics.processingRate << " samples/sec" << std::endl;
    
    // Verify performance is reasonable (these are loose bounds for testing)
    EXPECT_LT(duration.count(), 10000) << "Performance preset should complete quickly";
    EXPECT_LT(metrics.memoryUsage, 2.0f) << "Memory usage should be reasonable for test world";
}
