#ifndef VOXEL_ENGINE_VOXEL_MESH_H
#define VOXEL_ENGINE_VOXEL_MESH_H

#include <vector>
#include <cstdint> // For uint32_t
#include <glad/glad.h> // For OpenGL types

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
         * Contains position, normal, texture coordinate data, and additional UV data for texture tiling.
         */
        struct Vertex {
            glm::vec3 position;    ///< 3D position of the vertex.
            glm::vec3 normal;      ///< Normal vector of the vertex.
            glm::vec2 quad_uv;     ///< UV coordinates for tiling across the quad (0 to W, 0 to H).
            glm::vec2 atlas_tile_origin_uv; ///< UV coordinates of the tile's origin in the atlas.
            float light = 1.0f;    ///< Per-vertex light (0.0 = dark, 1.0 = max brightness)
            int atlas_id = 0;      ///< Atlas ID (0=main, 1=side, 2=bottom) for multi-atlas support

            Vertex() = default;
            Vertex(const glm::vec3& pos,
                   const glm::vec3& norm,
                   const glm::vec2& q_uv = glm::vec2(0.0f),
                   const glm::vec2& atlas_origin = glm::vec2(0.0f),
                   float l = 1.0f,
                   int atlas = 0)
                : position(pos), normal(norm), quad_uv(q_uv), atlas_tile_origin_uv(atlas_origin), light(l), atlas_id(atlas) {}
        };

        /**
         * @struct VoxelMesh
         * @brief Holds the geometry data for a renderable voxel mesh.
         *
         * Consists of a list of vertices and a list of indices that define the triangles.
         * Each mesh now has its own OpenGL buffers to prevent buffer sharing issues.
         */
        struct VoxelMesh {
            std::vector<Vertex> vertices;       ///< List of vertices in the mesh.
            std::vector<uint32_t> indices;      ///< List of indices defining triangles (e.g., for GL_TRIANGLES).
            glm::vec3 worldPosition_ = glm::vec3(0.0f); ///< World position of the mesh's origin.
            bool initialized_ = false;          ///< True if the mesh has been initialized with data.
            
            // OpenGL buffer objects - each mesh has its own buffers
            mutable GLuint vao = 0;             ///< Vertex Array Object (unique per mesh)
            mutable GLuint vbo = 0;             ///< Vertex Buffer Object (unique per mesh)
            mutable GLuint ebo = 0;             ///< Element Buffer Object (unique per mesh)
            mutable bool buffersUploaded = false; ///< True if mesh data has been uploaded to GPU

            /**
             * @brief Clears all vertex and index data from the mesh.
             * Resets the mesh to an empty state and cleans up OpenGL buffers.
             */
            void clear() {
                vertices.clear();
                indices.clear();
                worldPosition_ = glm::vec3(0.0f); // Reset world position on clear
                initialized_ = false;
                cleanupBuffers(); // Clean up OpenGL buffers
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
            
            /**
             * @brief Gets whether mesh buffers have been uploaded to GPU.
             * @return True if buffers are uploaded, false otherwise.
             */
            bool areBuffersUploaded() const {
                return buffersUploaded;
            }
            
            /**
             * @brief Destructor - cleans up OpenGL buffers.
             */
            ~VoxelMesh() {
                cleanupBuffers();
            }
            
            /**
             * @brief Default constructor
             */
            VoxelMesh() = default;

            /**
             * @brief Copy constructor - creates new OpenGL buffers.
             */
            VoxelMesh(const VoxelMesh& other) 
                : vertices(other.vertices), indices(other.indices), 
                  worldPosition_(other.worldPosition_), initialized_(other.initialized_),
                  vao(0), vbo(0), ebo(0), buffersUploaded(false) {
                // Don't copy OpenGL buffers - they will be created when needed
            }
            
            /**
             * @brief Assignment operator - creates new OpenGL buffers.
             */
            VoxelMesh& operator=(const VoxelMesh& other) {
                if (this != &other) {
                    cleanupBuffers(); // Clean up existing buffers
                    vertices = other.vertices;
                    indices = other.indices;
                    worldPosition_ = other.worldPosition_;
                    initialized_ = other.initialized_;
                    vao = vbo = ebo = 0;
                    buffersUploaded = false;
                }
                return *this;
            }

        private:
            /**
             * @brief Cleans up OpenGL buffers.
             */
            void cleanupBuffers() const {
                if (vao != 0) {
                    glDeleteVertexArrays(1, &vao);
                    vao = 0;
                }
                if (vbo != 0) {
                    glDeleteBuffers(1, &vbo);
                    vbo = 0;
                }
                if (ebo != 0) {
                    glDeleteBuffers(1, &ebo);
                    ebo = 0;
                }
                buffersUploaded = false;
            }
        };

    } // namespace Rendering
} // namespace VoxelEngine

#endif //VOXEL_ENGINE_VOXEL_MESH_H
