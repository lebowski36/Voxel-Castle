#include "rendering/meshing/culled_face_meshing_algorithm.h"
#include "world/voxel_types.h"
#include "world/chunk_segment.h"
#include <glm/glm.hpp>

namespace VoxelEngine {
    namespace Rendering {
        namespace Meshing {

            VoxelMesh CulledFaceMeshingAlgorithm::generateMesh(
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

                // Define face vertices and normals
                struct FaceData {
                    glm::vec3 vertices[4];
                    glm::vec3 normal;
                    glm::ivec3 neighborOffset;
                };

                const FaceData faces[6] = {
                    // Front face (z+1)
                    {{{0.0f, 0.0f, 1.0f}, {1.0f, 0.0f, 1.0f}, {1.0f, 1.0f, 1.0f}, {0.0f, 1.0f, 1.0f}},
                     {0.0f, 0.0f, 1.0f}, {0, 0, 1}},
                    // Back face (z-1)
                    {{{1.0f, 0.0f, 0.0f}, {0.0f, 0.0f, 0.0f}, {0.0f, 1.0f, 0.0f}, {1.0f, 1.0f, 0.0f}},
                     {0.0f, 0.0f, -1.0f}, {0, 0, -1}},
                    // Left face (x-1)
                    {{{0.0f, 0.0f, 0.0f}, {0.0f, 0.0f, 1.0f}, {0.0f, 1.0f, 1.0f}, {0.0f, 1.0f, 0.0f}},
                     {-1.0f, 0.0f, 0.0f}, {-1, 0, 0}},
                    // Right face (x+1)
                    {{{1.0f, 0.0f, 1.0f}, {1.0f, 0.0f, 0.0f}, {1.0f, 1.0f, 0.0f}, {1.0f, 1.0f, 1.0f}},
                     {1.0f, 0.0f, 0.0f}, {1, 0, 0}},
                    // Top face (y+1)
                    {{{0.0f, 1.0f, 1.0f}, {1.0f, 1.0f, 1.0f}, {1.0f, 1.0f, 0.0f}, {0.0f, 1.0f, 0.0f}},
                     {0.0f, 1.0f, 0.0f}, {0, 1, 0}},
                    // Bottom face (y-1)
                    {{{0.0f, 0.0f, 0.0f}, {1.0f, 0.0f, 0.0f}, {1.0f, 0.0f, 1.0f}, {0.0f, 0.0f, 1.0f}},
                     {0.0f, -1.0f, 0.0f}, {0, -1, 0}}
                };

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
                            glm::ivec3 voxel_coords(x, y, z);

                            // Check each face and only render if it should be visible
                            for (int faceIndex = 0; faceIndex < 6; ++faceIndex) {
                                const FaceData& face = faces[faceIndex];
                                
                                if (shouldRenderFace(voxel_coords, face.neighborOffset, getVoxel)) {
                                    addFace(mesh, voxel_pos, face.vertices, face.normal, voxelType, atlas);
                                }
                            }
                        }
                    }
                }

                return mesh;
            }

            bool CulledFaceMeshingAlgorithm::shouldRenderFace(
                const glm::ivec3& voxelPos,
                const glm::ivec3& neighborOffset,
                const std::function<VoxelEngine::World::Voxel(int, int, int)>& getVoxel
            ) {
                // Get the neighbor voxel in the direction of this face
                glm::ivec3 neighborPos = voxelPos + neighborOffset;
                VoxelEngine::World::Voxel neighbor = getVoxel(neighborPos.x, neighborPos.y, neighborPos.z);
                
                // Render the face if the neighbor is air (or out of bounds, which getVoxel should return as air)
                return neighbor.id == static_cast<uint8_t>(VoxelEngine::World::VoxelType::AIR);
            }

            void CulledFaceMeshingAlgorithm::addFace(VoxelMesh& mesh,
                                                    const glm::vec3& voxel_pos,
                                                    const glm::vec3 face_vertices[4],
                                                    const glm::vec3& normal,
                                                    VoxelEngine::World::VoxelType voxelType,
                                                    const TextureAtlas& atlas,
                                                    float debugLight) {
                
                uint32_t base_index = static_cast<uint32_t>(mesh.vertices.size());

                TextureCoordinates texCoords = atlas.getTextureCoordinates(voxelType);
                glm::vec2 atlas_origin_uv = texCoords.getBottomLeft();

                // For face culling, each face is still 1x1 voxel
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
