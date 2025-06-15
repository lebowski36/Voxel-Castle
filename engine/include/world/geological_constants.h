#pragma once
#include <cstring>
#include <iostream>
#include <atomic>

namespace VoxelCastle {
namespace World {

// Global warning counters for spam protection
extern std::atomic<int> extreme_elevation_warning_count;
extern std::atomic<bool> elevation_warnings_exceeded;

/**
 * @brief Global geological constants and bounds for realistic Earth-like simulation
 * 
 * This file centralizes all geological parameters to ensure consistency across
 * all geological systems (TectonicEngine, ErosionEngine, etc.) and prevent
 * maintenance issues from scattered constants.
 */
struct GeologicalConstants {
    // ===========================================
    // ELEVATION BOUNDS & TERRAIN LIMITS
    // ===========================================
    
    /** Absolute maximum elevation bound - emergency clamping limit */
    static constexpr float MAX_ELEVATION_BOUND = 2048.0f;  // meters
    
    /** Absolute minimum elevation bound - emergency clamping limit */
    static constexpr float MIN_ELEVATION_BOUND = -2048.0f; // meters
    
    /** Expected maximum terrain elevation under normal conditions */
    static constexpr float EXPECTED_MAX_ELEVATION = 1800.0f; // meters
    
    /** Expected minimum terrain elevation under normal conditions */
    static constexpr float EXPECTED_MIN_ELEVATION = -1800.0f; // meters
    
    /** Sea level reference point */
    static constexpr float SEA_LEVEL = 0.0f; // meters
    
    /** Deep ocean baseline elevation */
    static constexpr float DEEP_OCEAN_BASELINE = -1500.0f; // meters
    
    /** Continental shelf depth */
    static constexpr float CONTINENTAL_SHELF_DEPTH = -200.0f; // meters
    
    // ===========================================
    // GEOLOGICAL TIME SCALING
    // ===========================================
    
    /** Standard geological timestep in years (1 million years) */
    static constexpr float GEOLOGICAL_TIMESTEP_YEARS = 1000000.0f;
    
    /** Conversion factor: thousand years to years */
    static constexpr float KYEARS_TO_YEARS = 1000.0f;
    
    /** Conversion factor: million years to years */
    static constexpr float MYEARS_TO_YEARS = 1000000.0f;
    
    // ===========================================
    // EARTH-LIKE PROCESS RATES
    // ===========================================
    
    /** Maximum realistic tectonic uplift per million years (meters) */
    static constexpr float MAX_TECTONIC_UPLIFT_RATE = 10000.0f; // 10km per million years (extreme)
    
    /** Typical tectonic uplift per million years (meters) */
    static constexpr float TYPICAL_TECTONIC_UPLIFT_RATE = 1000.0f; // 1km per million years
    
    /** Maximum realistic erosion rate per million years (meters) */
    static constexpr float MAX_EROSION_RATE = 100000.0f; // 100km per million years (extreme)
    
    /** Typical erosion rate per million years (meters) */
    static constexpr float TYPICAL_EROSION_RATE = 10000.0f; // 10km per million years
    
    /** Maximum realistic volcanic activity per million years (meters) */
    static constexpr float MAX_VOLCANIC_RATE = 5000.0f; // 5km per million years
    
    // ===========================================
    // GEOLOGICAL STRESS & FORCE LIMITS
    // ===========================================
    
    /** Maximum realistic mantle stress (dimensionless units) */
    static constexpr float MAX_MANTLE_STRESS = 20.0f;
    
    /** Maximum realistic crustal stress (dimensionless units) */
    static constexpr float MAX_CRUSTAL_STRESS = 50.0f;
    
    /** Maximum realistic crustal thickness (meters) */
    static constexpr float MAX_CRUSTAL_THICKNESS = 80000.0f; // 80km
    
    /** Standard continental crustal thickness (meters) */
    static constexpr float STANDARD_CRUSTAL_THICKNESS = 35000.0f; // 35km
    
    // ===========================================
    // HELPER FUNCTIONS
    // ===========================================
    
    /**
     * @brief Clamp elevation to geological bounds
     * @param elevation Raw elevation value
     * @return Elevation clamped to realistic geological bounds
     */
    static float ClampElevation(float elevation) {
        if (elevation > MAX_ELEVATION_BOUND) return MAX_ELEVATION_BOUND;
        if (elevation < MIN_ELEVATION_BOUND) return MIN_ELEVATION_BOUND;
        return elevation;
    }
    
    /**
     * @brief Check if elevation is within expected geological range
     * @param elevation Elevation to check
     * @return True if within expected range, false if extreme
     */
    static bool IsElevationRealistic(float elevation) {
        return elevation >= EXPECTED_MIN_ELEVATION && elevation <= EXPECTED_MAX_ELEVATION;
    }
    
    /**
     * @brief Convert geological timestep to appropriate scale for process
     * @param timestepYears Timestep in years
     * @param processType Type of geological process for scaling
     * @return Scaled timestep for the specific process
     */
    static float ScaleTimestepForProcess(float timestepYears, const char* processType) {
        // All processes use million-year scaling for this simulation
        return timestepYears / MYEARS_TO_YEARS;
    }
    
    /**
     * @brief Get realistic rate limit for geological process
     * @param processType Type of geological process
     * @return Maximum realistic rate per million years
     */
    static float GetProcessRateLimit(const char* processType) {
        if (strcmp(processType, "tectonic") == 0) return MAX_TECTONIC_UPLIFT_RATE;
        if (strcmp(processType, "erosion") == 0) return MAX_EROSION_RATE;
        if (strcmp(processType, "volcanic") == 0) return MAX_VOLCANIC_RATE;
        return 1000.0f; // Default conservative limit
    }
};

// ===========================================
// CONVENIENCE MACROS FOR COMMON OPERATIONS
// ===========================================

/** Clamp elevation to geological bounds - use in all geological systems */
#define CLAMP_GEOLOGICAL_ELEVATION(elev) GeologicalConstants::ClampElevation(elev)

/** Check if elevation needs geological bounds warning with spam protection */
#define WARN_EXTREME_ELEVATION(elev, system, x, z) \
    do { \
        if (!GeologicalConstants::IsElevationRealistic(elev)) { \
            int current_count = extreme_elevation_warning_count.fetch_add(1); \
            if (current_count < 15) { \
                std::cout << "[ELEVATION_WARNING] " << system << " created extreme elevation: " \
                          << elev << "m at (" << x << "," << z << ") - outside expected range" << std::endl; \
            } else if (current_count == 15 && !elevation_warnings_exceeded.exchange(true)) { \
                std::cout << "[ELEVATION_WARNING] Maximum warnings reached (15). Further elevation warnings suppressed to prevent spam." << std::endl; \
            } \
        } \
    } while(0)

/** Scale rate for million-year timesteps */
#define SCALE_FOR_MYEARS(rate) ((rate) / GeologicalConstants::MYEARS_TO_YEARS)

} // namespace World
} // namespace VoxelCastle
