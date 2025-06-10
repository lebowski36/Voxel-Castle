#ifndef VOXELCASTLE_WORLD_COORDINATES_H
#define VOXELCASTLE_WORLD_COORDINATES_H

#include <cstdint>

namespace VoxelCastle {
namespace World {

/**
 * @brief World coordinate system constants and utilities.
 * 
 * This defines the coordinate system for the Voxel Castle world generation,
 * implementing the symmetrical ±256 chunk vertical range as documented in
 * BIOME_IMPLEMENTATION_PLAN.md and 08c_world_generation_system.md.
 */
namespace WorldCoordinates {

    // ============================================================================
    // CORE SCALE CONSTANTS
    // ============================================================================
    
    /// Voxel dimensions in meters
    constexpr float VOXEL_SIZE_METERS = 0.25f;  // 25cm × 25cm × 25cm per voxel
    
    /// Chunk dimensions in voxels
    constexpr int32_t CHUNK_SIZE_VOXELS = 32;   // 32×32×32 voxels per chunk
    
    /// Chunk dimensions in meters  
    constexpr float CHUNK_SIZE_METERS = CHUNK_SIZE_VOXELS * VOXEL_SIZE_METERS;  // 8m × 8m × 8m per chunk
    
    // ============================================================================
    // WORLD HEIGHT BOUNDARIES (±256 chunks)
    // ============================================================================
    
    /// Minimum chunk Y coordinate (deep underground)
    constexpr int32_t MIN_CHUNK_Y = -256;
    
    /// Maximum chunk Y coordinate (high altitude)
    constexpr int32_t MAX_CHUNK_Y = 255;
    
    /// Total number of chunk layers in the world
    constexpr int32_t TOTAL_CHUNK_LAYERS = MAX_CHUNK_Y - MIN_CHUNK_Y + 1;  // 512 chunks
    
    /// Sea level chunk Y coordinate
    constexpr int32_t SEA_LEVEL_CHUNK_Y = 0;
    
    // ============================================================================
    // WORLD BOUNDARIES IN DIFFERENT UNITS
    // ============================================================================
    
    /// World boundaries in meters
    constexpr float MIN_WORLD_Y_METERS = MIN_CHUNK_Y * CHUNK_SIZE_METERS;      // -2048m
    constexpr float MAX_WORLD_Y_METERS = (MAX_CHUNK_Y + 1) * CHUNK_SIZE_METERS; // +2048m
    constexpr float TOTAL_WORLD_HEIGHT_METERS = MAX_WORLD_Y_METERS - MIN_WORLD_Y_METERS; // 4096m
    
    /// World boundaries in blocks/voxels
    constexpr int32_t MIN_WORLD_Y_BLOCKS = MIN_CHUNK_Y * CHUNK_SIZE_VOXELS;    // -8192 blocks
    constexpr int32_t MAX_WORLD_Y_BLOCKS = (MAX_CHUNK_Y + 1) * CHUNK_SIZE_VOXELS - 1; // +8191 blocks
    constexpr int32_t TOTAL_WORLD_HEIGHT_BLOCKS = MAX_WORLD_Y_BLOCKS - MIN_WORLD_Y_BLOCKS + 1; // 16384 blocks
    
    /// Sea level in different units
    constexpr float SEA_LEVEL_METERS = SEA_LEVEL_CHUNK_Y * CHUNK_SIZE_METERS;  // 0m
    constexpr int32_t SEA_LEVEL_BLOCKS = SEA_LEVEL_CHUNK_Y * CHUNK_SIZE_VOXELS; // 0 blocks
    
    // ============================================================================
    // COORDINATE CONVERSION UTILITIES
    // ============================================================================
    
    /**
     * @brief Convert world block Y coordinate to chunk Y coordinate.
     * @param blockY World Y coordinate in blocks
     * @return Chunk Y coordinate
     */
    inline int32_t BlockYToChunkY(int32_t blockY) {
        // Use floor division to handle negative coordinates correctly
        if (blockY >= 0) {
            return blockY / CHUNK_SIZE_VOXELS;
        } else {
            return (blockY + 1) / CHUNK_SIZE_VOXELS - 1;
        }
    }
    
