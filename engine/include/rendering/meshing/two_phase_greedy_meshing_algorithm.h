#ifndef VOXEL_ENGINE_TWO_PHASE_GREEDY_MESHING_ALGORITHM_H
#define VOXEL_ENGINE_TWO_PHASE_GREEDY_MESHING_ALGORITHM_H

#include "rendering/meshing/meshing_algorithm.h"
#include <vector>

namespace VoxelEngine {
    namespace Rendering {
        namespace Meshing {

            /**
             * @class TwoPhaseGreedyMeshingAlgorithm
             * @brief Improved greedy meshing with two-phase approach
             * 
             * This algorithm fixes the missing face issues of the original greedy meshing
             * by separating face detection from face merging:
             * 
             * Phase 1: Detect all visible faces and mark them in a visibility mask
             * Phase 2: Apply greedy merging only to the faces marked as visible
             * 
             * This ensures no faces are missed while still providing the performance
             * benefits of greedy meshing.
             */
            class TwoPhaseGreedyMeshingAlgorithm : public MeshingAlgorithm {
            public:
                VoxelMesh generateMesh(
                    const VoxelCastle::World::ChunkSegment& segment,
                    const TextureAtlas& atlas,
                    const std::function<VoxelEngine::World::Voxel(int, int, int)>& getVoxel,
                    const glm::ivec3& chunkCoords
                ) override;

                MeshingAlgorithmType getType() const override {
                    return MeshingAlgorithmType::TWO_PHASE_GREEDY;
                }

                const char* getName() const override {
                    return "Two-Phase Greedy Meshing";
                }

                const char* getDescription() const override {
                    return "Improved greedy meshing that guarantees no missing faces.";
                }

            private:
                /**
                 * @brief Structure to hold face information for Phase 1
                 */
                struct FaceInfo {
                    VoxelEngine::World::VoxelType voxelType;
                    bool isVisible;
                    
                    FaceInfo() : voxelType(VoxelEngine::World::VoxelType::AIR), isVisible(false) {}
                    FaceInfo(VoxelEngine::World::VoxelType type) : voxelType(type), isVisible(true) {}
                };

                /**
                 * @brief Phase 1: Build visibility mask for all faces
                 */
                void buildVisibilityMask(
                    std::vector<FaceInfo>& visibilityMask,
                    int d, int u, int v, int dir,
                    const int chunk_dims[3],
                    const int q[3],
                    const std::function<VoxelEngine::World::Voxel(int, int, int)>& getVoxel
                );

                /**
                 * @brief Phase 2: Apply greedy merging to visible faces
                 */
                void processVisibleFaces(
                    VoxelMesh& mesh,
                    const std::vector<FaceInfo>& visibilityMask,
                    int d, int u, int v, int dir,
                    const int chunk_dims[3],
                    const int q[3],
                    const TextureAtlas& atlas,
                    const glm::ivec3& chunkCoords,
                    const std::function<VoxelEngine::World::Voxel(int, int, int)>& getVoxel
                );

                /**
                 * @brief Helper to determine if a face is visible
                 */
                bool isFaceVisible(
                    const int coords[3],
                    const int q[3],
                    const std::function<VoxelEngine::World::Voxel(int, int, int)>& getVoxel
                );

                /**
                 * @brief Helper to compute greedy quad dimensions
                 */
                void computeQuadDimensions(
                    const std::vector<FaceInfo>& visibilityMask,
                    int x_u, int x_v,
                    int& h_quad, int& w_quad,
                    const int chunk_dims[3],
                    VoxelEngine::World::VoxelType voxelType,
                    int u, int v
                );

                /**
                 * @brief Helper to generate a quad
                 */
                void generateQuad(
                    VoxelMesh& mesh,
                    int d, int u, int v, int dir,
                    const int x[3], const int q[3],
                    int h_quad, int w_quad,
                    VoxelEngine::World::VoxelType voxelType,
                    const TextureAtlas& atlas,
                    const glm::ivec3& chunkCoords
                );

                /**
                 * @brief Add a quad to the mesh with proper texture tiling
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

#endif // VOXEL_ENGINE_TWO_PHASE_GREEDY_MESHING_ALGORITHM_H
