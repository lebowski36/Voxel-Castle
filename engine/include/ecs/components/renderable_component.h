#ifndef RENDERABLE_COMPONENT_H
#define RENDERABLE_COMPONENT_H

namespace VoxelEngine {
    namespace ecs {
        // For now, a simple marker component, can be expanded later.
        struct RenderableComponent {
            bool isVisible = true;
            // int modelId; // Future: ID for a model asset
            // int materialId; // Future: ID for a material asset
        };
    }
}

#endif // RENDERABLE_COMPONENT_H
