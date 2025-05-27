#ifndef VOXEL_ENGINE_GREEDY_MESHING_ALGORITHM_H
#define VOXEL_ENGINE_GREEDY_MESHING_ALGORITHM_H

#include "rendering/meshing/meshing_algorithm.h"

namespace VoxelEngine {
    namespace Rendering {
        namespace Meshing {

            /**
             * @class GreedyMeshingAlgorithm
             * @brief Current greedy meshing implementation
             * 
             * This is the existing greedy meshing algorithm moved into the modular system.
             * It has known issues with missing faces that will be addressed by the
             * TwoPhaseGreedyMeshingAlgorithm.
             */
            class GreedyMeshingAlgorithm : public MeshingAlgorithm {
            public:
                VoxelMesh generateMesh(
                    const VoxelCastle::World::ChunkSegment& segment,
                    const TextureAtlas& atlas,
                    const std::function<VoxelEngine::World::Voxel(int, int, int)>& getVoxel,
                    const glm::ivec3& chunkCoords
                ) override;

                MeshingAlgorithmType getType() const override {
                    return MeshingAlgorithmType::GREEDY;
                }

                const char* getName() const override {
                    return "Greedy Meshing (Legacy)";
                }

                const char* getDescription() const override {
                    return "Original greedy meshing implementation. May have missing faces.";
                }

            private:
                /**
                 * @brief Add a quad to the mesh (used by greedy meshing)
                 */
                void addQuad(VoxelMesh& mesh,
                            const VoxelEngine::World::VoxelPosition& p1,
                            const VoxelEngine::World::VoxelPosition& p2,
                            const VoxelEngine::World::VoxelPosition& p3,
                            const VoxelEngine::World::VoxelPosition& p4,
                            const VoxelEngine::World::Normal& normal,
                            VoxelEngine::World::VoxelType voxelType,
                            const TextureAtlas& atlas,
                            int quad_width_voxels,
                            int quad_height_voxels,
                            const glm::ivec3& chunkCoords,
                            float debugLight = 1.0f);
            };

        } // namespace Meshing
    } // namespace Rendering
} // namespace VoxelEngine

#endif // VOXEL_ENGINE_GREEDY_MESHING_ALGORITHM_H
