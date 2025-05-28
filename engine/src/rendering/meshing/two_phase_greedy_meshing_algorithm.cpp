#include "rendering/meshing/two_phase_greedy_meshing_algorithm.h"
#include "world/voxel_types.h"
#include "world/chunk_segment.h"
#include "rendering/debug_render_mode.h"
#include "rendering/face_debug_utils.h"
#include "rendering/DebugText.h"
#include <glm/glm.hpp>
#include <iostream>
#include <cassert>

namespace VoxelEngine {
    namespace Rendering {
        namespace Meshing {

            VoxelMesh TwoPhaseGreedyMeshingAlgorithm::generateMesh(
                const VoxelCastle::World::ChunkSegment& segment,
                const TextureAtlas& atlas,
                const std::function<VoxelEngine::World::Voxel(int, int, int)>& getVoxel,
                const glm::ivec3& chunkCoords
            ) {
                VoxelMesh mesh;
                mesh.clear();

                const int chunk_dims[] = {
                    VoxelCastle::World::ChunkSegment::getWidth(),
                    VoxelCastle::World::ChunkSegment::getHeight(),
                    VoxelCastle::World::ChunkSegment::getDepth()
                };

                // For each axis (d = 0:X, 1:Y, 2:Z)
                for (int d = 0; d < 3; ++d) {
                    int u = (d + 1) % 3;
                    int v = (d + 2) % 3;

                    // Sweep both directions: +1 (front face), -1 (back face)
                    for (int dir = -1; dir <= 1; dir += 2) {
                        int q[3] = {0, 0, 0};
                        q[d] = dir;

                        // Phase 1: Build visibility mask
                        std::vector<FaceInfo> visibilityMask(chunk_dims[u] * chunk_dims[v]);
                        buildVisibilityMask(visibilityMask, d, u, v, dir, chunk_dims, q, getVoxel);

                        // Phase 2: Apply greedy merging to visible faces
                        processVisibleFaces(mesh, visibilityMask, d, u, v, dir, chunk_dims, q, atlas, chunkCoords, getVoxel);
                    }
                }

                return mesh;
            }

            void TwoPhaseGreedyMeshingAlgorithm::buildVisibilityMask(
                std::vector<FaceInfo>& visibilityMask,
                int d, int u, int v, int dir,
                const int chunk_dims[3],
                const int q[3],
                const std::function<VoxelEngine::World::Voxel(int, int, int)>& getVoxel
            ) {
                // This function now builds a mask for a SINGLE slice at a time
                // The visibility mask should be called once per slice, not for all slices
                // This is handled by the calling function processVisibleFaces
                
                // NOTE: This function is now called per slice from processVisibleFaces
                // and the slice_d parameter is passed via the visibilityMask context
                // We need to refactor this approach - see processVisibleFaces
            }

            void TwoPhaseGreedyMeshingAlgorithm::processVisibleFaces(
                VoxelMesh& mesh,
                const std::vector<FaceInfo>& visibilityMask,
                int d, int u, int v, int dir,
                const int chunk_dims[3],
                const int q[3],
                const TextureAtlas& atlas,
                const glm::ivec3& chunkCoords,
                const std::function<VoxelEngine::World::Voxel(int, int, int)>& getVoxel
            ) {
                // Process each slice individually to maintain proper 3D positioning
                for (int slice_d = 0; slice_d < chunk_dims[d]; ++slice_d) {
                    // Build visibility mask for this specific slice only
                    std::vector<FaceInfo> sliceVisibilityMask(chunk_dims[u] * chunk_dims[v]);
                    std::fill(sliceVisibilityMask.begin(), sliceVisibilityMask.end(), FaceInfo());
                    
                    // Check each position in this slice
                    for (int pos_v = 0; pos_v < chunk_dims[v]; ++pos_v) {
                        for (int pos_u = 0; pos_u < chunk_dims[u]; ++pos_u) {
                            int coords[3] = {0, 0, 0};
                            coords[d] = slice_d;
                            coords[u] = pos_u;
                            coords[v] = pos_v;

                            // Check if this face should be visible
                            if (isFaceVisible(coords, q, getVoxel)) {
                                VoxelEngine::World::Voxel voxel = getVoxel(coords[0], coords[1], coords[2]);
                                VoxelEngine::World::VoxelType voxelType = static_cast<VoxelEngine::World::VoxelType>(voxel.id);
                                
                                int maskIndex = pos_v * chunk_dims[u] + pos_u;
                                sliceVisibilityMask[maskIndex] = FaceInfo(voxelType);
                            }
                        }
                    }
                    
                    // Create a processed mask for this slice
                    std::vector<bool> processedMask(chunk_dims[u] * chunk_dims[v], false);

                    // Process visible faces in this slice with greedy merging
                    for (int pos_v = 0; pos_v < chunk_dims[v]; ++pos_v) {
                        for (int pos_u = 0; pos_u < chunk_dims[u]; ++pos_u) {
                            int maskIndex = pos_v * chunk_dims[u] + pos_u;
                            
                            // Skip if already processed or not visible
                            if (processedMask[maskIndex] || !sliceVisibilityMask[maskIndex].isVisible) {
                                continue;
                            }

                            VoxelEngine::World::VoxelType currentType = sliceVisibilityMask[maskIndex].voxelType;
                            
                            // Compute greedy quad dimensions for this slice
                            int h_quad, w_quad;
                            computeQuadDimensions(sliceVisibilityMask, pos_u, pos_v, h_quad, w_quad, chunk_dims, currentType, u, v);

                            // Generate the quad at the correct slice position
                            int x[3] = {0, 0, 0};
                            x[d] = slice_d;  // Use the actual slice position
                            x[u] = pos_u;
                            x[v] = pos_v;
                            
                            generateQuad(mesh, d, u, v, dir, x, q, h_quad, w_quad, currentType, atlas, chunkCoords);

                            // Mark all faces covered by this quad as processed
                            for (int l_v = 0; l_v < w_quad; ++l_v) {
                                for (int m_u = 0; m_u < h_quad; ++m_u) {
                                    int mark_index = (pos_v + l_v) * chunk_dims[u] + (pos_u + m_u);
                                    if (mark_index >= 0 && mark_index < static_cast<int>(processedMask.size())) {
                                        processedMask[mark_index] = true;
                                    }
                                }
                            }
                        }
                    }
                }
            }

