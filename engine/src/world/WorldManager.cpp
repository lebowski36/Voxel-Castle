#include "utils/debug_logger_stub.h"

// Redirected dirty segment logs to a file-based logging system.

void WorldManager::markSegmentDirty(int columnX, int columnZ, int segmentY) {
    // ...existing logic for marking a segment dirty...

    // Redirect log to file-based logging system
    DEBUG_LOG("WorldManager", "Found dirty segment at column (" + std::to_string(columnX) + ", " + std::to_string(columnZ) + ") segment Y=" + std::to_string(segmentY));
}