    /**
     * @brief Convert chunk Y coordinate to the minimum world block Y in that chunk.
     * @param chunkY Chunk Y coordinate
     * @return Minimum world Y coordinate (in blocks) for this chunk
     */
    inline int32_t ChunkYToMinBlockY(int32_t chunkY) {
        return chunkY * CHUNK_SIZE_VOXELS;
    }
    
    /**
     * @brief Convert chunk Y coordinate to the maximum world block Y in that chunk.
     * @param chunkY Chunk Y coordinate
     * @return Maximum world Y coordinate (in blocks) for this chunk
     */
    inline int32_t ChunkYToMaxBlockY(int32_t chunkY) {
        return (chunkY + 1) * CHUNK_SIZE_VOXELS - 1;
    }
    
    /**
     * @brief Get the local Y coordinate within a chunk from world block Y.
     * @param blockY World Y coordinate in blocks
     * @return Local Y coordinate within the chunk (0-31)
     */
    inline int32_t BlockYToLocalY(int32_t blockY) {
        int32_t localY = blockY % CHUNK_SIZE_VOXELS;
        // Handle negative coordinates correctly
        if (localY < 0) {
            localY += CHUNK_SIZE_VOXELS;
        }
        return localY;
    }
    
    /**
     * @brief Check if a chunk Y coordinate is within valid world bounds.
     * @param chunkY Chunk Y coordinate to validate
     * @return True if the chunk Y is within valid bounds
     */
    inline bool IsValidChunkY(int32_t chunkY) {
        return chunkY >= MIN_CHUNK_Y && chunkY <= MAX_CHUNK_Y;
    }
    
    /**
     * @brief Check if a block Y coordinate is within valid world bounds.
     * @param blockY Block Y coordinate to validate
     * @return True if the block Y is within valid bounds
     */
    inline bool IsValidBlockY(int32_t blockY) {
        return blockY >= MIN_WORLD_Y_BLOCKS && blockY <= MAX_WORLD_Y_BLOCKS;
    }
    
    /**
     * @brief Clamp a chunk Y coordinate to valid world bounds.
     * @param chunkY Chunk Y coordinate to clamp
     * @return Clamped chunk Y coordinate
     */
    inline int32_t ClampChunkY(int32_t chunkY) {
        if (chunkY < MIN_CHUNK_Y) return MIN_CHUNK_Y;
        if (chunkY > MAX_CHUNK_Y) return MAX_CHUNK_Y;
        return chunkY;
    }
    
    /**
     * @brief Clamp a block Y coordinate to valid world bounds.
     * @param blockY Block Y coordinate to clamp
     * @return Clamped block Y coordinate
     */
    inline int32_t ClampBlockY(int32_t blockY) {
        if (blockY < MIN_WORLD_Y_BLOCKS) return MIN_WORLD_Y_BLOCKS;
        if (blockY > MAX_WORLD_Y_BLOCKS) return MAX_WORLD_Y_BLOCKS;
        return blockY;
    }

    // ============================================================================
    // COORDINATE VALIDATION HELPERS
    // ============================================================================
    
    /**
     * @brief Validate and convert world coordinates to chunk coordinates.
     * @param worldX World X coordinate
     * @param worldY World Y coordinate  
     * @param worldZ World Z coordinate
     * @param chunkX Output chunk X coordinate
     * @param chunkY Output chunk Y coordinate
     * @param chunkZ Output chunk Z coordinate
     * @return True if coordinates are valid and conversion succeeded
     */
    inline bool WorldToChunkCoordinates(int64_t worldX, int64_t worldY, int64_t worldZ,
                                       int32_t& chunkX, int32_t& chunkY, int32_t& chunkZ) {
        // Validate Y coordinate is within world bounds
        if (!IsValidBlockY(static_cast<int32_t>(worldY))) {
            return false;
        }
        
        // Convert to chunk coordinates
        chunkX = static_cast<int32_t>(worldX / CHUNK_SIZE_VOXELS);
        chunkY = BlockYToChunkY(static_cast<int32_t>(worldY));
        chunkZ = static_cast<int32_t>(worldZ / CHUNK_SIZE_VOXELS);
        
        return true;
    }
    
    /**
     * @brief Get debug information about world coordinate boundaries.
     * @return String containing coordinate system information
     */
    const char* GetCoordinateSystemInfo();

} // namespace WorldCoordinates
} // namespace World
} // namespace VoxelCastle

#endif // VOXELCASTLE_WORLD_COORDINATES_H
