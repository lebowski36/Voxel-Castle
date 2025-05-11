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
             * @param base_pos Base position of the face.
             * @param vertices Array of 4 vertices defining the face.
             * @param normal Normal vector of the face.
             */
            static void addFace(VoxelMesh& mesh,
                                const glm::vec3& base_pos, // Changed from individual floats
                                const glm::vec3* vertices, // Changed to represent 4 vertices
                                const glm::vec3& normal);  // Changed from individual floats

            /**
             * @brief Helper to add a quad to the mesh.
             *
             * @param mesh The VoxelMesh to add the quad to.
             * @param p1, p2, p3, p4 Positions of the quad vertices.
             * @param normal Normal vector of the quad.
             * @param voxelType Type of the voxel (for potential future use, e.g., texture selection).
             */
            static void addQuad(VoxelMesh& mesh,
                                const ::VoxelEngine::World::VoxelPosition& p1, const ::VoxelEngine::World::VoxelPosition& p2,
                                const ::VoxelEngine::World::VoxelPosition& p3, const ::VoxelEngine::World::VoxelPosition& p4,
                                const ::VoxelEngine::World::Normal& normal,
                                ::VoxelEngine::World::VoxelType voxelType);
        };

    } // namespace Rendering
} // namespace VoxelEngine

#endif //VOXEL_ENGINE_MESH_BUILDER_H
