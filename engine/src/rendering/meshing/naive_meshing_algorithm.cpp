#include "rendering/meshing/naive_meshing_algorithm.h"
#include "world/voxel_types.h"
#include "world/chunk_segment.h"
#include <glm/glm.hpp>

namespace VoxelEngine {
    namespace Rendering {
        namespace Meshing {

            VoxelMesh NaiveMeshingAlgorithm::generateMesh(
                const VoxelCastle::World::ChunkSegment& segment,
                const TextureAtlas& atlas,
                const std::function<VoxelEngine::World::Voxel(int, int, int)>& getVoxel,
                const glm::ivec3& chunkCoords
            ) {
                VoxelMesh mesh;
                mesh.clear();

                const int width = VoxelCastle::World::ChunkSegment::getWidth();
                const int height = VoxelCastle::World::ChunkSegment::getHeight();
                const int depth = VoxelCastle::World::ChunkSegment::getDepth();

                // Define face vertices (relative to voxel position)
                const glm::vec3 front_face_verts[4] = {
                    {0.0f, 0.0f, 1.0f}, {1.0f, 0.0f, 1.0f}, {1.0f, 1.0f, 1.0f}, {0.0f, 1.0f, 1.0f}
                };
                const glm::vec3 back_face_verts[4] = {
                    {1.0f, 0.0f, 0.0f}, {0.0f, 0.0f, 0.0f}, {0.0f, 1.0f, 0.0f}, {1.0f, 1.0f, 0.0f}
                };
                const glm::vec3 left_face_verts[4] = {
                    {0.0f, 0.0f, 0.0f}, {0.0f, 0.0f, 1.0f}, {0.0f, 1.0f, 1.0f}, {0.0f, 1.0f, 0.0f}
                };
                const glm::vec3 right_face_verts[4] = {
                    {1.0f, 0.0f, 1.0f}, {1.0f, 0.0f, 0.0f}, {1.0f, 1.0f, 0.0f}, {1.0f, 1.0f, 1.0f}
                };
                const glm::vec3 top_face_verts[4] = {
                    {0.0f, 1.0f, 1.0f}, {1.0f, 1.0f, 1.0f}, {1.0f, 1.0f, 0.0f}, {0.0f, 1.0f, 0.0f}
                };
                const glm::vec3 bottom_face_verts[4] = {
                    {0.0f, 0.0f, 0.0f}, {1.0f, 0.0f, 0.0f}, {1.0f, 0.0f, 1.0f}, {0.0f, 0.0f, 1.0f}
                };

                // Face normals
                const glm::vec3 front_normal(0.0f, 0.0f, 1.0f);
                const glm::vec3 back_normal(0.0f, 0.0f, -1.0f);
                const glm::vec3 left_normal(-1.0f, 0.0f, 0.0f);
                const glm::vec3 right_normal(1.0f, 0.0f, 0.0f);
                const glm::vec3 top_normal(0.0f, 1.0f, 0.0f);
                const glm::vec3 bottom_normal(0.0f, -1.0f, 0.0f);

                // Iterate through all voxels in the segment
                for (int y = 0; y < height; ++y) {
                    for (int z = 0; z < depth; ++z) {
                        for (int x = 0; x < width; ++x) {
                            VoxelEngine::World::Voxel voxel = segment.getVoxel(x, y, z);
                            
                            if (voxel.id == static_cast<uint8_t>(VoxelEngine::World::VoxelType::AIR)) {
                                continue; // Skip air voxels
                            }

                            VoxelEngine::World::VoxelType voxelType = static_cast<VoxelEngine::World::VoxelType>(voxel.id);
                            glm::vec3 voxel_pos(static_cast<float>(x), static_cast<float>(y), static_cast<float>(z));

                            // Check each face direction and only add faces that are exposed
                            
                            // Front face (check z+1)
                            VoxelEngine::World::Voxel front_neighbor = getVoxel(x, y, z + 1);
                            if (front_neighbor.id == static_cast<uint8_t>(VoxelEngine::World::VoxelType::AIR)) {
                                addFace(mesh, voxel_pos, front_face_verts, front_normal, voxelType, atlas);
                            }

                            // Back face (check z-1)
                            VoxelEngine::World::Voxel back_neighbor = getVoxel(x, y, z - 1);
                            if (back_neighbor.id == static_cast<uint8_t>(VoxelEngine::World::VoxelType::AIR)) {
                                addFace(mesh, voxel_pos, back_face_verts, back_normal, voxelType, atlas);
                            }

                            // Left face (check x-1)
                            VoxelEngine::World::Voxel left_neighbor = getVoxel(x - 1, y, z);
                            if (left_neighbor.id == static_cast<uint8_t>(VoxelEngine::World::VoxelType::AIR)) {
                                addFace(mesh, voxel_pos, left_face_verts, left_normal, voxelType, atlas);
                            }

                            // Right face (check x+1)
                            VoxelEngine::World::Voxel right_neighbor = getVoxel(x + 1, y, z);
                            if (right_neighbor.id == static_cast<uint8_t>(VoxelEngine::World::VoxelType::AIR)) {
                                addFace(mesh, voxel_pos, right_face_verts, right_normal, voxelType, atlas);
                            }

                            // Top face (check y+1)
                            VoxelEngine::World::Voxel top_neighbor = getVoxel(x, y + 1, z);
                            if (top_neighbor.id == static_cast<uint8_t>(VoxelEngine::World::VoxelType::AIR)) {
                                addFace(mesh, voxel_pos, top_face_verts, top_normal, voxelType, atlas);
                            }

                            // Bottom face (check y-1)
                            VoxelEngine::World::Voxel bottom_neighbor = getVoxel(x, y - 1, z);
                            if (bottom_neighbor.id == static_cast<uint8_t>(VoxelEngine::World::VoxelType::AIR)) {
                                addFace(mesh, voxel_pos, bottom_face_verts, bottom_normal, voxelType, atlas);
                            }
                        }
                    }
                }

                return mesh;
            }

