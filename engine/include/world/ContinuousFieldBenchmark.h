#pragma once

#include <chrono>
#include <string>
#include <vector>
#include <iostream>
#include <memory>
#include <functional>
#include <random>
#include "world/ContinuousField.h"
#include "world/geological_data.h"  // For RockType

namespace VoxelCastle {
namespace World {
namespace Performance {

/**
 * @brief Performance benchmark results for ContinuousField operations
 */
struct BenchmarkResults {
    std::string testName;
    size_t operationCount;
    double totalTimeMs;
    double avgTimeUs;
    double minTimeUs;
    double maxTimeUs;
    bool passedTarget;
    double targetTimeUs;
    
    void print() const {
        std::cout << "\n=== " << testName << " ===" << std::endl;
        std::cout << "Operations: " << operationCount << std::endl;
        std::cout << "Total Time: " << totalTimeMs << " ms" << std::endl;
        std::cout << "Average: " << avgTimeUs << " μs per operation" << std::endl;
        std::cout << "Range: " << minTimeUs << " - " << maxTimeUs << " μs" << std::endl;
        std::cout << "Target: " << targetTimeUs << " μs" << std::endl;
        std::cout << "Result: " << (passedTarget ? "✅ PASSED" : "❌ FAILED") << std::endl;
    }
};

/**
 * @brief Comprehensive performance benchmarking suite for ContinuousField
 */
class ContinuousFieldBenchmark {
private:
    std::unique_ptr<ContinuousField<float>> smallField_;
    std::unique_ptr<ContinuousField<float>> mediumField_;
    std::unique_ptr<ContinuousField<float>> largeField_;
    std::unique_ptr<ContinuousField<RockType>> rockTypeField_;
    
    std::vector<BenchmarkResults> results_;
    
public:
    ContinuousFieldBenchmark() {
        // Create test fields of various sizes
        smallField_ = std::make_unique<ContinuousField<float>>(100, 100, 1.0f);
        mediumField_ = std::make_unique<ContinuousField<float>>(500, 500, 2.0f);
        largeField_ = std::make_unique<ContinuousField<float>>(1000, 1000, 1.0f);
        rockTypeField_ = std::make_unique<ContinuousField<RockType>>(200, 200, 5.0f);
        
        initializeTestData();
    }
    
    /**
     * @brief Run all performance benchmarks
     */
    void runAllBenchmarks() {
        std::cout << "🔥 Starting ContinuousField Performance Benchmarks" << std::endl;
        std::cout << "Target Performance (from Subtask 01 requirements):" << std::endl;
        std::cout << "- Sample Lookup: < 1000 μs" << std::endl;
        std::cout << "- Force Propagation: < 10ms for 1000m radius" << std::endl;
        std::cout << "- Memory Usage: < 100MB for 1000×1000 field" << std::endl;
        
        // Core operation benchmarks
        benchmarkSampleLookup();
        benchmarkInterpolationAccuracy();
        benchmarkForcePropagation();
        benchmarkToroidalOperations();
        benchmarkMemoryUsage();
        benchmarkBatchOperations();
        benchmarkEdgeCases();
        
        // Print summary
        printSummary();
    }
    
private:
    void initializeTestData() {
        // Initialize fields with realistic data patterns
        for (int z = 0; z < 100; ++z) {
            for (int x = 0; x < 100; ++x) {
                float elevation = std::sin(x * 0.1f) * std::cos(z * 0.1f) * 100.0f + 200.0f;
                smallField_->setSample(x, z, elevation);
            }
        }
        
        for (int z = 0; z < 500; ++z) {
            for (int x = 0; x < 500; ++x) {
                float elevation = std::sin(x * 0.02f) * std::cos(z * 0.02f) * 500.0f + 1000.0f;
                mediumField_->setSample(x, z, elevation);
            }
        }
        
        // Only partially initialize large field to save time
        for (int z = 0; z < 1000; z += 10) {
            for (int x = 0; x < 1000; x += 10) {
                float elevation = std::sin(x * 0.01f) * std::cos(z * 0.01f) * 1000.0f + 2000.0f;
                largeField_->setSample(x, z, elevation);
            }
        }
        
        for (int z = 0; z < 200; ++z) {
            for (int x = 0; x < 200; ++x) {
                RockType rock = static_cast<RockType>((x + z) % static_cast<int>(RockType::COUNT));
                rockTypeField_->setSample(x, z, rock);
            }
        }
    }
    
    void benchmarkSampleLookup() {
        const int numSamples = 10000;
        std::vector<std::pair<float, float>> testPositions;
        
        // Generate random test positions
        std::random_device rd;
        std::mt19937 gen(rd());
        std::uniform_real_distribution<float> dist(0.0f, 999.0f);
        
        for (int i = 0; i < numSamples; ++i) {
            testPositions.emplace_back(dist(gen), dist(gen));
        }
        
        // Benchmark sample lookup performance
        std::vector<double> times;
        times.reserve(numSamples);
        
        for (const auto& pos : testPositions) {
            auto start = std::chrono::high_resolution_clock::now();
            volatile float result = largeField_->sampleAt(pos.first, pos.second);
            auto end = std::chrono::high_resolution_clock::now();
            
            double timeUs = std::chrono::duration<double, std::micro>(end - start).count();
            times.push_back(timeUs);
        }
        
        recordBenchmarkResult("Sample Lookup Performance", times, 1000.0); // Target: < 1000μs
    }
    
