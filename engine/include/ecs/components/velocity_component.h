#ifndef VELOCITY_COMPONENT_H
#define VELOCITY_COMPONENT_H

#include <glm/vec3.hpp>

namespace VoxelEngine {
    namespace ecs {
        struct VelocityComponent {
            glm::vec3 velocity;
        };
    }
}

#endif // VELOCITY_COMPONENT_H
