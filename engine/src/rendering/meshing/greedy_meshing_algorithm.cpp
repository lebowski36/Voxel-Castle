#include "rendering/meshing/greedy_meshing_algorithm.h"
#include "world/voxel_types.h"
#include "world/chunk_segment.h"
#include "rendering/debug_render_mode.h"
#include "rendering/face_debug_utils.h"
#include "rendering/DebugText.h"
#include <glm/glm.hpp>
#include <fstream>
#include <iostream>
#include <cassert>

namespace VoxelEngine {
    namespace Rendering {
        namespace Meshing {

            VoxelMesh GreedyMeshingAlgorithm::generateMesh(
                const VoxelCastle::World::ChunkSegment& segment,
                const TextureAtlas& atlas,
                const std::function<VoxelEngine::World::Voxel(int, int, int)>& getVoxel,
                const glm::ivec3& chunkCoords
            ) {
                // This is the existing greedy meshing implementation moved from MeshBuilder
                VoxelMesh mesh;
                mesh.clear();

                const int chunk_dims[] = {
                    VoxelCastle::World::ChunkSegment::getWidth(),
                    VoxelCastle::World::ChunkSegment::getHeight(),
                    VoxelCastle::World::ChunkSegment::getDepth()
                };

                using VoxelEngine::World::VoxelType;
                using ::VoxelEngine::World::Voxel;

                // For each axis (d = 0:X, 1:Y, 2:Z)
                for (int d = 0; d < 3; ++d) {
                    int u = (d + 1) % 3;
                    int v = (d + 2) % 3;

                    // Sweep both directions: +1 (front face), -1 (back face)
                    for (int dir = -1; dir <= 1; dir += 2) {
                        int x[3] = {0, 0, 0};
                        int q[3] = {0, 0, 0};
                        q[d] = dir;

                        // Initialize mask for this sweep
                        std::vector<bool> mask(chunk_dims[u] * chunk_dims[v], false);

                        // For each slice along d
                        for (x[d] = 0; x[d] < chunk_dims[d]; ++x[d]) {
                            // Process each voxel in the current slice
                            for (x[v] = 0; x[v] < chunk_dims[v]; ++x[v]) {
                                for (x[u] = 0; x[u] < chunk_dims[u]; ++x[u]) {
                                    // Calculate the mask index for the current voxel
                                    int n = x[v] * chunk_dims[u] + x[u];
                                    if (mask[n]) continue;

                                    // Get the voxel and its neighbor in the q direction
                                    Voxel voxel1 = getVoxel(x[0], x[1], x[2]);
                                    Voxel voxel2 = getVoxel(x[0] + q[0], x[1] + q[1], x[2] + q[2]);
                                    bool is_voxel1_solid = (voxel1.id != static_cast<uint8_t>(VoxelType::AIR));
                                    bool is_voxel2_solid = (voxel2.id != static_cast<uint8_t>(VoxelType::AIR));

                                    // Generate a face if voxel1 is solid and voxel2 (neighbor in direction q) is air.
                                    if (is_voxel1_solid && !is_voxel2_solid) {
                                        VoxelType current_face_voxel_type = static_cast<VoxelType>(voxel1.id);

                                        // Find width (h_quad) in u direction
                                        int h_quad = 1;
                                        for (; x[u] + h_quad < chunk_dims[u]; ++h_quad) {
                                            // Calculate the correct mask index for next voxel in u direction
                                            int mask_check_idx_h = (x[v]) * chunk_dims[u] + (x[u] + h_quad);
                                            assert(mask_check_idx_h >= 0 && mask_check_idx_h < static_cast<int>(mask.size()));
                                            
                                            if (mask[mask_check_idx_h]) break;
                                            
                                            // Check the next voxel in the u direction
                                            int cur_coords_u_ext[3] = {x[0], x[1], x[2]}; cur_coords_u_ext[u] += h_quad;
                                            Voxel v_strip_1 = getVoxel(cur_coords_u_ext[0], cur_coords_u_ext[1], cur_coords_u_ext[2]);
                                            Voxel v_strip_2 = getVoxel(cur_coords_u_ext[0] + q[0], cur_coords_u_ext[1] + q[1], cur_coords_u_ext[2] + q[2]);
                                            
                                            bool v_strip_1_solid = (v_strip_1.id != static_cast<uint8_t>(VoxelType::AIR));
                                            bool v_strip_2_solid = (v_strip_2.id != static_cast<uint8_t>(VoxelType::AIR));
                                            if (!v_strip_1_solid || v_strip_2_solid || static_cast<VoxelType>(v_strip_1.id) != current_face_voxel_type) {
                                                break;
                                            }
                                        }

                                        // Find height (w_quad) in v direction
                                        int w_quad = 1;
                                        for (; x[v] + w_quad < chunk_dims[v]; ++w_quad) {
                                            // Check that the entire next row of voxels is valid
                                            bool valid_row = true;
                                            
                                            // Check every voxel in the next row
                                            for (int k_u = 0; k_u < h_quad; ++k_u) {
                                                // Calculate the correct mask index for each voxel in the next row
                                                int mask_check_idx_w = (x[v] + w_quad) * chunk_dims[u] + (x[u] + k_u);
                                                assert(mask_check_idx_w >= 0 && mask_check_idx_w < static_cast<int>(mask.size()));
                                                
                                                // If any voxel in this row is already masked, skip the entire row
                                                if (mask[mask_check_idx_w]) { 
                                                    valid_row = false; 
                                                    break; 
                                                }
                                                
                                                // Check that this voxel is solid, same type, and has an exposed face
                                                int cur_coords_uv_ext[3] = {x[0], x[1], x[2]}; 
                                                cur_coords_uv_ext[u] += k_u; 
                                                cur_coords_uv_ext[v] += w_quad;
                                                
                                                Voxel v_scan_1 = getVoxel(cur_coords_uv_ext[0], cur_coords_uv_ext[1], cur_coords_uv_ext[2]);
                                                Voxel v_scan_2 = getVoxel(cur_coords_uv_ext[0] + q[0], cur_coords_uv_ext[1] + q[1], cur_coords_uv_ext[2] + q[2]);
                                                
                                                bool v_scan_1_solid = (v_scan_1.id != static_cast<uint8_t>(VoxelType::AIR));
                                                bool v_scan_2_solid = (v_scan_2.id != static_cast<uint8_t>(VoxelType::AIR));
                                                
                                                if (!v_scan_1_solid || v_scan_2_solid || static_cast<VoxelType>(v_scan_1.id) != current_face_voxel_type) {
                                                    valid_row = false; 
                                                    break;
                                                }
                                            }
                                            
                                            if (!valid_row) break;
                                        }

                                        // Compute quad corners
                                        float quad_plane_coord_d = static_cast<float>(x[d] + (dir > 0 ? 1 : 0));

                                        ::VoxelEngine::World::VoxelPosition vp_base;
                                        vp_base.x = static_cast<float>(x[0]);
                                        vp_base.y = static_cast<float>(x[1]);
                                        vp_base.z = static_cast<float>(x[2]);

                                        if (d == 0) vp_base.x = quad_plane_coord_d;
                                        else if (d == 1) vp_base.y = quad_plane_coord_d;
                                        else vp_base.z = quad_plane_coord_d;

                                        float du_vec[3] = {0,0,0}; du_vec[u] = static_cast<float>(h_quad);
                                        float dv_vec[3] = {0,0,0}; dv_vec[v] = static_cast<float>(w_quad);

                                        ::VoxelEngine::World::VoxelPosition ovp1, ovp2, ovp3, ovp4;
                                        ovp1 = vp_base;
                                        ovp2.x = vp_base.x + dv_vec[0]; ovp2.y = vp_base.y + dv_vec[1]; ovp2.z = vp_base.z + dv_vec[2];
                                        ovp3.x = vp_base.x + dv_vec[0] + du_vec[0]; ovp3.y = vp_base.y + dv_vec[1] + du_vec[1]; ovp3.z = vp_base.z + dv_vec[2] + du_vec[2];
                                        ovp4.x = vp_base.x + du_vec[0]; ovp4.y = vp_base.y + du_vec[1]; ovp4.z = vp_base.z + du_vec[2];

                                        ::VoxelEngine::World::Normal normal_val;
                                        normal_val.x = static_cast<float>(q[0]);
                                        normal_val.y = static_cast<float>(q[1]);
                                        normal_val.z = static_cast<float>(q[2]);

                                        static std::ofstream mesh_debug_log;
                                        if (!mesh_debug_log.is_open()) {
                                            mesh_debug_log.open("./mesh_debug.log", std::ios::app);
                                            if (!mesh_debug_log.is_open()) {
                                                std::cerr << "[ERROR] Could not open mesh_debug.log for writing!" << std::endl;
                                            }
                                        }
                                        mesh_debug_log << "[DEBUG] Quad: axis=" << d << ", dir=" << dir
                                                      << ", base=(" << x[0] << "," << x[1] << "," << x[2] << ")"
                                                      << ", h_quad=" << h_quad << ", w_quad=" << w_quad
                                                      << ", voxelType=" << static_cast<int>(current_face_voxel_type)
                                                      << ", ovp1=(" << ovp1.x << "," << ovp1.y << "," << ovp1.z << ")"
                                                      << ", ovp2=(" << ovp2.x << "," << ovp2.y << "," << ovp2.z << ")"
                                                      << ", ovp3=(" << ovp3.x << "," << ovp3.y << "," << ovp3.z << ")"
                                                      << ", ovp4=(" << ovp4.x << "," << ovp4.y << "," << ovp4.z << ")"
                                                      << std::endl;
                                        float debugLight = 1.0f; // For faces that are actually rendered

                                        if (dir > 0) { // Front face (normal points in +d direction)
                                            addQuad(mesh, ovp1, ovp4, ovp3, ovp2, normal_val, current_face_voxel_type, atlas, h_quad, w_quad, chunkCoords, debugLight);
                                        } else { // Back face (normal points in -d direction)
                                            addQuad(mesh, ovp1, ovp2, ovp3, ovp4, normal_val, current_face_voxel_type, atlas, h_quad, w_quad, chunkCoords, debugLight);
                                        }

                                        // Mark mask - correctly mark all voxels in the quad
                                        for (int l_v = 0; l_v < w_quad; ++l_v) {
                                            for (int m_u = 0; m_u < h_quad; ++m_u) {
                                                int mask_index = (x[v] + l_v) * chunk_dims[u] + (x[u] + m_u);
                                                assert(mask_index >= 0 && mask_index < static_cast<int>(mask.size()));
                                                
                                                // Double-check the voxel is actually part of our continuous surface
                                                int check_coords[3] = {x[0], x[1], x[2]};
                                                check_coords[u] += m_u;
                                                check_coords[v] += l_v;
                                                
                                                // Only mark the mask if the voxel exists and has the same properties
                                                Voxel check_voxel = getVoxel(check_coords[0], check_coords[1], check_coords[2]);
                                                if (check_voxel.id == static_cast<uint8_t>(current_face_voxel_type)) {
                                                    mask[mask_index] = true;
                                                }
                                            }
                                        }
                                    }
                                }
                            }
                        }
                    }
                }
                return mesh;
            }

            void GreedyMeshingAlgorithm::addQuad(VoxelMesh& mesh,
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
