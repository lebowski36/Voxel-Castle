#include "rendering/mesh_builder.h"
#include "rendering/meshing/two_phase_greedy_meshing_algorithm.h"
#include "rendering/debug_render_mode.h"
#include "world/voxel_types.h"
#include "world/chunk_segment.h"
#include "rendering/texture_atlas.h"
#include "../../game/include/utils/debug_logger.h"

#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <iostream>
#include <fstream>
#include <vector>
#include <chrono>
#include <sstream>
#include <iomanip>

namespace VoxelEngine {
    namespace Rendering {

        // Timestamp helper for mesh builder
        static std::string getTimestampMB() {
            auto now = std::chrono::system_clock::now();
            auto time_t = std::chrono::system_clock::to_time_t(now);
            auto ms = std::chrono::duration_cast<std::chrono::milliseconds>(
                now.time_since_epoch()) % 1000;
            
            std::stringstream ss;
            ss << std::put_time(std::localtime(&time_t), "%H:%M:%S");
            ss << '.' << std::setfill('0') << std::setw(3) << ms.count();
            return ss.str();
        }

        VoxelMesh MeshBuilder::buildMesh(
            const VoxelCastle::World::ChunkSegment& segment,
            const TextureAtlas& atlas,
            const std::function<VoxelEngine::World::Voxel(int, int, int)>& getVoxel,
            const glm::ivec3& chunkCoords
        ) {
            auto start = std::chrono::steady_clock::now();
            
            // Use two-phase greedy meshing directly
            Meshing::TwoPhaseGreedyMeshingAlgorithm algorithm;
            VoxelMesh result = algorithm.generateMesh(segment, atlas, getVoxel, chunkCoords);
            
            auto end = std::chrono::steady_clock::now();
            auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(end - start);
            
            // Redirected mesh building logs to a file-based logging system.
            if (duration.count() > 100) {
                std::cout << "[" << getTimestampMB() << "] Mesh built in " << duration.count() << " ms\n";
            }
            
            return result;
        }

        VoxelMesh MeshBuilder::buildTwoPhaseGreedyMesh(
            const VoxelCastle::World::ChunkSegment& segment, 
            const TextureAtlas& atlas, 
            const std::function<VoxelEngine::World::Voxel(int, int, int)>& getVoxel,
            const glm::ivec3& chunkCoords
        ) {
            auto start = std::chrono::steady_clock::now();
            
            // Use two-phase greedy meshing directly
            Meshing::TwoPhaseGreedyMeshingAlgorithm algorithm;
            VoxelMesh result = algorithm.generateMesh(segment, atlas, getVoxel, chunkCoords);
            
            auto end = std::chrono::steady_clock::now();
            auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(end - start);
            
            // Redirect log to file-based logging system
            DEBUG_LOG("MeshBuilder", "Two-phase greedy mesh built in " + std::to_string(duration.count()) + " ms");
            
            return result;
        }

        /**
         * @brief Adds a single face (composed of two triangles) to the given VoxelMesh.
         *
         * This is a helper function used by meshing algorithms (e.g., Naive Meshing).
         * It constructs four vertices for the face, calculates their world positions based on `voxel_pos`
         * and `face_vertices` (which define the face's shape relative to the voxel's origin).
         * It also applies texture coordinates from the `atlas` based on the `voxelType`.
         * The face is defined by `face_vertices` in a specific winding order (typically counter-clockwise
         * when viewed from outside) to ensure correct normal orientation.
         *
         * @param mesh The VoxelMesh to which the face's vertices and indices will be added.
         * @param voxel_pos The world position of the minimum corner of the voxel to which this face belongs.
         * @param face_vertices An array of 4 glm::vec3 defining the corners of the face, relative to (0,0,0) if voxel_pos is the origin.
         *                      The order should be consistent (e.g., bottom-left, bottom-right, top-right, top-left).
         * @param normal The normal vector for this face.
         * @param voxelType The type of the voxel, used to determine texture coordinates from the atlas.
         * @param atlas The TextureAtlas providing texture coordinates for different voxel types.
         */
        void MeshBuilder::addFace(VoxelMesh& mesh,
                     const glm::vec3& voxel_pos, // Base position of the voxel (e.g., its minimum corner)
                     const glm::vec3 face_vertices[4], // The 4 vertices of the face, relative to (0,0,0) if voxel_pos is origin
                     const glm::vec3& normal,
                     VoxelEngine::World::VoxelType voxelType,
                     const TextureAtlas& atlas,
                     float debugLight) {
            
            uint32_t base_index = static_cast<uint32_t>(mesh.vertices.size());

            TextureCoordinates texCoords = atlas.getTextureCoordinates(voxelType);
            glm::vec2 atlas_origin_uv = texCoords.getBottomLeft(); // This is the v_atlas_tile_origin_uv

            // quad_uv for a single face (non-greedy) will always be 0-1 range
            glm::vec2 quad_uvs[4];
            quad_uvs[0] = glm::vec2(0.0f, 0.0f); // Bottom-Left
            quad_uvs[1] = glm::vec2(1.0f, 0.0f); // Bottom-Right
            quad_uvs[2] = glm::vec2(1.0f, 1.0f); // Top-Right
            quad_uvs[3] = glm::vec2(0.0f, 1.0f); // Top-Left

            // The 'light' component is set to 1.0f for now.
            // Note: The order of face_vertices (BL, BR, TR, TL) must match the quad_uvs order.
            for (int i = 0; i < 4; ++i) {
                mesh.vertices.emplace_back(voxel_pos + face_vertices[i], normal, quad_uvs[i], atlas_origin_uv, debugLight);
            }

            mesh.indices.push_back(base_index);
            mesh.indices.push_back(base_index + 1);
            mesh.indices.push_back(base_index + 2);

            mesh.indices.push_back(base_index);
            mesh.indices.push_back(base_index + 2);
            mesh.indices.push_back(base_index + 3);
        }

