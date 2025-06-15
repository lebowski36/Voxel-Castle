#pragma once

#include "world/ContinuousField.h"
#include "world/GeologicalTypes.h"
#include "world/FractalContinentGenerator.h"
#include <memory>
#include <random>
#include <string>

namespace VoxelCastle {
namespace World {

/**
 * TectonicEngine - Handles all tectonic processes in the geological simulation
 * Part of the modular architecture for Step 4.1 Simulation Architecture Overhaul
 */
class TectonicEngine {
public:
    struct TectonicFields {
        ContinuousField<float>* mantleStress;
        ContinuousField<float>* crustStress;
        ContinuousField<float>* elevationField;
        ContinuousField<float>* crustalThickness;
        ContinuousField<float>* mantleTemperature;
        ContinuousField<float>* isostasyAdjustment;
        ContinuousField<RockType>* rockTypes;
        ContinuousField<float>* rockHardness;
    };

    struct TectonicMetrics {
        float totalUplift = 0.0f;
        float totalSubsidence = 0.0f;
        float averageCrustalThickness = 35000.0f;
        float volcanicActivity = 0.0f;
        int activeVolcanoes = 0;
        float plateVelocity = 0.0f;
    };

    TectonicEngine(float worldSizeKm, const GeologicalConfig& config, uint64_t seed);
    ~TectonicEngine() = default;

    // Main simulation methods
    void simulateMantleConvection(TectonicFields& fields, float timeStepMyears);
    void simulatePlateMovement(TectonicFields& fields, float timeStepMyears);
    void simulateMountainBuilding(TectonicFields& fields, float timeStepMyears);
    void simulateVolcanicActivity(TectonicFields& fields, float timeStepMyears);
    void simulateIsostasyAdjustment(TectonicFields& fields, float timeStepMyears);

    // Metrics and info
    const TectonicMetrics& getMetrics() const { return metrics_; }
    void updateMetrics(const TectonicFields& fields);

    // Utility methods
    float getRockHardness(RockType rockType) const;
    void validateAndClampElevation(TectonicFields& fields, int x, int z, const std::string& processName);

private:
    float worldSizeKm_;
    GeologicalConfig config_;
    uint64_t seed_;
    std::mt19937 rng_;
    TectonicMetrics metrics_;

    // Internal simulation helpers
    void applyMantleConvectionCell(TectonicFields& fields, int x, int z, float intensity, float timeStep);
    void applyPlateMotion(TectonicFields& fields, int x, int z, float motionX, float motionZ, float timeStep);
    void applyMountainBuilding(TectonicFields& fields, int x, int z, float compressionForce, float timeStep);
    void createVolcanicActivity(TectonicFields& fields, int x, int z, float intensity, float timeStep);
    void applyIsostasyAdjustment(TectonicFields& fields, int x, int z, float adjustment, float timeStep);

    // Noise and pattern generation
    float generateFractalNoise(float x, float z, float scale1, float scale2, float scale3) const;
    float generateDomainWarpedNoise(float x, float z, float scale, float warpIntensity) const;
};

}}
