#pragma once

// Debug render mode for runtime toggling

enum class DebugRenderMode {
    NORMAL = 0,
    WIREFRAME = 1
};

// Extern declaration for global mode variable
extern DebugRenderMode g_debugRenderMode;