            void NaiveMeshingAlgorithm::addFace(VoxelMesh& mesh,
                                               const glm::vec3& voxel_pos,
                                               const glm::vec3 face_vertices[4],
                                               const glm::vec3& normal,
                                               VoxelEngine::World::VoxelType voxelType,
                                               const TextureAtlas& atlas,
                                               float debugLight) {
                
                uint32_t base_index = static_cast<uint32_t>(mesh.vertices.size());

                TextureCoordinates texCoords = atlas.getTextureCoordinates(voxelType);
                glm::vec2 atlas_origin_uv = texCoords.getBottomLeft();

                // For naive meshing, each face is 1x1 voxel
                glm::vec2 quad_uvs[4] = {
                    {0.0f, 0.0f}, // Bottom-left
                    {1.0f, 0.0f}, // Bottom-right
                    {1.0f, 1.0f}, // Top-right
                    {0.0f, 1.0f}  // Top-left
                };

                // Debug color for face identification
                glm::vec4 debugColor(1.0f, 1.0f, 1.0f, 1.0f);

                // Add vertices
                for (int i = 0; i < 4; ++i) {
                    glm::vec3 world_pos = voxel_pos + face_vertices[i];
                    mesh.vertices.emplace_back(
                        VoxelEngine::World::VoxelPosition{world_pos.x, world_pos.y, world_pos.z},
                        VoxelEngine::World::Normal{normal.x, normal.y, normal.z},
                        quad_uvs[i],
                        atlas_origin_uv,
                        debugLight,
                        debugColor
                    );
                }

                // Add indices (two triangles per face)
                mesh.indices.push_back(base_index);
                mesh.indices.push_back(base_index + 1);
                mesh.indices.push_back(base_index + 2);

                mesh.indices.push_back(base_index);
                mesh.indices.push_back(base_index + 2);
                mesh.indices.push_back(base_index + 3);
            }

        } // namespace Meshing
    } // namespace Rendering
} // namespace VoxelEngine
