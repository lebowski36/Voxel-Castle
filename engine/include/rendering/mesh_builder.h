#ifndef VOXEL_ENGINE_MESH_BUILDER_H
#define VOXEL_ENGINE_MESH_BUILDER_H

#include "rendering/voxel_mesh.h"
#include "rendering/texture_atlas.h"
#include "world/chunk_segment.h"
#include <vector>
#include <cstdint>
#include <functional>

namespace VoxelEngine {
    namespace Rendering {

        /**
         * @class MeshBuilder
         * @brief Responsible for generating a VoxelMesh from voxel data structures like ChunkSegment.
         * 
         * This class uses the two-phase greedy meshing algorithm which provides optimal
         * performance and mesh quality by ensuring no faces are missed while still 
         * providing the benefits of face merging.
         */
        class MeshBuilder {
        public:
            /**
             * @brief Constructs a VoxelMesh from a given ChunkSegment using two-phase greedy meshing.
             *
             * @param segment The VoxelCastle::World::ChunkSegment to generate the mesh from.
             * @param atlas The VoxelEngine::Rendering::TextureAtlas to use for UV mapping.
             * @param getVoxel A function to get voxel data, potentially looking into adjacent chunks.
             * @param chunkCoords The coordinates of the chunk being meshed.
             * @return A VoxelMesh containing the geometry for the segment.
             */
            static VoxelMesh buildMesh(
                const VoxelCastle::World::ChunkSegment& segment,
                const TextureAtlas& atlas,
                const std::function<VoxelEngine::World::Voxel(int, int, int)>& getVoxel,
                const glm::ivec3& chunkCoords
            );

            /**
             * @brief Method for two-phase greedy meshing - the recommended meshing method.
             * This is the primary method that provides optimal performance and quality.
             */
            static VoxelMesh buildTwoPhaseGreedyMesh(
                const VoxelCastle::World::ChunkSegment& segment, 
                const TextureAtlas& atlas, 
                const std::function<VoxelEngine::World::Voxel(int, int, int)>& getVoxel,
                const glm::ivec3& chunkCoords
            );

        private:

            /**
             * @brief Helper function to add a single face (composed of two triangles) to a VoxelMesh.
             * This is used internally by the meshing algorithms.
             *
             * @param mesh The VoxelMesh to which the face will be added.
             * @param voxel_pos The world position of the minimum corner of the voxel to which this face belongs.
             * @param face_vertices An array of 4 glm::vec3 points defining the corners of the face.
             * @param normal The normal vector for this face.
             * @param voxelType The VoxelEngine::World::VoxelType of the voxel, used for texture lookup.
             * @param atlas The VoxelEngine::Rendering::TextureAtlas used to get UV coordinates.
             * @param debugLight Light value for debugging.
             */
            static void addFace(VoxelMesh& mesh,
                                const glm::vec3& voxel_pos,
                                const glm::vec3 face_vertices[4],
                                const glm::vec3& normal,
                                VoxelEngine::World::VoxelType voxelType,
                                const TextureAtlas& atlas,
                                float debugLight);

            /**
             * @brief Helper function to add a quadrilateral (composed of two triangles) to a VoxelMesh.
             * This is used internally by the meshing algorithms for merged faces.
             *
             * @param mesh The VoxelMesh to which the quad will be added.
             * @param p1 The first vertex position of the quad.
             * @param p2 The second vertex position of the quad.
             * @param p3 The third vertex position of the quad.
             * @param p4 The fourth vertex position of the quad.
             * @param normal The normal vector for this quad.
             * @param voxelType The VoxelEngine::World::VoxelType of the original voxels.
             * @param atlas The VoxelEngine::Rendering::TextureAtlas used to get UV coordinates.
             * @param quad_width_voxels Width of the quad in voxel units.
             * @param quad_height_voxels Height of the quad in voxel units.
             * @param chunkCoords Chunk coordinates.
             * @param debugLight Light value for debugging.
             */
            static void addQuad(VoxelMesh& mesh,
                                const ::VoxelEngine::World::VoxelPosition& p1, const ::VoxelEngine::World::VoxelPosition& p2,
                                const ::VoxelEngine::World::VoxelPosition& p3, const ::VoxelEngine::World::VoxelPosition& p4,
                                const ::VoxelEngine::World::Normal& normal,
                                ::VoxelEngine::World::VoxelType voxelType,
                                const TextureAtlas& atlas,
                                int quad_width_voxels, 
                                int quad_height_voxels,
                                const glm::ivec3& chunkCoords,
                                float debugLight);
        };

    } // namespace Rendering
} // namespace VoxelEngine

#endif //VOXEL_ENGINE_MESH_BUILDER_H
