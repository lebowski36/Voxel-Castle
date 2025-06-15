#include "world/GeologicalSnapshot.h"
#include <iostream>
#include <algorithm>
#include <chrono>

namespace VoxelCastle {
namespace World {

GeologicalSnapshot::GeologicalSnapshot(int width, int height, float spacing, 
                                      float simTime, const std::string& phase, 
                                      int step, float completion)
    : simulationTime(simTime)
    , phaseDescription(phase)
    , stepNumber(step)
    , completionPercentage(completion)
    , generationTimeMs(0.0f) {
    
    // Initialize the continuous fields
    elevationData = std::make_unique<ContinuousField<float>>(width, height, spacing);
    rockTypeData = std::make_unique<ContinuousField<RockType>>(width, height, spacing);
    mantleStressData = std::make_unique<ContinuousField<float>>(width, height, spacing);
    
    // ===== Step 4.1.2: Initialize Water System Fields =====
    surfaceWaterDepth = std::make_unique<ContinuousField<float>>(width, height, spacing);
    precipitationField = std::make_unique<ContinuousField<float>>(width, height, spacing);
    groundwaterTable = std::make_unique<ContinuousField<float>>(width, height, spacing);
    waterFlowField = std::make_unique<ContinuousField<float>>(width, height, spacing);
    sedimentLoadField = std::make_unique<ContinuousField<float>>(width, height, spacing);
}

float GeologicalSnapshot::GetElevationAt(float worldX, float worldZ) const {
    if (!elevationData) return 0.0f;
    return elevationData->sampleAt(worldX, worldZ);
}

RockType GeologicalSnapshot::GetRockTypeAt(float worldX, float worldZ) const {
    if (!rockTypeData) return RockType::SEDIMENTARY_SANDSTONE;
    return rockTypeData->sampleAt(worldX, worldZ);
}

float GeologicalSnapshot::GetMantleStressAt(float worldX, float worldZ) const {
    if (!mantleStressData) return 0.0f;
    return mantleStressData->sampleAt(worldX, worldZ);
}

// ===== Step 4.1.2: Water System Access Methods =====
float GeologicalSnapshot::GetSurfaceWaterAt(float worldX, float worldZ) const {
    if (!surfaceWaterDepth) return 0.0f;
    return surfaceWaterDepth->sampleAt(worldX, worldZ);
}

float GeologicalSnapshot::GetPrecipitationAt(float worldX, float worldZ) const {
    if (!precipitationField) return 0.0f;
    return precipitationField->sampleAt(worldX, worldZ);
}

float GeologicalSnapshot::GetGroundwaterAt(float worldX, float worldZ) const {
    if (!groundwaterTable) return 0.0f;
    return groundwaterTable->sampleAt(worldX, worldZ);
}

float GeologicalSnapshot::GetWaterFlowAt(float worldX, float worldZ) const {
    if (!waterFlowField) return 0.0f;
    return waterFlowField->sampleAt(worldX, worldZ);
}

float GeologicalSnapshot::GetSedimentLoadAt(float worldX, float worldZ) const {
    if (!sedimentLoadField) return 0.0f;
    return sedimentLoadField->sampleAt(worldX, worldZ);
}

GeologicalSnapshotManager::GeologicalSnapshotManager(float worldSizeKm, int simulationWidth, 
                                                   int simulationHeight, float spacing)
    : currentSnapshotIndex_(0)
    , isGenerating_(false)
    , maxSnapshots_(15)  // Allow up to 15 snapshots for detailed progression
    , worldSizeKm_(worldSizeKm)
    , simulationWidth_(simulationWidth)
    , simulationHeight_(simulationHeight)
    , spacing_(spacing) {
    
    snapshots_.reserve(maxSnapshots_);
    std::cout << "[GeologicalSnapshotManager] Initialized for " << worldSizeKm 
              << "km world, " << simulationWidth << "x" << simulationHeight 
              << " resolution, max " << maxSnapshots_ << " snapshots" << std::endl;
}

void GeologicalSnapshotManager::AddSnapshot(const ContinuousField<float>& elevationField,
                                           const ContinuousField<RockType>& rockTypeField, 
                                           const ContinuousField<float>& mantleStressField,
                                           // ===== Step 4.1.2: Water System Fields =====
                                           const ContinuousField<float>& surfaceWaterField,
                                           const ContinuousField<float>& precipitationField,
                                           const ContinuousField<float>& groundwaterField,
                                           const ContinuousField<float>& waterFlowField,
                                           const ContinuousField<float>& sedimentLoadField,
                                           float simulationTime,
                                           const std::string& phaseDescription,
                                           int stepNumber,
                                           float completionPercentage) {
    
    auto startTime = std::chrono::high_resolution_clock::now();
    
    // Create new snapshot
    auto snapshot = std::make_unique<GeologicalSnapshot>(
        simulationWidth_, simulationHeight_, spacing_,
        simulationTime, phaseDescription, stepNumber, completionPercentage
    );
    
    // Copy data from simulation fields
    // This is the expensive operation, but it's done once per snapshot
    for (int z = 0; z < simulationHeight_; ++z) {
        for (int x = 0; x < simulationWidth_; ++x) {
            float elevation = elevationField.getSample(x, z);
            RockType rockType = rockTypeField.getSample(x, z);
            float mantleStress = mantleStressField.getSample(x, z);
            
            // ===== Step 4.1.2: Copy Water System Data =====
            float surfaceWater = surfaceWaterField.getSample(x, z);
            float precipitation = precipitationField.getSample(x, z);
            float groundwater = groundwaterField.getSample(x, z);
            float waterFlow = waterFlowField.getSample(x, z);
            float sedimentLoad = sedimentLoadField.getSample(x, z);
            
            snapshot->elevationData->setSample(x, z, elevation);
            snapshot->rockTypeData->setSample(x, z, rockType);
            snapshot->mantleStressData->setSample(x, z, mantleStress);
            
            // Set water system data
            snapshot->surfaceWaterDepth->setSample(x, z, surfaceWater);
            snapshot->precipitationField->setSample(x, z, precipitation);
            snapshot->groundwaterTable->setSample(x, z, groundwater);
            snapshot->waterFlowField->setSample(x, z, waterFlow);
            snapshot->sedimentLoadField->setSample(x, z, sedimentLoad);
        }
    }
    
    auto endTime = std::chrono::high_resolution_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::microseconds>(endTime - startTime);
    snapshot->generationTimeMs = duration.count() / 1000.0f;
    
    // Remove oldest snapshot if we've reached the limit
    if (snapshots_.size() >= maxSnapshots_) {
        snapshots_.erase(snapshots_.begin());
        if (currentSnapshotIndex_ > 0) {
            currentSnapshotIndex_--;
        }
    }
    
    // Add the new snapshot
    snapshots_.push_back(std::move(snapshot));
    currentSnapshotIndex_ = snapshots_.size() - 1;  // Show newest snapshot by default
    
    std::cout << "[GeologicalSnapshotManager] Added snapshot " << stepNumber 
              << " (" << phaseDescription << ") - " 
              << snapshots_.back()->generationTimeMs << "ms generation time, "
              << snapshots_.size() << " total snapshots" << std::endl;
}

const GeologicalSnapshot* GeologicalSnapshotManager::GetCurrentSnapshot() const {
    if (snapshots_.empty()) return nullptr;
    if (currentSnapshotIndex_ >= snapshots_.size()) return nullptr;
    return snapshots_[currentSnapshotIndex_].get();
}

const GeologicalSnapshot* GeologicalSnapshotManager::GetSnapshot(size_t index) const {
    if (index >= snapshots_.size()) return nullptr;
    return snapshots_[index].get();
}

bool GeologicalSnapshotManager::NextSnapshot() {
    if (snapshots_.empty()) return false;
    if (currentSnapshotIndex_ + 1 >= snapshots_.size()) return false;
    
    currentSnapshotIndex_++;
    std::cout << "[GeologicalSnapshotManager] Advanced to snapshot " 
              << currentSnapshotIndex_ << "/" << snapshots_.size() << std::endl;
    return true;
}

bool GeologicalSnapshotManager::PreviousSnapshot() {
    if (snapshots_.empty()) return false;
    if (currentSnapshotIndex_ == 0) return false;
    
    currentSnapshotIndex_--;
    std::cout << "[GeologicalSnapshotManager] Moved back to snapshot " 
              << currentSnapshotIndex_ << "/" << snapshots_.size() << std::endl;
    return true;
}

bool GeologicalSnapshotManager::SetCurrentSnapshot(size_t index) {
    if (index >= snapshots_.size()) return false;
    
    currentSnapshotIndex_ = index;
    std::cout << "[GeologicalSnapshotManager] Jumped to snapshot " 
              << currentSnapshotIndex_ << "/" << snapshots_.size() << std::endl;
    return true;
}

void GeologicalSnapshotManager::Clear() {
    snapshots_.clear();
    currentSnapshotIndex_ = 0;
    isGenerating_ = false;
    std::cout << "[GeologicalSnapshotManager] Cleared all snapshots" << std::endl;
}

float GeologicalSnapshotManager::GetGenerationProgress() const {
    if (snapshots_.empty()) return 0.0f;
    return snapshots_.back()->completionPercentage;
}

std::string GeologicalSnapshotManager::GetCurrentPhaseDescription() const {
    const auto* snapshot = GetCurrentSnapshot();
    if (!snapshot) return "No geological data";
    return snapshot->phaseDescription;
}

} // namespace World
} // namespace VoxelCastle
