#ifndef POSITION_COMPONENT_H
#define POSITION_COMPONENT_H

#include <glm/vec3.hpp>

namespace VoxelEngine {
    namespace ecs {
        struct PositionComponent {
            glm::vec3 position;
        };
    }
}

#endif // POSITION_COMPONENT_H
