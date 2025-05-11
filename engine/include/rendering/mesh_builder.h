#ifndef VOXEL_ENGINE_MESH_BUILDER_H
#define VOXEL_ENGINE_MESH_BUILDER_H

#include "rendering/voxel_mesh.h"
// Forward declaration to avoid circular dependency if ChunkSegment were to use MeshBuilder directly
// However, MeshBuilder needs the definition to iterate voxels.
#include "world/chunk_segment.h"
#include <vector>
#include <cstdint> // For uint8_t

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

            /**
             * @brief Constructs a VoxelMesh from a given ChunkSegment using a greedy meshing approach.
             *
             * Combines adjacent faces into larger quads to reduce the number of polygons.
             *
             * @param segment The ChunkSegment to generate the mesh from.
             * @return A VoxelMesh containing the geometry for the segment.
             */
            static VoxelMesh buildGreedyMesh(const VoxelCastle::World::ChunkSegment& segment);

        private:
            /**
             * @brief Helper to add a face to the mesh.
             *
             * @param mesh The VoxelMesh to add the face to.
             * @param x1, y1, z1 Coordinates of the first vertex.
             * @param x2, y2, z2 Coordinates of the second vertex.
             * @param x3, y3, z3 Coordinates of the third vertex.
             * @param x4, y4, z4 Coordinates of the fourth vertex.
             * @param nx, ny, nz Normal vector of the face.
             */
            static void addFace(VoxelMesh& mesh,
                                float x1, float y1, float z1,
                                float x2, float y2, float z2,
                                float x3, float y3, float z3,
                                float x4, float y4, float z4,
                                float nx, float ny, float nz);

            /**
             * @brief Helper to add a quad to the mesh.
             *
             * @param mesh The VoxelMesh to add the quad to.
             * @param p1, p2, p3, p4 Positions of the quad vertices.
             * @param normal Normal vector of the quad.
             * @param voxelType Type of the voxel (for potential future use, e.g., texture selection).
             */
            static void addQuad(VoxelMesh& mesh,
                                const Voxel::VoxelPosition& p1, const Voxel::VoxelPosition& p2,
                                const Voxel::VoxelPosition& p3, const Voxel::VoxelPosition& p4,
                                const Voxel::Normal& normal,
                                VoxelType voxelType);
        };

    } // namespace Rendering
} // namespace VoxelEngine

#endif //VOXEL_ENGINE_MESH_BUILDER_H