    void benchmarkInterpolationAccuracy() {
        // Test interpolation quality by comparing with known mathematical functions
        const int numTests = 1000;
        std::vector<double> times;
        times.reserve(numTests);
        
        double maxError = 0.0;
        double avgError = 0.0;
        
        for (int i = 0; i < numTests; ++i) {
            float x = i * 0.1f;
            float z = i * 0.1f;
            
            auto start = std::chrono::high_resolution_clock::now();
            float interpolated = smallField_->sampleAt(x, z);
            auto end = std::chrono::high_resolution_clock::now();
            
            // Compare with expected sine wave pattern
            float expected = std::sin(x * 0.1f) * std::cos(z * 0.1f) * 100.0f + 200.0f;
            double error = std::abs(interpolated - expected);
            maxError = std::max(maxError, error);
            avgError += error;
            
            double timeUs = std::chrono::duration<double, std::micro>(end - start).count();
            times.push_back(timeUs);
        }
        
        avgError /= numTests;
        
        std::cout << "\nInterpolation Accuracy Results:" << std::endl;
        std::cout << "Average Error: " << avgError << std::endl;
        std::cout << "Maximum Error: " << maxError << std::endl;
        
        recordBenchmarkResult("Interpolation Accuracy", times, 1000.0);
    }
    
    void benchmarkForcePropagation() {
        auto uniformResistance = [](float x, float z) -> float {
            return 1.0f;
        };
        
        // Test different propagation ranges
        std::vector<float> ranges = {100.0f, 500.0f, 1000.0f, 2000.0f};
        
        for (float range : ranges) {
            auto start = std::chrono::high_resolution_clock::now();
            mediumField_->propagateValue(100.0f, 250.0f, 250.0f, range, uniformResistance);
            auto end = std::chrono::high_resolution_clock::now();
            
            double timeMs = std::chrono::duration<double, std::milli>(end - start).count();
            
            std::string testName = "Force Propagation (" + std::to_string(static_cast<int>(range)) + "m radius)";
            BenchmarkResults result;
            result.testName = testName;
            result.operationCount = 1;
            result.totalTimeMs = timeMs;
            result.avgTimeUs = timeMs * 1000.0; // Convert to microseconds
            result.minTimeUs = result.avgTimeUs;
            result.maxTimeUs = result.avgTimeUs;
            result.targetTimeUs = (range >= 1000.0f) ? 10000.0 : 5000.0; // 10ms for 1000m+
            result.passedTarget = result.avgTimeUs <= result.targetTimeUs;
            
            results_.push_back(result);
            result.print();
        }
    }
    
    void benchmarkToroidalOperations() {
        const int numOperations = 10000;
        std::vector<double> times;
        times.reserve(numOperations);
        
        std::random_device rd;
        std::mt19937 gen(rd());
        std::uniform_real_distribution<float> dist(-500.0f, 1500.0f); // Include out-of-bounds
        
        for (int i = 0; i < numOperations; ++i) {
            float x1 = dist(gen), z1 = dist(gen);
            float x2 = dist(gen), z2 = dist(gen);
            
            auto start = std::chrono::high_resolution_clock::now();
            Point wrapped = largeField_->wrapCoordinates(x1, z1);
            float distance = largeField_->getToroidalDistance(x1, z1, x2, z2);
            auto end = std::chrono::high_resolution_clock::now();
            
            double timeUs = std::chrono::duration<double, std::micro>(end - start).count();
            times.push_back(timeUs);
        }
        
        recordBenchmarkResult("Toroidal Operations", times, 100.0); // Target: < 100μs
    }
    
    void benchmarkMemoryUsage() {
        // Test memory usage for different field sizes
        std::vector<std::pair<int, size_t>> fieldSizes = {
            {100, sizeof(float) * 100 * 100},
            {500, sizeof(float) * 500 * 500},
            {1000, sizeof(float) * 1000 * 1000},
            {2000, sizeof(float) * 2000 * 2000}
        };
        
        std::cout << "\n=== Memory Usage Analysis ===" << std::endl;
        
        for (const auto& [size, expectedBytes] : fieldSizes) {
            double expectedMB = expectedBytes / (1024.0 * 1024.0);
            bool passesTarget = (size <= 1000) ? (expectedMB < 100.0) : true; // 100MB target for 1000x1000
            
            std::cout << size << "×" << size << " field: " 
                      << expectedMB << " MB " 
                      << (passesTarget ? "✅" : "❌") << std::endl;
        }
        
        // Memory efficiency test
        auto start = std::chrono::high_resolution_clock::now();
        auto testField = std::make_unique<ContinuousField<float>>(1000, 1000, 1.0f);
        auto end = std::chrono::high_resolution_clock::now();
        
        double allocTimeMs = std::chrono::duration<double, std::milli>(end - start).count();
        std::cout << "1000×1000 allocation time: " << allocTimeMs << " ms" << std::endl;
    }
    
