#include "rendering/mesh_builder.h"
#include "world/voxel_types.h" // For VoxelType::AIR
#include "world/chunk_segment.h" // For CHUNK_SIZE constants

// GLM is already included via voxel_mesh.h, but good to be explicit if direct glm types are used
#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp> // For glm::value_ptr if needed later

namespace VoxelEngine {
    namespace Rendering {

        // Helper to add a quad (two triangles) for a given face.
        // Vertices are defined in local voxel coordinates (0 to 1 range for the face)
        // and then offset by voxel_pos.
        // Normal is the same for all 4 vertices of a flat face.
        // Winding order: v1-v2-v3, v1-v3-v4 (CCW from outside)
        void MeshBuilder::addFace(VoxelMesh& mesh,
                     const glm::vec3& voxel_pos, // Base position of the voxel (e.g., its minimum corner)
                     const glm::vec3 face_vertices[4], // The 4 vertices of the face, relative to (0,0,0) if voxel_pos is origin
                     const glm::vec3& normal) {
            
            uint32_t base_index = static_cast<uint32_t>(mesh.vertices.size());

            for (int i = 0; i < 4; ++i) {
                mesh.vertices.emplace_back(voxel_pos + face_vertices[i], normal, glm::vec2(0.0f)); // UVs placeholder for now
            }

            mesh.indices.push_back(base_index);
            mesh.indices.push_back(base_index + 1);
            mesh.indices.push_back(base_index + 2);

            mesh.indices.push_back(base_index);
            mesh.indices.push_back(base_index + 2);
            mesh.indices.push_back(base_index + 3);
        }

        // Implementation for the new addQuad helper
        void MeshBuilder::addQuad(VoxelMesh& mesh,
                                  const ::VoxelEngine::World::VoxelPosition& p1, const ::VoxelEngine::World::VoxelPosition& p2,
                                  const ::VoxelEngine::World::VoxelPosition& p3, const ::VoxelEngine::World::VoxelPosition& p4,
                                  const ::VoxelEngine::World::Normal& normal,
                                  ::VoxelEngine::World::VoxelType voxelType) {
            // voxelType is currently unused but kept for future texture mapping etc.
            (void)voxelType; // Mark as unused to prevent compiler warnings

            uint32_t base_index = static_cast<uint32_t>(mesh.vertices.size());

            mesh.vertices.emplace_back(p1, normal, glm::vec2(0.0f, 0.0f)); // UVs placeholder
            mesh.vertices.emplace_back(p2, normal, glm::vec2(1.0f, 0.0f)); // UVs placeholder (width)
            mesh.vertices.emplace_back(p3, normal, glm::vec2(1.0f, 1.0f)); // UVs placeholder (width, height)
            mesh.vertices.emplace_back(p4, normal, glm::vec2(0.0f, 1.0f)); // UVs placeholder (height)

            // Standard quad triangulation (ensure CCW winding order as seen from the direction of the normal)
            mesh.indices.push_back(base_index);     // p1
            mesh.indices.push_back(base_index + 1); // p2
            mesh.indices.push_back(base_index + 2); // p3

            mesh.indices.push_back(base_index);     // p1
            mesh.indices.push_back(base_index + 2); // p3
            mesh.indices.push_back(base_index + 3); // p4
        }

