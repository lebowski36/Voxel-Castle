#include "rendering/mesh_builder.h"
#include "rendering/meshing/meshing_factory.h"
#include "rendering/face_debug_utils.h"
#include "rendering/debug_render_mode.h"
#include "world/voxel_types.h"
#include "world/chunk_segment.h"
#include "rendering/texture_atlas.h"
#include "rendering/DebugText.h"

#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <iostream>
#include <fstream>
#include <vector>

namespace VoxelEngine {
    namespace Rendering {

        // Import the meshing types for convenience
        using Meshing::MeshingAlgorithmType;
        using Meshing::MeshingAlgorithm;
        using Meshing::MeshingFactory;

        // Static member initialization
        MeshingAlgorithmType MeshBuilder::defaultAlgorithmType = MeshingAlgorithmType::TWO_PHASE_GREEDY;

        VoxelMesh MeshBuilder::buildMesh(
            const VoxelCastle::World::ChunkSegment& segment,
            const TextureAtlas& atlas,
            const std::function<VoxelEngine::World::Voxel(int, int, int)>& getVoxel,
            const glm::ivec3& chunkCoords,
            MeshingAlgorithmType algorithmType
        ) {
            // Get the appropriate meshing algorithm
            auto algorithm = MeshingFactory::getInstance().createAlgorithm(algorithmType);
            if (!algorithm) {
                std::cerr << "Failed to create meshing algorithm of type: " 
                         << MeshingFactory::algorithmTypeToString(algorithmType) << std::endl;
                // Fallback to TwoPhaseGreedy
                algorithm = MeshingFactory::getInstance().createAlgorithm(MeshingAlgorithmType::TWO_PHASE_GREEDY);
                if (!algorithm) {
                    std::cerr << "Fatal error: Failed to create fallback TWO_PHASE_GREEDY algorithm" << std::endl;
                    return VoxelMesh(); // Return empty mesh
                }
            }

            // Generate the mesh using the selected algorithm
            return algorithm->generateMesh(segment, atlas, getVoxel, chunkCoords);
        }

        void MeshBuilder::setDefaultAlgorithm(MeshingAlgorithmType algorithmType) {
            defaultAlgorithmType = algorithmType;
        }

        MeshingAlgorithmType MeshBuilder::getDefaultAlgorithm() {
            return defaultAlgorithmType;
        }

        // Legacy methods for backward compatibility
        VoxelMesh MeshBuilder::buildNaiveMesh(
            const VoxelCastle::World::ChunkSegment& segment,
            const TextureAtlas& atlas,
            const std::function<VoxelEngine::World::Voxel(int, int, int)>& getVoxel,
            const glm::ivec3& chunkCoords
        ) {
            return buildMesh(segment, atlas, getVoxel, chunkCoords, MeshingAlgorithmType::NAIVE);
        }

        VoxelMesh MeshBuilder::buildGreedyMesh(
            const VoxelCastle::World::ChunkSegment& segment, 
            const TextureAtlas& atlas, 
            const std::function<VoxelEngine::World::Voxel(int, int, int)>& getVoxel,
            const glm::ivec3& chunkCoords
        ) {
            return buildMesh(segment, atlas, getVoxel, chunkCoords, MeshingAlgorithmType::GREEDY);
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
                    // DEBUG LOGGING
                    std::cout << "[MeshBuilder DEBUG addFace] VoxelPos: (" << voxel_pos.x << "," << voxel_pos.y << "," << voxel_pos.z 
                              << ") Normal: (" << normal.x << "," << normal.y << "," << normal.z
                              << ") DirID: " << directionId 
                              << " Color: (" << debugColor.r << "," << debugColor.g << "," << debugColor.b << "," << debugColor.a << ")" << std::endl;
                }
                mesh.vertices.emplace_back(voxel_pos + face_vertices[i], normal, quad_uvs[i], atlas_origin_uv, debugLight, debugColor);
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
                // DEBUG LOGGING
                std::cout << "[MeshBuilder DEBUG addQuad] P1: (" << p1.x << "," << p1.y << "," << p1.z
                          << ") Normal: (" << normal.x << "," << normal.y << "," << normal.z
                          << ") DirID: " << directionId
                          << " Color: (" << debugColor.r << "," << debugColor.g << "," << debugColor.b << "," << debugColor.a << ")" << std::endl;
            }
            mesh.vertices.emplace_back(p1, normal, quad_uvs[0], atlas_origin_uv, light, debugColor);
            mesh.vertices.emplace_back(p2, normal, quad_uvs[1], atlas_origin_uv, light, debugColor);
            mesh.vertices.emplace_back(p3, normal, quad_uvs[2], atlas_origin_uv, light, debugColor);
            mesh.vertices.emplace_back(p4, normal, quad_uvs[3], atlas_origin_uv, light, debugColor);

            // Standard quad triangulation (ensure CCW winding order as seen from the direction of the normal for front faces)
            mesh.indices.push_back(base_index);     // p1
            mesh.indices.push_back(base_index + 1); // p2
            mesh.indices.push_back(base_index + 2); // p3

            mesh.indices.push_back(base_index);     // p1
            mesh.indices.push_back(base_index + 2); // p3
            mesh.indices.push_back(base_index + 3); // p4

            if (::g_debugRenderMode == DebugRenderMode::FACE_DEBUG) {
                // Calculate the center of the quad for text placement (this is fine for where the text is drawn)
                glm::vec3 faceCenterLocal = (static_cast<glm::vec3>(p1) + static_cast<glm::vec3>(p2) + static_cast<glm::vec3>(p3) + static_cast<glm::vec3>(p4)) / 4.0f;
                
                char coordText[256];
                // Display Chunk Coords (C: X,Y,Z) and Local Voxel Coords of p1 (V: X,Y,Z)
                snprintf(coordText, sizeof(coordText), "C(%.0f,%.0f,%.0f)V(%.0f,%.0f,%.0f)", 
                         static_cast<float>(chunkCoords.x), static_cast<float>(chunkCoords.y), static_cast<float>(chunkCoords.z),
                         p1.x, p1.y, p1.z);
                
                mesh.debugFaceTexts.push_back({std::string(coordText), faceCenterLocal, normal});
            }
        }

    } // namespace Rendering
} // namespace VoxelEngine