            bool TwoPhaseGreedyMeshingAlgorithm::isFaceVisible(
                const int coords[3],
                const int q[3],
                const std::function<VoxelEngine::World::Voxel(int, int, int)>& getVoxel
            ) {
                // Get the current voxel and its neighbor in the q direction
                VoxelEngine::World::Voxel voxel1 = getVoxel(coords[0], coords[1], coords[2]);
                VoxelEngine::World::Voxel voxel2 = getVoxel(coords[0] + q[0], coords[1] + q[1], coords[2] + q[2]);
                
                bool is_voxel1_solid = (voxel1.id != static_cast<uint8_t>(VoxelEngine::World::VoxelType::AIR));
                bool is_voxel2_solid = (voxel2.id != static_cast<uint8_t>(VoxelEngine::World::VoxelType::AIR));

                // Face is visible if voxel1 is solid and voxel2 is air
                return is_voxel1_solid && !is_voxel2_solid;
            }

            void TwoPhaseGreedyMeshingAlgorithm::computeQuadDimensions(
                const std::vector<FaceInfo>& visibilityMask,
                int x_u, int x_v,
                int& h_quad, int& w_quad,
                const int chunk_dims[3],
                VoxelEngine::World::VoxelType voxelType,
                int u, int v
            ) {
                // Find width (h_quad) in u direction
                h_quad = 1;
                for (; x_u + h_quad < chunk_dims[u]; ++h_quad) {
                    int checkIndex = x_v * chunk_dims[u] + (x_u + h_quad);
                    if (checkIndex >= static_cast<int>(visibilityMask.size()) ||
                        !visibilityMask[checkIndex].isVisible ||
                        visibilityMask[checkIndex].voxelType != voxelType) {
                        break;
                    }
                }

                // Find height (w_quad) in v direction
                w_quad = 1;
                for (; x_v + w_quad < chunk_dims[v]; ++w_quad) {
                    // Check that the entire next row is valid
                    bool valid_row = true;
                    for (int k_u = 0; k_u < h_quad; ++k_u) {
                        int checkIndex = (x_v + w_quad) * chunk_dims[u] + (x_u + k_u);
                        if (checkIndex >= static_cast<int>(visibilityMask.size()) ||
                            !visibilityMask[checkIndex].isVisible ||
                            visibilityMask[checkIndex].voxelType != voxelType) {
                            valid_row = false;
                            break;
                        }
                    }
                    if (!valid_row) break;
                }
            }

