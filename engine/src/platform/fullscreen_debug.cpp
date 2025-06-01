\
#include "platform/fullscreen_debug.h"

namespace FullscreenDebug {

    std::ofstream debugLogFile;
    bool logOpened = false;

    void openLog(const std::string& projectRoot) {
        if (logOpened) return;
        std::string logPath = projectRoot + "debug_logs/fullscreen_debug.log";
        debugLogFile.open(logPath, std::ios::app); // Append to existing log
        if (debugLogFile.is_open()) {
            logOpened = true;
            FS_LOG("Fullscreen debug log opened.");
        } else {
            std::cerr << "Failed to open fullscreen debug log at: " << logPath << std::endl;
        }
    }

    void closeLog() {
        if (logOpened && debugLogFile.is_open()) {
            FS_LOG("Fullscreen debug log closing.");
            debugLogFile.close();
            logOpened = false;
        }
    }

    void log(const std::string& message) {
        if (logOpened && debugLogFile.is_open()) {
            debugLogFile << message << std::endl;
        }
        // Also print to console for immediate visibility during debugging
        std::cout << message << std::endl;
    }

} // namespace FullscreenDebug