        /**
         * @brief Adds a quadrilateral (quad) defined by four points to the VoxelMesh.
         *
         * This helper function is primarily used by the Greedy Meshing algorithm.
         * It takes four explicit corner positions (p1, p2, p3, p4) that form the quad,
         * a normal vector, the voxel type (for texturing), and the texture atlas.
         * It creates four vertices with appropriate texture coordinates and adds two triangles
         * to the mesh's index buffer to render the quad.
         * The order of points p1-p4 should define the quad with a counter-clockwise winding
         * when viewed from the direction of the normal.
         *
         * @param mesh The VoxelMesh to which the quad's vertices and indices will be added.
         * @param p1 The first vertex position of the quad.
         * @param p2 The second vertex position of the quad.
         * @param p3 The third vertex position of the quad.
         * @param p4 The fourth vertex position of the quad.
         * @param normal The normal vector for this quad.
         * @param voxelType The type of the voxel, used to determine texture coordinates from the atlas.
         * @param atlas The TextureAtlas providing texture coordinates for different voxel types.
         * @param quad_width_voxels The width of the quad in voxel units.
         * @param quad_height_voxels The height of the quad in voxel units.
         * @param chunkCoords The chunk coordinates of the quad.
         * @param debugLight The light intensity for debugging.
         */
        void MeshBuilder::addQuad(VoxelMesh& mesh,
                                  const ::VoxelEngine::World::VoxelPosition& p1, const ::VoxelEngine::World::VoxelPosition& p2,
                                  const ::VoxelEngine::World::VoxelPosition& p3, const ::VoxelEngine::World::VoxelPosition& p4,
                                  const ::VoxelEngine::World::Normal& normal,
                                  ::VoxelEngine::World::VoxelType voxelType,
                                  const TextureAtlas& atlas,
                                  int quad_width_voxels, 
                                  int quad_height_voxels,
                                  const glm::ivec3& chunkCoords,
                                  float debugLight) {

            uint32_t base_index = static_cast<uint32_t>(mesh.vertices.size());

            float light = debugLight; // Use debugLight for visualization
            TextureCoordinates texCoords = atlas.getTextureCoordinates(voxelType);
            glm::vec2 atlas_origin_uv = texCoords.getBottomLeft(); // This is v_atlas_tile_origin_uv

            // quad_uvs are now 0-W and 0-H
            glm::vec2 quad_uvs[4];
            // Assuming p1=BL, p2=BR, p3=TR, p4=TL for the quad definition
            // This needs to be consistent with how h_quad and w_quad are passed from buildGreedyMesh
            // and how they relate to the U and V directions of the texture on the quad.
            // If p1,p2,p3,p4 correspond to visual BL, BR, TR, TL of the quad on screen:
            quad_uvs[0] = glm::vec2(0.0f, 0.0f);
            quad_uvs[1] = glm::vec2(static_cast<float>(quad_width_voxels), 0.0f);
            quad_uvs[2] = glm::vec2(static_cast<float>(quad_width_voxels), static_cast<float>(quad_height_voxels));
            quad_uvs[3] = glm::vec2(0.0f, static_cast<float>(quad_height_voxels));

            mesh.vertices.emplace_back(p1, normal, quad_uvs[0], atlas_origin_uv, light);
            mesh.vertices.emplace_back(p2, normal, quad_uvs[1], atlas_origin_uv, light);
            mesh.vertices.emplace_back(p3, normal, quad_uvs[2], atlas_origin_uv, light);
            mesh.vertices.emplace_back(p4, normal, quad_uvs[3], atlas_origin_uv, light);

            // Standard quad triangulation (ensure CCW winding order as seen from the direction of the normal for front faces)
            mesh.indices.push_back(base_index);     // p1
            mesh.indices.push_back(base_index + 1); // p2
            mesh.indices.push_back(base_index + 2); // p3

            mesh.indices.push_back(base_index);     // p1
            mesh.indices.push_back(base_index + 2); // p3
            mesh.indices.push_back(base_index + 3); // p4
        }

    } // namespace Rendering
} // namespace VoxelEngine
