#ifndef VOXEL_ENGINE_MESHING_ALGORITHM_H
#define VOXEL_ENGINE_MESHING_ALGORITHM_H

#include "rendering/voxel_mesh.h"
#include "rendering/texture_atlas.h"
#include "world/chunk_segment.h"
#include "world/voxel.h"
#include <functional>
#include <glm/glm.hpp>

namespace VoxelEngine {
    namespace Rendering {
        namespace Meshing {

            /**
             * @enum MeshingAlgorithmType
             * @brief Enum representing different meshing algorithms
             */
            enum class MeshingAlgorithmType {
                NAIVE,              // Simple 1:1 voxel to cubes
                GREEDY,             // Current greedy meshing implementation  
                CULLED_FACE,        // Naive with face culling optimization
                TWO_PHASE_GREEDY,   // Improved greedy meshing with two-phase approach
                // Future algorithms can be added here
                LOD_AWARE           // Future LOD-aware meshing
            };

            /**
             * @class MeshingAlgorithm
             * @brief Abstract base class for all meshing algorithms
             * 
             * This interface defines the contract that all meshing algorithms must implement.
             * It provides a standardized way to generate VoxelMesh from chunk segment data.
             */
            class MeshingAlgorithm {
            public:
                virtual ~MeshingAlgorithm() = default;

                /**
                 * @brief Generate a mesh from the given chunk segment
                 * 
                 * @param segment The chunk segment to generate mesh from
                 * @param atlas The texture atlas for UV mapping
                 * @param getVoxel Function to get voxel data (including from adjacent chunks)
                 * @param chunkCoords The coordinates of the chunk being meshed
                 * @return Generated VoxelMesh
                 */
                virtual VoxelMesh generateMesh(
                    const VoxelCastle::World::ChunkSegment& segment,
                    const TextureAtlas& atlas,
                    const std::function<VoxelEngine::World::Voxel(int, int, int)>& getVoxel,
                    const glm::ivec3& chunkCoords
                ) = 0;

                /**
                 * @brief Get the algorithm type
                 * @return The type of this meshing algorithm
                 */
                virtual MeshingAlgorithmType getType() const = 0;

                /**
                 * @brief Get a human-readable name for this algorithm
                 * @return Algorithm name
                 */
                virtual const char* getName() const = 0;

                /**
                 * @brief Get performance characteristics info
                 * @return String describing performance trade-offs
                 */
                virtual const char* getDescription() const = 0;
            };

        } // namespace Meshing
    } // namespace Rendering
} // namespace VoxelEngine

#endif // VOXEL_ENGINE_MESHING_ALGORITHM_H
