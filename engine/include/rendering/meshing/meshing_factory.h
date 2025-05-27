#ifndef VOXEL_ENGINE_MESHING_FACTORY_H
#define VOXEL_ENGINE_MESHING_FACTORY_H

#include "rendering/meshing/meshing_algorithm.h"
#include <memory>
#include <unordered_map>
#include <vector>

namespace VoxelEngine {
    namespace Rendering {
        namespace Meshing {

            /**
             * @class MeshingFactory
             * @brief Factory class for creating and managing meshing algorithms
             * 
             * This factory provides a centralized way to create meshing algorithm instances
             * and supports runtime switching between different algorithms.
             */
            class MeshingFactory {
            public:
                /**
                 * @brief Get the singleton instance of the factory
                 * @return Reference to the factory instance
                 */
                static MeshingFactory& getInstance();

                /**
                 * @brief Create a meshing algorithm instance
                 * @param type The type of algorithm to create
                 * @return Unique pointer to the algorithm instance, or nullptr if type is invalid
                 */
                std::unique_ptr<MeshingAlgorithm> createAlgorithm(MeshingAlgorithmType type);

                /**
                 * @brief Get a list of all available algorithm types
                 * @return Vector of algorithm types
                 */
                std::vector<MeshingAlgorithmType> getAvailableAlgorithms() const;

                /**
                 * @brief Get algorithm info without creating an instance
                 * @param type The algorithm type
                 * @return Algorithm name and description, or empty strings if type is invalid
                 */
                struct AlgorithmInfo {
                    const char* name;
                    const char* description;
                };
                AlgorithmInfo getAlgorithmInfo(MeshingAlgorithmType type) const;

                /**
                 * @brief Convert algorithm type to string
                 * @param type The algorithm type
                 * @return String representation
                 */
                static const char* algorithmTypeToString(MeshingAlgorithmType type);

                /**
                 * @brief Convert string to algorithm type
                 * @param str The string representation
                 * @return Algorithm type, or NAIVE if string is invalid
                 */
                static MeshingAlgorithmType stringToAlgorithmType(const char* str);

            private:
                MeshingFactory() = default;
                ~MeshingFactory() = default;
                MeshingFactory(const MeshingFactory&) = delete;
                MeshingFactory& operator=(const MeshingFactory&) = delete;

                // Registry of algorithm creation functions
                std::unordered_map<MeshingAlgorithmType, std::function<std::unique_ptr<MeshingAlgorithm>()>> m_creators;
                
                void initializeCreators();
                bool m_initialized = false;
            };

        } // namespace Meshing
    } // namespace Rendering
} // namespace VoxelEngine

#endif // VOXEL_ENGINE_MESHING_FACTORY_H
