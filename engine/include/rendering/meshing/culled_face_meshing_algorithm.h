#ifndef VOXEL_ENGINE_CULLED_FACE_MESHING_ALGORITHM_H
#define VOXEL_ENGINE_CULLED_FACE_MESHING_ALGORITHM_H

#include "rendering/meshing/meshing_algorithm.h"

namespace VoxelEngine {
    namespace Rendering {
        namespace Meshing {

            /**
             * @class CulledFaceMeshingAlgorithm
             * @brief Naive meshing with face culling optimization
             * 
             * This algorithm is similar to naive meshing but only generates faces
             * that are actually visible (i.e., faces that border air or chunk boundaries).
             * This provides a middle ground between naive and greedy meshing:
             * - More vertices than greedy meshing
             * - Fewer vertices than naive meshing
             * - More reliable than current greedy implementation
             */
            class CulledFaceMeshingAlgorithm : public MeshingAlgorithm {
            public:
                VoxelMesh generateMesh(
                    const VoxelCastle::World::ChunkSegment& segment,
                    const TextureAtlas& atlas,
                    const std::function<VoxelEngine::World::Voxel(int, int, int)>& getVoxel,
                    const glm::ivec3& chunkCoords
                ) override;

                MeshingAlgorithmType getType() const override {
                    return MeshingAlgorithmType::CULLED_FACE;
                }

                const char* getName() const override {
                    return "Culled Face Meshing";
                }

                const char* getDescription() const override {
                    return "Naive meshing with face culling. Reliable with moderate optimization.";
                }

            private:
                /**
                 * @brief Check if a face should be rendered
                 */
                bool shouldRenderFace(
                    const glm::ivec3& voxelPos,
                    const glm::ivec3& neighborOffset,
                    const std::function<VoxelEngine::World::Voxel(int, int, int)>& getVoxel
                );

                /**
                 * @brief Add a single face to the mesh
                 */
                void addFace(VoxelMesh& mesh,
                            const glm::vec3& voxel_pos,
                            const glm::vec3 face_vertices[4],
                            const glm::vec3& normal,
                            VoxelEngine::World::VoxelType voxelType,
                            const TextureAtlas& atlas,
                            float debugLight = 1.0f);
            };

        } // namespace Meshing
    } // namespace Rendering
} // namespace VoxelEngine

#endif // VOXEL_ENGINE_CULLED_FACE_MESHING_ALGORITHM_H