            void TwoPhaseGreedyMeshingAlgorithm::generateQuad(
                VoxelMesh& mesh,
                int d, int u, int v, int dir,
                const int x[3], const int q[3],
                int h_quad, int w_quad,
                VoxelEngine::World::VoxelType voxelType,
                const TextureAtlas& atlas,
                const glm::ivec3& chunkCoords
            ) {
                // Compute quad corners
                float quad_plane_coord_d = static_cast<float>(x[d] + (dir > 0 ? 1 : 0));

                VoxelEngine::World::VoxelPosition vp_base;
                vp_base.x = static_cast<float>(x[0]);
                vp_base.y = static_cast<float>(x[1]);
                vp_base.z = static_cast<float>(x[2]);

                if (d == 0) vp_base.x = quad_plane_coord_d;
                else if (d == 1) vp_base.y = quad_plane_coord_d;
                else vp_base.z = quad_plane_coord_d;

                float du_vec[3] = {0,0,0}; du_vec[u] = static_cast<float>(h_quad);
                float dv_vec[3] = {0,0,0}; dv_vec[v] = static_cast<float>(w_quad);

                VoxelEngine::World::VoxelPosition ovp1, ovp2, ovp3, ovp4;
                ovp1 = vp_base;
                ovp2.x = vp_base.x + dv_vec[0]; ovp2.y = vp_base.y + dv_vec[1]; ovp2.z = vp_base.z + dv_vec[2];
                ovp3.x = vp_base.x + dv_vec[0] + du_vec[0]; ovp3.y = vp_base.y + dv_vec[1] + du_vec[1]; ovp3.z = vp_base.z + dv_vec[2] + du_vec[2];
                ovp4.x = vp_base.x + du_vec[0]; ovp4.y = vp_base.y + du_vec[1]; ovp4.z = vp_base.z + du_vec[2];

                VoxelEngine::World::Normal normal_val;
                normal_val.x = static_cast<float>(q[0]);
                normal_val.y = static_cast<float>(q[1]);
                normal_val.z = static_cast<float>(q[2]);

                float debugLight = 1.0f;

                if (dir > 0) { // Front face (normal points in +d direction)
                    addQuad(mesh, ovp1, ovp4, ovp3, ovp2, normal_val, voxelType, atlas, h_quad, w_quad, chunkCoords, debugLight);
                } else { // Back face (normal points in -d direction)
                    addQuad(mesh, ovp1, ovp2, ovp3, ovp4, normal_val, voxelType, atlas, h_quad, w_quad, chunkCoords, debugLight);
                }
            }

            void TwoPhaseGreedyMeshingAlgorithm::addQuad(VoxelMesh& mesh,
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
                                                        float debugLight) {

                uint32_t base_index = static_cast<uint32_t>(mesh.vertices.size());

                float light = debugLight;
                TextureCoordinates texCoords = atlas.getTextureCoordinates(voxelType);
                glm::vec2 atlas_origin_uv = texCoords.getBottomLeft();

                // quad_uvs are now 0-W and 0-H
                glm::vec2 quad_uvs[4];
                quad_uvs[0] = glm::vec2(0.0f, 0.0f);
                quad_uvs[1] = glm::vec2(static_cast<float>(quad_width_voxels), 0.0f);
                quad_uvs[2] = glm::vec2(static_cast<float>(quad_width_voxels), static_cast<float>(quad_height_voxels));
                quad_uvs[3] = glm::vec2(0.0f, static_cast<float>(quad_height_voxels));

                glm::vec4 debugColor(0.0f);
                if (::g_debugRenderMode == DebugRenderMode::FACE_DEBUG) {
                    // Determine direction ID based on normal
                    int directionId = 0;
                    if (normal.x > 0.5f) directionId = 1;      // +X (Right)
                    else if (normal.x < -0.5f) directionId = 2; // -X (Left)
                    else if (normal.y > 0.5f) directionId = 3;  // +Y (Top)
                    else if (normal.y < -0.5f) directionId = 4; // -Y (Bottom)
                    else if (normal.z > 0.5f) directionId = 5;  // +Z (Front)
                    else if (normal.z < -0.5f) directionId = 6; // -Z (Back)
                    debugColor = VoxelEngine::Rendering::encodeFaceDebugColor(directionId);
                }
                
                mesh.vertices.emplace_back(p1, normal, quad_uvs[0], atlas_origin_uv, light, debugColor);
                mesh.vertices.emplace_back(p2, normal, quad_uvs[1], atlas_origin_uv, light, debugColor);
                mesh.vertices.emplace_back(p3, normal, quad_uvs[2], atlas_origin_uv, light, debugColor);
                mesh.vertices.emplace_back(p4, normal, quad_uvs[3], atlas_origin_uv, light, debugColor);

                // Standard quad triangulation
                mesh.indices.push_back(base_index);
                mesh.indices.push_back(base_index + 1);
                mesh.indices.push_back(base_index + 2);

                mesh.indices.push_back(base_index);
                mesh.indices.push_back(base_index + 2);
                mesh.indices.push_back(base_index + 3);

                if (::g_debugRenderMode == DebugRenderMode::FACE_DEBUG) {
                    glm::vec3 faceCenterLocal = (static_cast<glm::vec3>(p1) + static_cast<glm::vec3>(p2) + static_cast<glm::vec3>(p3) + static_cast<glm::vec3>(p4)) / 4.0f;
                    
                    char coordText[256];
                    snprintf(coordText, sizeof(coordText), "C(%.0f,%.0f,%.0f)V(%.0f,%.0f,%.0f)", 
                             static_cast<float>(chunkCoords.x), static_cast<float>(chunkCoords.y), static_cast<float>(chunkCoords.z),
                             p1.x, p1.y, p1.z);
                    
                    mesh.debugFaceTexts.push_back({std::string(coordText), faceCenterLocal, normal});
                }
            }

        } // namespace Meshing
    } // namespace Rendering
} // namespace VoxelEngine
