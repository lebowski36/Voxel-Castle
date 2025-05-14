#pragma once

// Forward declarations to avoid full includes in header if possible
namespace VoxelCastle {
    namespace World {
        class WorldManager;
        class WorldGenerator;
    }
}

namespace GameWorldSetup {

/**
 * @brief Initializes a static portion of the game world.
 * 
 * This function will typically be called once during game initialization
 * to create the initial environment for the player.
 * 
 * @param worldManager A reference to the main WorldManager instance.
 * @param worldGenerator A reference to the WorldGenerator instance to be used.
 */
void initializeStaticWorld(VoxelCastle::World::WorldManager& worldManager, 
                           VoxelCastle::World::WorldGenerator& worldGenerator);

} // namespace GameWorldSetup
