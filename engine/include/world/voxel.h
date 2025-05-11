// filepath: engine/include/world/voxel.h
#pragma once

#include <cstdint>

namespace VoxelEngine {
    namespace World {

        // Represents a single voxel in the world
        struct Voxel {
            // Type ID for the voxel (e.g., 0 for air, 1 for stone, 2 for dirt)
            uint8_t id; 
            // Add other per-voxel data here in the future, e.g.:
            // uint8_t light_level;
            // uint8_t health;

            // Default constructor: initializes to an air block (or inactive)
            Voxel() : id(0) {}

            // Constructor with a specific type
            Voxel(uint8_t type_id) : id(type_id) {}

            // Check if the voxel is active (e.g., not air)
            // Assumes id 0 is 'air' or 'inactive'
            bool isActive() const {
                return id != 0;
            }
        };

    } // namespace World
} // namespace VoxelEngine
