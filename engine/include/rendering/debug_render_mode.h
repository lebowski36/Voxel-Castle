#pragma once

// Debug render mode for runtime toggling

enum class DebugRenderMode {
    NORMAL = 0,
    WIREFRAME = 1,
    FACE_DEBUG = 2
};

// Extern declaration for global mode variable
extern DebugRenderMode g_debugRenderMode;
