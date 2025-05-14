#ifndef VOXEL_ENGINE_MESH_BUILDER_H
#define VOXEL_ENGINE_MESH_BUILDER_H

#include "rendering/voxel_mesh.h"
#include "rendering/texture_atlas.h" // Added include for TextureAtlas
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
             * faces for any side not adjacent to another solid voxel. This method is simpler
             * but results in a higher vertex count compared to optimized methods.
             *
             * @param segment The VoxelCastle::World::ChunkSegment to generate the mesh from.
             * @param atlas The VoxelEngine::Rendering::TextureAtlas to use for UV mapping.
             * @return A VoxelMesh containing the geometry for the segment.
             */
            static VoxelMesh buildNaiveMesh(const VoxelCastle::World::ChunkSegment& segment, const TextureAtlas& atlas);

            /**
             * @brief Constructs a VoxelMesh from a given ChunkSegment using a greedy meshing approach.
             *
             * This method attempts to combine adjacent coplanar faces of the same voxel type
             * into larger quads. This significantly reduces the total number of vertices and indices,
             * leading to better rendering performance.
             *
             * @param segment The VoxelCastle::World::ChunkSegment to generate the mesh from.
             * @param atlas The VoxelEngine::Rendering::TextureAtlas to use for UV mapping.
             * @return A VoxelMesh containing the optimized geometry for the segment.
             */
            static VoxelMesh buildGreedyMesh(const VoxelCastle::World::ChunkSegment& segment, const TextureAtlas& atlas);

        private:
            /**
             * @brief Helper function to add a single face (composed of two triangles) to a VoxelMesh.
             * This is typically used by the naive meshing algorithm.
             *
             * @param mesh The VoxelMesh to which the face will be added.
             * @param voxel_pos The world position of the minimum corner of the voxel to which this face belongs.
             *                  Face vertices are defined relative to this position.
             * @param face_vertices An array of 4 glm::vec3 points defining the corners of the face,
             *                      typically in local voxel coordinates (e.g., from (0,0,0) to (1,1,1)).
             * @param normal The normal vector for this face.
             * @param voxelType The VoxelEngine::World::VoxelType of the voxel, used for texture lookup.
             * @param atlas The VoxelEngine::Rendering::TextureAtlas used to get UV coordinates.
             */
            static void addFace(VoxelMesh& mesh,
                                const glm::vec3& voxel_pos,
                                const glm::vec3 face_vertices[4], // Matches implementation
                                const glm::vec3& normal,
                                VoxelEngine::World::VoxelType voxelType,
                                const TextureAtlas& atlas);

            /**
             * @brief Helper function to add a quadrilateral (composed of two triangles) to a VoxelMesh.
             * This is typically used by the greedy meshing algorithm for adding merged faces.
             *
             * @param mesh The VoxelMesh to which the quad will be added.
             * @param p1 The first vertex position of the quad.
             * @param p2 The second vertex position of the quad.
             * @param p3 The third vertex position of the quad.
             * @param p4 The fourth vertex position of the quad.
             *           Vertices should be provided in an order that respects the desired winding (typically counter-clockwise for front faces).
             * @param normal The normal vector for this quad.
             * @param voxelType The VoxelEngine::World::VoxelType of the original voxels forming this quad, used for texture lookup.
             * @param atlas The VoxelEngine::Rendering::TextureAtlas used to get UV coordinates.
             * @param quad_width_voxels Width of the quad in voxel units.
             * @param quad_height_voxels Height of the quad in voxel units.
             */
            static void addQuad(VoxelMesh& mesh,
                                const ::VoxelEngine::World::VoxelPosition& p1, const ::VoxelEngine::World::VoxelPosition& p2,
                                const ::VoxelEngine::World::VoxelPosition& p3, const ::VoxelEngine::World::VoxelPosition& p4,
                                const ::VoxelEngine::World::Normal& normal,
                                ::VoxelEngine::World::VoxelType voxelType,
                                const TextureAtlas& atlas,
                                int quad_width_voxels, // Added: Width of the quad in voxel units
                                int quad_height_voxels // Added: Height of the quad in voxel units
                                );
        };

    } // namespace Rendering
} // namespace VoxelEngine

#endif //VOXEL_ENGINE_MESH_BUILDER_H
