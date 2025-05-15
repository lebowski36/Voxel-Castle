#ifndef VOXEL_ENGINE_VOXEL_MESH_H
#define VOXEL_ENGINE_VOXEL_MESH_H

#include <vector>
#include <cstdint> // For uint32_t

// GLM headers
#include <glm/glm.hpp>
#include <glm/vec2.hpp> // For glm::vec2
#include <glm/vec3.hpp> // For glm::vec3

#include "rendering/DebugText.h" // Added for DebugTextInfo

namespace VoxelEngine {
    namespace Rendering {

        /**
         * @struct Vertex
         * @brief Represents a single vertex in a mesh.
         *
         * Contains position, normal, texture coordinate data, and additional UV data for texture tiling.
         */
        struct Vertex {
            glm::vec3 position;    ///< 3D position of the vertex.
            glm::vec3 normal;      ///< Normal vector of the vertex.
            glm::vec2 quad_uv;     ///< UV coordinates for tiling across the quad (0 to W, 0 to H).
            glm::vec2 atlas_tile_origin_uv; ///< UV coordinates of the tile's origin in the atlas.
            float light = 1.0f;    ///< Per-vertex light (0.0 = dark, 1.0 = max brightness)
            glm::vec4 debugColor = glm::vec4(0.0f); ///< Debug color for face ID visualization (used only in FACE_DEBUG mode)

            Vertex() = default;
            Vertex(const glm::vec3& pos,
                   const glm::vec3& norm,
                   const glm::vec2& q_uv = glm::vec2(0.0f),
                   const glm::vec2& atlas_origin = glm::vec2(0.0f),
                   float l = 1.0f,
                   const glm::vec4& dbg = glm::vec4(0.0f))
                : position(pos), normal(norm), quad_uv(q_uv), atlas_tile_origin_uv(atlas_origin), light(l), debugColor(dbg) {}
        };

        /**
         * @struct VoxelMesh
         * @brief Holds the geometry data for a renderable voxel mesh.
         *
         * Consists of a list of vertices and a list of indices that define the triangles.
         * This mesh data is typically generated from a ChunkSegment.
         */
        struct VoxelMesh {
            std::vector<Vertex> vertices;       ///< List of vertices in the mesh.
            std::vector<uint32_t> indices;      ///< List of indices defining triangles (e.g., for GL_TRIANGLES).
            glm::vec3 worldPosition_ = glm::vec3(0.0f); ///< World position of the mesh's origin.
            bool initialized_ = false;          ///< True if the mesh has been initialized with data.
            std::vector<DebugTextInfo> debugFaceTexts; ///< Information for rendering text on faces in debug mode.

            const std::vector<DebugTextInfo>& getDebugFaceTexts() const { return debugFaceTexts; } // Added getter

            /**
             * @brief Clears all vertex and index data from the mesh.
             * Resets the mesh to an empty state.
             */
            void clear() {
                vertices.clear();
                indices.clear();
                debugFaceTexts.clear(); // Clear debug texts as well
                worldPosition_ = glm::vec3(0.0f); // Reset world position on clear
                initialized_ = false;
            }

            /**
             * @brief Checks if the mesh has any data.
             * @return True if the mesh contains no vertices, false otherwise.
             *         Note: An empty mesh might still have indices if cleared improperly,
             *         but typically vertices are the primary indicator.
             */
            bool isEmpty() const {
                return vertices.empty();
            }

            /**
             * @brief Checks if the mesh has been initialized.
             * @return True if the mesh has been initialized, false otherwise.
             */
            bool isInitialized() const {
                return initialized_;
            }

            /**
             * @brief Marks the mesh as initialized.
             * Typically called after vertices and indices are populated.
             */
            void setInitialized(bool initialized) {
                initialized_ = initialized;
            }

            /**
             * @brief Sets the world position of the mesh's origin.
             * @param position The world coordinates of the origin.
             */
            void setWorldPosition(const glm::vec3& position) {
                worldPosition_ = position;
            }

            /**
             * @brief Gets the world position of the mesh's origin.
             * @return The world coordinates of the origin.
             */
            glm::vec3 getWorldPosition() const {
                return worldPosition_;
            }

            /**
             * @brief Gets the number of vertices in the mesh.
             * @return The count of vertices.
             */
            size_t getVertexCount() const {
                return vertices.size();
            }

            /**
             * @brief Gets the number of indices in the mesh.
             * @return The count of indices.
             */
            size_t getIndexCount() const {
                return indices.size();
            }
        };

    } // namespace Rendering
} // namespace VoxelEngine

#endif //VOXEL_ENGINE_VOXEL_MESH_H
