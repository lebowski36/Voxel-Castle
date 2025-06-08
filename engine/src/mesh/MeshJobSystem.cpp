#include <string>
#include "utils/debug_logger_stub.h"

// Redirect verbose logs to file-based logging
DEBUG_LOG("MeshJobSystem", "Worker thread started");
DEBUG_LOG("MeshJobSystem", "Job enqueued, queue size: " + std::to_string(queueSize));
DEBUG_LOG("MeshJobSystem", "MeshJobSystem created successfully");