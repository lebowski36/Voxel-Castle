#ifndef MOVEMENT_SYSTEM_H
#define MOVEMENT_SYSTEM_H

#include <flecs.h>

namespace VoxelEngine {
    namespace ecs {
        // Forward declare components if not already included
        // struct PositionComponent; // Already in engine/include/ecs/components/position_component.h
        // struct VelocityComponent; // Already in engine/include/ecs/components/velocity_component.h

        class MovementSystem {
        public:
            MovementSystem(flecs::world& ecsWorld);

        private:
            // System registration can be done in the constructor
            // Or a separate registration function can be called
        };
    }
}

#endif // MOVEMENT_SYSTEM_H
