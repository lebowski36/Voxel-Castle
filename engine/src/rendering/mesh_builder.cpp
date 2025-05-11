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
        void addFace(VoxelMesh& mesh,
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

    } // namespace Rendering
} // namespace VoxelEngine
