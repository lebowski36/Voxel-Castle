#pragma once

#include "world/ContinuousField.h"
#include "world/GeologicalTypes.h"
#include <memory>
#include <random>
#include <string>

namespace VoxelCastle {
namespace World {

/**
 * ErosionEngine - Handles all erosion and weathering processes in the geological simulation
 * Part of the modular architecture for Step 4.1 Simulation Architecture Overhaul
 */
class ErosionEngine {
public:
    struct ErosionFields {
        ContinuousField<float>* elevationField;
        ContinuousField<float>* rockHardness;
        ContinuousField<float>* waterFlow;
        ContinuousField<float>* precipitationField;
        ContinuousField<float>* surfaceWaterDepth;
        ContinuousField<float>* sedimentLoad;
        ContinuousField<float>* erosionRateField;
        ContinuousField<RockType>* rockTypes;
    };

    struct ErosionMetrics {
        float totalErosion = 0.0f;
        float totalDeposition = 0.0f;
        float averageErosionRate = 0.0f;
        float sedimentTransported = 0.0f;
        float weatheringIntensity = 0.0f;
        int activeRivers = 0;
    };

    ErosionEngine(float worldSizeKm, const GeologicalConfig& config, uint64_t seed);
    ~ErosionEngine() = default;

    // Main simulation methods
    void simulateChemicalWeathering(ErosionFields& fields, float timeStepKyears);
    void simulatePhysicalErosion(ErosionFields& fields, float timeStepKyears);
    void simulateWaterDrivenErosion(ErosionFields& fields, float timeStepKyears);
    void simulateSedimentTransport(ErosionFields& fields, float timeStepKyears);
    void simulateSedimentDeposition(ErosionFields& fields, float timeStepKyears);
    void simulateRiverSystems(ErosionFields& fields, float timeStepKyears);
    void simulateGlacialCarving(ErosionFields& fields, float timeStepKyears);
    void simulateMicroWeathering(ErosionFields& fields, float timeStepYears);

    // Integrated erosion-uplift balance
    void simulateErosionUpliftBalance(ErosionFields& fields, float timeStepKyears);

    // Metrics and info
    const ErosionMetrics& getMetrics() const { return metrics_; }
    void updateMetrics(const ErosionFields& fields);

    // Utility methods
    float calculateErosionRate(RockType rockType, float precipitation, float slope) const;
    float calculateSedimentCapacity(float waterFlow, float slope) const;
    void validateAndClampElevation(ErosionFields& fields, int x, int z, const std::string& processName);

private:
    float worldSizeKm_;
    GeologicalConfig config_;
    uint64_t seed_;
    std::mt19937 rng_;
    ErosionMetrics metrics_;

    // Internal simulation helpers
    void applyChemicalWeathering(ErosionFields& fields, int x, int z, float intensity, float timeStep);
    void applyPhysicalErosion(ErosionFields& fields, int x, int z, float intensity, float timeStep);
    void applyWaterErosion(ErosionFields& fields, int x, int z, float waterFlow, float timeStep);
    void transportSediment(ErosionFields& fields, int x, int z, float capacity, float timeStep);
    void depositSediment(ErosionFields& fields, int x, int z, float amount, float timeStep);
    void carveRiverChannel(ErosionFields& fields, int x, int z, float flow, float timeStep);
    void applyGlacialErosion(ErosionFields& fields, int x, int z, float intensity, float timeStep);

    // Helper calculations
    float calculateSlope(const ContinuousField<float>& elevationField, int x, int z) const;
    float calculateWaterFlowVelocity(float waterDepth, float slope) const;
    float getWeatheringResistance(RockType rockType) const;
    float getErosionResistance(RockType rockType) const;
};

}}