        VoxelMesh MeshBuilder::buildNaiveMesh(const VoxelCastle::World::ChunkSegment& segment) {
            VoxelMesh mesh;
            mesh.clear(); // Ensure it's empty

            const auto segmentWidth = VoxelCastle::World::ChunkSegment::getWidth();
            const auto segmentHeight = VoxelCastle::World::ChunkSegment::getHeight();
            const auto segmentDepth = VoxelCastle::World::ChunkSegment::getDepth();

            using VoxelEngine::World::VoxelType;

            // Define face vertices relative to a voxel's local origin (0,0,0)
            // These are unit quads for each face.
            // Order: Bottom-Left, Bottom-Right, Top-Right, Top-Left (when looking at the face from outside)
            // This standard order helps with UV mapping later.

            // +X face (Right)
            const glm::vec3 right_face_verts[4] = {
                glm::vec3(1.0f, 0.0f, 0.0f), glm::vec3(1.0f, 0.0f, 1.0f),
                glm::vec3(1.0f, 1.0f, 1.0f), glm::vec3(1.0f, 1.0f, 0.0f)
            };
            const glm::vec3 right_normal(1.0f, 0.0f, 0.0f);

            // -X face (Left)
            const glm::vec3 left_face_verts[4] = {
                glm::vec3(0.0f, 0.0f, 1.0f), glm::vec3(0.0f, 0.0f, 0.0f),
                glm::vec3(0.0f, 1.0f, 0.0f), glm::vec3(0.0f, 1.0f, 1.0f)
            };
            const glm::vec3 left_normal(-1.0f, 0.0f, 0.0f);

            // +Y face (Top)
            const glm::vec3 top_face_verts[4] = {
                glm::vec3(0.0f, 1.0f, 0.0f), glm::vec3(1.0f, 1.0f, 0.0f),
                glm::vec3(1.0f, 1.0f, 1.0f), glm::vec3(0.0f, 1.0f, 1.0f)
            };
            const glm::vec3 top_normal(0.0f, 1.0f, 0.0f);

            // -Y face (Bottom)
            const glm::vec3 bottom_face_verts[4] = {
                glm::vec3(0.0f, 0.0f, 1.0f), glm::vec3(1.0f, 0.0f, 1.0f),
                glm::vec3(1.0f, 0.0f, 0.0f), glm::vec3(0.0f, 0.0f, 0.0f)
            };
            const glm::vec3 bottom_normal(0.0f, -1.0f, 0.0f);

            // +Z face (Front)
            const glm::vec3 front_face_verts[4] = {
                glm::vec3(1.0f, 0.0f, 1.0f), glm::vec3(0.0f, 0.0f, 1.0f),
                glm::vec3(0.0f, 1.0f, 1.0f), glm::vec3(1.0f, 1.0f, 1.0f)
            };
            const glm::vec3 front_normal(0.0f, 0.0f, 1.0f);

            // -Z face (Back)
            const glm::vec3 back_face_verts[4] = {
                glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(1.0f, 0.0f, 0.0f),
                glm::vec3(1.0f, 1.0f, 0.0f), glm::vec3(0.0f, 1.0f, 0.0f)
            };
            const glm::vec3 back_normal(0.0f, 0.0f, -1.0f);

            for (int_fast16_t x = 0; x < segmentWidth; ++x) {
                for (int_fast16_t y = 0; y < segmentHeight; ++y) {
                    for (int_fast16_t z = 0; z < segmentDepth; ++z) {
                        const auto current_voxel_type = segment.getVoxel(x, y, z).id;

                        if (current_voxel_type == static_cast<uint8_t>(VoxelType::AIR)) {
                            continue; // Skip air voxels
                        }

                        glm::vec3 voxel_world_pos(static_cast<float>(x), static_cast<float>(y), static_cast<float>(z));

                        // Check +X neighbor (Right)
                        if (segment.getVoxel(x + 1, y, z).id == static_cast<uint8_t>(VoxelType::AIR)) {
                            addFace(mesh, voxel_world_pos, right_face_verts, right_normal);
                        }
                        // Check -X neighbor (Left)
                        if (segment.getVoxel(x - 1, y, z).id == static_cast<uint8_t>(VoxelType::AIR)) {
                            addFace(mesh, voxel_world_pos, left_face_verts, left_normal);
                        }
                        // Check +Y neighbor (Top)
                        if (segment.getVoxel(x, y + 1, z).id == static_cast<uint8_t>(VoxelType::AIR)) {
                            addFace(mesh, voxel_world_pos, top_face_verts, top_normal);
                        }
                        // Check -Y neighbor (Bottom)
                        if (segment.getVoxel(x, y - 1, z).id == static_cast<uint8_t>(VoxelType::AIR)) {
                            addFace(mesh, voxel_world_pos, bottom_face_verts, bottom_normal);
                        }
                        // Check +Z neighbor (Front)
                        if (segment.getVoxel(x, y, z + 1).id == static_cast<uint8_t>(VoxelType::AIR)) {
                            addFace(mesh, voxel_world_pos, front_face_verts, front_normal);
                        }
                        // Check -Z neighbor (Back)
                        if (segment.getVoxel(x, y, z - 1).id == static_cast<uint8_t>(VoxelType::AIR)) {
                            addFace(mesh, voxel_world_pos, back_face_verts, back_normal);
                        }
                    }
                }
            }
            return mesh;
        }

