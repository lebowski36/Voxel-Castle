#ifndef VOXEL_ENGINE_MESH_BUILDER_H
#define VOXEL_ENGINE_MESH_BUILDER_H

#include "rendering/voxel_mesh.h"
// Forward declaration to avoid circular dependency if ChunkSegment were to use MeshBuilder directly
// However, MeshBuilder needs the definition to iterate voxels.
#include "world/chunk_segment.h"

namespace VoxelEngine {
    namespace Rendering {

        /**
         * @class MeshBuilder
         * @brief Responsible for generating a VoxelMesh from voxel data structures like ChunkSegment.
         */
        class MeshBuilder {
        public:
            /**
             * @brief Constructs a VoxelMesh from a given ChunkSegment using a naive blocky approach.
             *
             * Iterates through each voxel in the segment. For each solid voxel, it generates
             * faces for any side not adjacent to another solid voxel.
             *
             * @param segment The ChunkSegment to generate the mesh from.
             * @return A VoxelMesh containing the geometry for the segment.
             */
            static VoxelMesh buildNaiveMesh(const VoxelCastle::World::ChunkSegment& segment);

            // Future methods for other meshing algorithms (e.g., greedy meshing) could be added here.
            // static VoxelMesh buildGreedyMesh(const VoxelCastle::World::ChunkSegment& segment);
        };

    } // namespace Rendering
} // namespace VoxelEngine

#endif //VOXEL_ENGINE_MESH_BUILDER_H
