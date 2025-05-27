#include "rendering/meshing/meshing_factory.h"
#include "rendering/meshing/naive_meshing_algorithm.h"
#include "rendering/meshing/greedy_meshing_algorithm.h"
#include "rendering/meshing/two_phase_greedy_meshing_algorithm.h"
#include "rendering/meshing/culled_face_meshing_algorithm.h"
#include <iostream>

namespace VoxelEngine {
    namespace Rendering {
        namespace Meshing {

            MeshingFactory& MeshingFactory::getInstance() {
                static MeshingFactory instance;
                if (!instance.m_initialized) {
                    instance.initializeCreators();
                    instance.m_initialized = true;
                }
                return instance;
            }

            void MeshingFactory::initializeCreators() {
                // Register algorithm creators
                m_creators[MeshingAlgorithmType::NAIVE] = []() {
                    return std::make_unique<NaiveMeshingAlgorithm>();
                };

                m_creators[MeshingAlgorithmType::GREEDY] = []() {
                    return std::make_unique<GreedyMeshingAlgorithm>();
                };

                m_creators[MeshingAlgorithmType::CULLED_FACE] = []() {
                    return std::make_unique<CulledFaceMeshingAlgorithm>();
                };

                m_creators[MeshingAlgorithmType::TWO_PHASE_GREEDY] = []() {
                    return std::make_unique<TwoPhaseGreedyMeshingAlgorithm>();
                };

                // Future algorithms can be registered here
            }

            std::unique_ptr<MeshingAlgorithm> MeshingFactory::createAlgorithm(MeshingAlgorithmType type) {
                auto it = m_creators.find(type);
                if (it != m_creators.end()) {
                    return it->second();
                }
                
                std::cerr << "[MeshingFactory] Warning: Unknown algorithm type " 
                          << static_cast<int>(type) << ", falling back to NAIVE" << std::endl;
                return std::make_unique<NaiveMeshingAlgorithm>();
            }

            std::vector<MeshingAlgorithmType> MeshingFactory::getAvailableAlgorithms() const {
                std::vector<MeshingAlgorithmType> algorithms;
                for (const auto& pair : m_creators) {
                    algorithms.push_back(pair.first);
                }
                return algorithms;
            }

            MeshingFactory::AlgorithmInfo MeshingFactory::getAlgorithmInfo(MeshingAlgorithmType type) const {
                // Create a temporary instance to get info
                auto algorithm = const_cast<MeshingFactory*>(this)->createAlgorithm(type);
                if (algorithm) {
                    return {algorithm->getName(), algorithm->getDescription()};
                }
                return {"Unknown", "Unknown algorithm type"};
            }

            const char* MeshingFactory::algorithmTypeToString(MeshingAlgorithmType type) {
                switch (type) {
                    case MeshingAlgorithmType::NAIVE:
                        return "NAIVE";
                    case MeshingAlgorithmType::GREEDY:
                        return "GREEDY";
                    case MeshingAlgorithmType::CULLED_FACE:
                        return "CULLED_FACE";
                    case MeshingAlgorithmType::TWO_PHASE_GREEDY:
                        return "TWO_PHASE_GREEDY";
                    case MeshingAlgorithmType::LOD_AWARE:
                        return "LOD_AWARE";
                    default:
                        return "UNKNOWN";
                }
            }

            MeshingAlgorithmType MeshingFactory::stringToAlgorithmType(const char* str) {
                if (std::string(str) == "NAIVE") return MeshingAlgorithmType::NAIVE;
                if (std::string(str) == "GREEDY") return MeshingAlgorithmType::GREEDY;
                if (std::string(str) == "CULLED_FACE") return MeshingAlgorithmType::CULLED_FACE;
                if (std::string(str) == "TWO_PHASE_GREEDY") return MeshingAlgorithmType::TWO_PHASE_GREEDY;
                if (std::string(str) == "LOD_AWARE") return MeshingAlgorithmType::LOD_AWARE;
                
                std::cerr << "[MeshingFactory] Warning: Unknown algorithm string '" 
                          << str << "', falling back to NAIVE" << std::endl;
                return MeshingAlgorithmType::NAIVE;
            }

        } // namespace Meshing
    } // namespace Rendering
} // namespace VoxelEngine
