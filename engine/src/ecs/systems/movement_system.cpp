#include "ecs/systems/movement_system.h"
#include "ecs/components/position_component.h"
#include "ecs/components/velocity_component.h"
#include <iostream> // For temporary logging

namespace VoxelEngine {
    namespace ecs {

        MovementSystem::MovementSystem(flecs::world& ecsWorld) {
            ecsWorld.system<PositionComponent, VelocityComponent>()
                .each([](flecs::entity e, PositionComponent& p, VelocityComponent& v) {
                    p.position += v.velocity * e.world().delta_time();
                    // For debugging purposes:
                    std::cout << "Entity " << e.name() << " moved to ("
                              << p.position.x << ", " << p.position.y << ", " << p.position.z << ")"
                              << " by (" << v.velocity.x << ", " << v.velocity.y << ", " << v.velocity.z << ") delta_time: " << e.world().delta_time() << "\n";
                });
            
            std::cout << "MovementSystem registered." << std::endl;
        }

    }
}