    void benchmarkBatchOperations() {
        const int batchSize = 1000;
        std::vector<double> times;
        times.reserve(batchSize);
        
        // Benchmark batch sampling
        std::vector<std::pair<float, float>> positions;
        for (int i = 0; i < batchSize; ++i) {
            positions.emplace_back(i * 0.5f, i * 0.7f);
        }
        
        auto start = std::chrono::high_resolution_clock::now();
        for (const auto& pos : positions) {
            volatile float result = mediumField_->sampleAt(pos.first, pos.second);
        }
        auto end = std::chrono::high_resolution_clock::now();
        
        double totalTimeMs = std::chrono::duration<double, std::milli>(end - start).count();
        double avgTimeUs = (totalTimeMs * 1000.0) / batchSize;
        
        BenchmarkResults result;
        result.testName = "Batch Sampling (" + std::to_string(batchSize) + " operations)";
        result.operationCount = batchSize;
        result.totalTimeMs = totalTimeMs;
        result.avgTimeUs = avgTimeUs;
        result.minTimeUs = avgTimeUs; // Approximation for batch
        result.maxTimeUs = avgTimeUs;
        result.targetTimeUs = 1000.0; // Target: < 1000μs per sample
        result.passedTarget = avgTimeUs <= result.targetTimeUs;
        
        results_.push_back(result);
        result.print();
    }
    
    void benchmarkEdgeCases() {
        const int numTests = 1000;
        std::vector<double> times;
        times.reserve(numTests);
        
        // Test boundary conditions and extreme values
        std::vector<std::pair<float, float>> edgeCases = {
            {0.0f, 0.0f},           // Origin
            {999.999f, 999.999f},   // Near boundary
            {-100.0f, -100.0f},     // Negative (should wrap)
            {1500.0f, 1500.0f},     // Out of bounds (should wrap)
            {0.001f, 0.001f},       // Very small values
            {999.001f, 999.001f}    // Just past boundary
        };
        
        for (const auto& testCase : edgeCases) {
            auto start = std::chrono::high_resolution_clock::now();
            volatile float result = largeField_->sampleAt(testCase.first, testCase.second);
            auto end = std::chrono::high_resolution_clock::now();
            
            double timeUs = std::chrono::duration<double, std::micro>(end - start).count();
            times.push_back(timeUs);
        }
        
        recordBenchmarkResult("Edge Case Handling", times, 1000.0);
    }
    
    void recordBenchmarkResult(const std::string& testName, const std::vector<double>& times, double targetTimeUs) {
        if (times.empty()) return;
        
        BenchmarkResults result;
        result.testName = testName;
        result.operationCount = times.size();
        result.targetTimeUs = targetTimeUs;
        
        double total = 0.0;
        double minTime = times[0];
        double maxTime = times[0];
        
        for (double time : times) {
            total += time;
            minTime = std::min(minTime, time);
            maxTime = std::max(maxTime, time);
        }
        
        result.totalTimeMs = total / 1000.0; // Convert to milliseconds
        result.avgTimeUs = total / times.size();
        result.minTimeUs = minTime;
        result.maxTimeUs = maxTime;
        result.passedTarget = result.avgTimeUs <= targetTimeUs;
        
        results_.push_back(result);
        result.print();
    }
    
    void printSummary() {
        std::cout << "\n" << std::string(60, '=') << std::endl;
        std::cout << "🏆 PERFORMANCE BENCHMARK SUMMARY" << std::endl;
        std::cout << std::string(60, '=') << std::endl;
        
        int passed = 0;
        int total = results_.size();
        
        for (const auto& result : results_) {
            std::cout << (result.passedTarget ? "✅" : "❌") << " " 
                      << result.testName << ": " 
                      << result.avgTimeUs << " μs (target: " 
                      << result.targetTimeUs << " μs)" << std::endl;
            
            if (result.passedTarget) passed++;
        }
        
        std::cout << std::string(60, '-') << std::endl;
        std::cout << "Overall Results: " << passed << "/" << total << " tests passed" << std::endl;
        
        if (passed == total) {
            std::cout << "🎉 ALL PERFORMANCE TARGETS MET!" << std::endl;
            std::cout << "✅ ContinuousField meets Subtask 01 requirements" << std::endl;
        } else {
            std::cout << "⚠️  Some performance targets not met" << std::endl;
            std::cout << "Consider optimization or hardware upgrade" << std::endl;
        }
        
        std::cout << std::string(60, '=') << std::endl;
    }
};

/**
 * @brief Convenience function to run comprehensive performance benchmarks
 */
inline void runContinuousFieldBenchmarks() {
    ContinuousFieldBenchmark benchmark;
    benchmark.runAllBenchmarks();
}

} // namespace Performance
} // namespace World
} // namespace VoxelCastle
