#ifndef VOXEL_ENGINE_VOXEL_MESH_H
#define VOXEL_ENGINE_VOXEL_MESH_H

#include <vector>
#include <cstdint> // For uint32_t

// GLM headers
#include <glm/glm.hpp>
#include <glm/vec2.hpp> // For glm::vec2
#include <glm/vec3.hpp> // For glm::vec3

namespace VoxelEngine {
    namespace Rendering {

        /**
         * @struct Vertex
         * @brief Represents a single vertex in a mesh.
         *
         * Contains position, normal, and texture coordinate data.
         */
        struct Vertex {
            glm::vec3 position;    ///< 3D position of the vertex.
            glm::vec3 normal;      ///< Normal vector of the vertex.
            glm::vec2 texCoords;   ///< UV texture coordinates.

            /**
             * @brief Default constructor.
             */
            Vertex() = default;

            /**
             * @brief Constructs a Vertex with specified attributes.
             * @param pos The 3D position of the vertex.
             * @param norm The normal vector of the vertex (defaults to Z-up if unspecified and pos is zero, otherwise needs calculation).
             * @param uv The UV texture coordinates (defaults to (0,0)).
             */
            Vertex(const glm::vec3& pos,
                   const glm::vec3& norm,
                   const glm::vec2& uv = glm::vec2(0.0f))
                : position(pos), normal(norm), texCoords(uv) {}
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

            /**
             * @brief Clears all vertex and index data from the mesh.
             * Resets the mesh to an empty state.
             */
            void clear() {
                vertices.clear();
                indices.clear();
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
        };

    } // namespace Rendering
} // namespace VoxelEngine

#endif //VOXEL_ENGINE_VOXEL_MESH_H
