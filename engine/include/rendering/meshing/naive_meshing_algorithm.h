#ifndef VOXEL_ENGINE_NAIVE_MESHING_ALGORITHM_H
#define VOXEL_ENGINE_NAIVE_MESHING_ALGORITHM_H

#include "rendering/meshing/meshing_algorithm.h"

namespace VoxelEngine {
    namespace Rendering {
        namespace Meshing {

            /**
             * @class NaiveMeshingAlgorithm
             * @brief Simple 1:1 voxel to cube meshing algorithm
             * 
             * This algorithm generates one cube per voxel without any optimization.
             * It's the simplest approach but results in the highest vertex count.
             * Good for debugging and reference implementation.
             */
            class NaiveMeshingAlgorithm : public MeshingAlgorithm {
            public:
                VoxelMesh generateMesh(
                    const VoxelCastle::World::ChunkSegment& segment,
                    const TextureAtlas& atlas,
                    const std::function<VoxelEngine::World::Voxel(int, int, int)>& getVoxel,
                    const glm::ivec3& chunkCoords
                ) override;

                MeshingAlgorithmType getType() const override {
                    return MeshingAlgorithmType::NAIVE;
                }

                const char* getName() const override {
                    return "Naive Meshing";
                }

                const char* getDescription() const override {
                    return "Simple 1:1 voxel to cube mapping. High vertex count but reliable.";
                }

            private:
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

#endif // VOXEL_ENGINE_NAIVE_MESHING_ALGORITHM_H
