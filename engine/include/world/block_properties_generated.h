#ifndef VOXEL_ENGINE_BLOCK_PROPERTIES_GENERATED_H
#define VOXEL_ENGINE_BLOCK_PROPERTIES_GENERATED_H

#include "world/block_properties.h"

namespace VoxelEngine {
namespace World {

class BlockPropertiesGenerator {
public:
    /**
     * @brief Initialize all block properties from unified JSON data.
     * @param manager The BlockPropertiesManager to populate.
     */
    static void initializeFromUnifiedData(BlockPropertiesManager& manager);
    
private:
    static void registerAllBlocks(BlockPropertiesManager& manager);
};

} // namespace World
} // namespace VoxelEngine

#endif // VOXEL_ENGINE_BLOCK_PROPERTIES_GENERATED_H