        // Greedy Meshing Implementation
        VoxelMesh MeshBuilder::buildGreedyMesh(const VoxelCastle::World::ChunkSegment& segment) {
            VoxelMesh mesh;
            mesh.clear();

            const int chunk_dims[] = {
                VoxelCastle::World::ChunkSegment::getWidth(),
                VoxelCastle::World::ChunkSegment::getHeight(),
                VoxelCastle::World::ChunkSegment::getDepth()
            };

            using VoxelEngine::World::VoxelType;
            using ::VoxelEngine::World::Voxel;

            // Iterate over each of the 3 dimensions (X, Y, Z)
            for (int d = 0; d < 3; ++d) { // d is the dimension normal to the face (0=X, 1=Y, 2=Z)
                int u = (d + 1) % 3; // u-axis (secondary axis of the slice)
                int v = (d + 2) % 3; // v-axis (primary axis of the slice)

                int x[3] = {0, 0, 0}; // Current voxel coordinates
                int q[3] = {0, 0, 0}; // Offset in direction d
                q[d] = 1;

                // Mask to keep track of visited voxels on the current 2D slice
                // Dimensions of the mask depend on u and v axes for the current slice
                std::vector<bool> mask(chunk_dims[u] * chunk_dims[v], false);
                
                // Iterate over the dimension d (slices of the chunk)
                // x[d] goes from -1 (representing boundary before chunk) to chunk_dims[d]-1 (last layer in chunk)
                // This allows checking faces on both ends of the chunk.
                for (x[d] = -1; x[d] < chunk_dims[d]; ++x[d]) {
                    int n = 0; // Mask index

                    // Iterate over the v-axis of the slice
                    for (x[v] = 0; x[v] < chunk_dims[v]; ++x[v]) {
                        // Iterate over the u-axis of the slice
                        for (x[u] = 0; x[u] < chunk_dims[u]; ++x[u], ++n) {
                            if (mask[n]) {
                                continue; // Already meshed
                            }

                            // Get voxel types on both sides of the potential face
                            // Voxel 1: current voxel at x (potentially x[d]=-1, handled by getVoxel)
                            // Voxel 2: neighbor voxel at x + q (potentially x[d]+q[d]=chunk_dims[d], handled by getVoxel)
                            ::VoxelEngine::World::Voxel voxel1 = segment.getVoxel(x[0], x[1], x[2]);
                            ::VoxelEngine::World::Voxel voxel2 = segment.getVoxel(x[0] + q[0], x[1] + q[1], x[2] + q[2]);
                            
                            bool is_voxel1_solid = (voxel1.id != static_cast<uint8_t>(VoxelType::AIR));
                            bool is_voxel2_solid = (voxel2.id != static_cast<uint8_t>(VoxelType::AIR));

                            if (is_voxel1_solid == is_voxel2_solid) {
                                continue; // Both same (solid-solid or air-air), no face here
                            }

                            // Determine face properties
                            ::VoxelEngine::World::VoxelType current_face_voxel_type;
                            bool process_front_face; // Face of voxel1 in +d direction

                            if (is_voxel1_solid) { // voxel1 solid, voxel2 air -> front face of voxel1
                                current_face_voxel_type = static_cast<::VoxelEngine::World::VoxelType>(voxel1.id);
                                process_front_face = true;
                            } else { // voxel1 air, voxel2 solid -> "back" face of voxel2 (effectively front face of voxel2 in -d direction)
                                current_face_voxel_type = static_cast<::VoxelEngine::World::VoxelType>(voxel2.id);
                                process_front_face = false;
                            }

                            // Greedily expand in u-direction (height of quad on the slice)
                            int h_quad;
                            for (h_quad = 1; x[u] + h_quad < chunk_dims[u]; ++h_quad) {
                                int mask_check_idx_h = n + h_quad; // Mask index for (x[u]+h_quad, x[v])
                                if (mask[mask_check_idx_h]) {
                                    break; // Already part of another mesh
                                }

                                // Voxel on "this" side of the face, extended along u
                                int cur_coords_u_ext[3] = {x[0], x[1], x[2]};
                                cur_coords_u_ext[u] += h_quad;
                                ::VoxelEngine::World::Voxel v_strip_1 = segment.getVoxel(cur_coords_u_ext[0], cur_coords_u_ext[1], cur_coords_u_ext[2]);

                                // Voxel on "other" side of the face, extended along u
                                int other_coords_u_ext[3] = {x[0] + q[0], x[1] + q[1], x[2] + q[2]};
                                other_coords_u_ext[u] += h_quad;
                                ::VoxelEngine::World::Voxel v_strip_2 = segment.getVoxel(other_coords_u_ext[0], other_coords_u_ext[1], other_coords_u_ext[2]);
                                
                                bool v_strip_1_solid = (v_strip_1.id != static_cast<uint8_t>(VoxelType::AIR));
                                bool v_strip_2_solid = (v_strip_2.id != static_cast<uint8_t>(VoxelType::AIR));

                                if (process_front_face) {
                                    if (!v_strip_1_solid || v_strip_2_solid || static_cast<::VoxelEngine::World::VoxelType>(v_strip_1.id) != current_face_voxel_type) {
                                        break;
                                    }
                                } else { // back face
                                    if (v_strip_1_solid || !v_strip_2_solid || static_cast<::VoxelEngine::World::VoxelType>(v_strip_2.id) != current_face_voxel_type) {
                                        break;
                                    }
                                }
                            }

                            // Greedily expand in v-direction (width of quad on the slice)
                            int w_quad;
                            bool done_w = false;
                            for (w_quad = 1; x[v] + w_quad < chunk_dims[v]; ++w_quad) {
                                for (int k_u = 0; k_u < h_quad; ++k_u) { // Check all cells in the current quad's height
                                    // Mask index for (x[u]+k_u, x[v]+w_quad)
                                    int mask_check_idx_w = (x[v] + w_quad) * chunk_dims[u] + (x[u] + k_u);
                                    if (mask[mask_check_idx_w]) {
                                        done_w = true; break;
                                    }

                                    // Voxel on "this" side, extended along u and v
                                    int cur_coords_uv_ext[3] = {x[0], x[1], x[2]};
                                    cur_coords_uv_ext[u] += k_u;
                                    cur_coords_uv_ext[v] += w_quad;
                                    ::VoxelEngine::World::Voxel v_scan_1 = segment.getVoxel(cur_coords_uv_ext[0], cur_coords_uv_ext[1], cur_coords_uv_ext[2]);

                                    // Voxel on "other" side, extended along u and v
                                    int other_coords_uv_ext[3] = {x[0] + q[0], x[1] + q[1], x[2] + q[2]};
                                    other_coords_uv_ext[u] += k_u;
                                    other_coords_uv_ext[v] += w_quad;
                                    ::VoxelEngine::World::Voxel v_scan_2 = segment.getVoxel(other_coords_uv_ext[0], other_coords_uv_ext[1], other_coords_uv_ext[2]);

                                    bool v_scan_1_solid = (v_scan_1.id != static_cast<uint8_t>(VoxelType::AIR));
                                    bool v_scan_2_solid = (v_scan_2.id != static_cast<uint8_t>(VoxelType::AIR));

                                    if (process_front_face) {
                                        if (!v_scan_1_solid || v_scan_2_solid || static_cast<::VoxelEngine::World::VoxelType>(v_scan_1.id) != current_face_voxel_type) {
                                            done_w = true; break;
                                        }
                                    } else { // back face
                                        if (v_scan_1_solid || !v_scan_2_solid || static_cast<::VoxelEngine::World::VoxelType>(v_scan_2.id) != current_face_voxel_type) {
                                            done_w = true; break;
                                        }
                                    }
                                }
                                if (done_w) {
                                    break;
                                }
                            }

                            // Add the quad
                            // Quad base position is on the boundary plane: x[d] + q[d] (which is x[d]+1.0f)
                            // The coordinates x[u] and x[v] are the starting corner on that plane.
                            float quad_base_pos_on_plane[3];
                            quad_base_pos_on_plane[d] = static_cast<float>(x[d] + q[d]); // e.g. x_coord + 1.0f
                            quad_base_pos_on_plane[u] = static_cast<float>(x[u]);
                            quad_base_pos_on_plane[v] = static_cast<float>(x[v]);

                            float du_vec[3] = {0,0,0}; du_vec[u] = static_cast<float>(h_quad); // Vector along u-axis for height
                            float dv_vec[3] = {0,0,0}; dv_vec[v] = static_cast<float>(w_quad); // Vector along v-axis for width

                            // Vertices of the quad on the plane
                            ::VoxelEngine::World::VoxelPosition vp1 = {quad_base_pos_on_plane[0], quad_base_pos_on_plane[1], quad_base_pos_on_plane[2]};
                            ::VoxelEngine::World::VoxelPosition vp2 = {quad_base_pos_on_plane[0] + dv_vec[0], quad_base_pos_on_plane[1] + dv_vec[1], quad_base_pos_on_plane[2] + dv_vec[2]}; // vp1 + dv
                            ::VoxelEngine::World::VoxelPosition vp3 = {quad_base_pos_on_plane[0] + dv_vec[0] + du_vec[0], quad_base_pos_on_plane[1] + dv_vec[1] + du_vec[1], quad_base_pos_on_plane[2] + dv_vec[2] + du_vec[2]}; // vp1 + dv + du
                            ::VoxelEngine::World::VoxelPosition vp4 = {quad_base_pos_on_plane[0] + du_vec[0], quad_base_pos_on_plane[1] + du_vec[1], quad_base_pos_on_plane[2] + du_vec[2]}; // vp1 + du
                            
                            ::VoxelEngine::World::Normal normal_val;
                            if (process_front_face) { // Normal points in +d direction
                                normal_val.x = static_cast<float>(q[0]);
                                normal_val.y = static_cast<float>(q[1]);
                                normal_val.z = static_cast<float>(q[2]);
                                addQuad(mesh, vp1, vp2, vp3, vp4, normal_val, current_face_voxel_type);
                            } else { // Normal points in -d direction
                                normal_val.x = -static_cast<float>(q[0]);
                                normal_val.y = -static_cast<float>(q[1]);
                                normal_val.z = -static_cast<float>(q[2]);
                                // Reversed winding for back face (vp1, vp4, vp3, vp2) to keep CCW from normal's view
                                addQuad(mesh, vp1, vp4, vp3, vp2, normal_val, current_face_voxel_type);
                            }

                            // Mark voxels in mask
                            for (int l_v = 0; l_v < w_quad; ++l_v) {
                                for (int m_u = 0; m_u < h_quad; ++m_u) {
                                    mask[(x[v] + l_v) * chunk_dims[u] + (x[u] + m_u)] = true;
                                }
                            }
                        } // End u-axis iteration
                    } // End v-axis iteration
                } // End d-axis slice iteration
            } // End dimension iteration (d)
            return mesh;
        }

    } // namespace Rendering
} // namespace VoxelEngine
