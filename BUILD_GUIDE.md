# Voxel Fortress Build Guide

## System Requirements

### Minimum Requirements
- **OS**: Linux (Ubuntu 20.04+, Fedora 34+, Arch Linux), Windows 10+, or macOS 10.15+
- **CPU**: 64-bit processor with SSE4.1 support
- **RAM**: 4GB minimum, 8GB recommended
- **GPU**: OpenGL 3.3+ compatible graphics card
- **Storage**: 2GB free space for build

### Software Requirements
- **C++ Compiler**: GCC 9+, Clang 10+, or MSVC 2019+
- **CMake**: Version 3.20 or higher
- **Git**: For cloning the repository

## External Dependencies

All external libraries (SDL3, GLM, GLAD, and Flecs) are included in the repository under the `external/` directory. You don't need to download or install these separately.

## Platform-Specific Setup

### Ubuntu/Debian
```bash
# Update package manager
sudo apt update

# Install build essentials
sudo apt install build-essential cmake git

# Install OpenGL development libraries
sudo apt install libgl1-mesa-dev libglu1-mesa-dev

# Install additional dependencies
sudo apt install pkg-config
```

### Fedora/RHEL/CentOS
```bash
# Install development tools
sudo dnf groupinstall "Development Tools"
sudo dnf install cmake git

# Install OpenGL development libraries
sudo dnf install mesa-libGL-devel mesa-libGLU-devel

# Install pkg-config
sudo dnf install pkgconf-devel
```

### Arch Linux
```bash
# Install build tools
sudo pacman -S base-devel cmake git

# Install OpenGL libraries
sudo pacman -S mesa glu

# pkg-config is included in base-devel
```

### Windows (Visual Studio)
1. **Install Visual Studio 2019 or 2022** with C++ development tools
2. **Install CMake**: Download from https://cmake.org/download/
3. **Install Git**: Download from https://git-scm.com/download/win
4. **Graphics Drivers**: Ensure latest GPU drivers are installed

### Windows (MinGW/MSYS2)
```bash
# In MSYS2 terminal
pacman -S mingw-w64-x86_64-toolchain
pacman -S mingw-w64-x86_64-cmake
pacman -S mingw-w64-x86_64-mesa
pacman -S git
```

### macOS
```bash
# Install Xcode command line tools
xcode-select --install

# Install Homebrew (if not already installed)
/bin/bash -c "$(curl -fsSL https://raw.githubusercontent.com/Homebrew/install/HEAD/install.sh)"

# Install dependencies
brew install cmake git
```

## Build Process

### Step 1: Clone the Repository
```bash
# Clone to your desired location
git clone <repository-url> "Voxel Castle"
cd "Voxel Castle"
```

### Step 2: Initial Configuration
```bash
# Create build directory (first time only)
mkdir -p build

# Configure the build
cmake -S . -B build
```

### Step 3: Build the Project
```bash
# Build the project
cmake --build build

# Or for faster parallel build
cmake --build build -j $(nproc)  # Linux/macOS
cmake --build build -j %NUMBER_OF_PROCESSORS%  # Windows
```

### Step 4: Run the Game
```bash
# Linux/macOS
./build/bin/VoxelFortressGame

# Windows
build\bin\VoxelFortressGame.exe
```

### Directory Structure After Build
After a successful build, your directory structure will look like this:
```
Voxel Castle/
├── assets/           # Game assets (used at runtime)
│   ├── shaders/      # GLSL shader files
│   ├── textures/     # Texture files including atlas.png
│   └── ...
├── build/            # Build artifacts
│   ├── bin/          # Executables (VoxelFortressGame)
│   ├── lib/          # Compiled libraries
│   └── ...
└── ...
```

The game executable expects to find assets in the `assets/` directory relative to where the executable is run from. For proper asset loading, always run the game from the project root directory.

## What to Expect During First Build

When you first build Voxel Fortress, here's what happens:

1. **CMake Configuration**:
   - Processes CMakeLists.txt files
   - Detects compiler, OpenGL, and other system libraries
   - Sets up build targets

2. **External Libraries**:
   - Builds SDL3 from source (in external/SDL3)
   - Builds GLAD OpenGL loader (in external/glad)
   - Builds Flecs ECS library (in external/flecs)
   - Sets up GLM as a header-only library

3. **Engine Library**:
   - Compiles the core engine components
   - Builds voxel system, rendering pipeline, and world management

4. **Game Executable**:
   - Compiles game-specific code
   - Links against the engine library and external dependencies
   - Creates the executable at build/bin/VoxelFortressGame

The first build will take longer (typically 3-5 minutes) as it compiles all external libraries. Subsequent builds should be much faster, only recompiling changed files.

### Compilation Size
- **Initial Repository**: ~50-100 MB
- **After First Build**: ~500-800 MB (including debug symbols)
- **Full Build with Debug Info**: ~1-2 GB (if including all debug information)

### Common First-Run Messages
- Warnings during SDL3 compilation are generally safe to ignore
- Compiler warnings for external libraries can be disregarded
- "Setting up new project..." is normal at first run

## Development Build Commands

For active development, use these commands for cleaner output:

### Linux/macOS
```bash
# Build with clean console
clear && cmake --build build

# Run with clean console
clear && ./build/bin/VoxelFortressGame
```

### Windows (PowerShell)
```powershell
# Build with clean console
Clear-Host; cmake --build build

# Run with clean console
Clear-Host; .\build\bin\VoxelFortressGame.exe
```

## Troubleshooting

### Common Issues

#### "CMake not found"
- **Linux**: Install cmake package through your distribution's package manager
- **Windows**: Add CMake to your PATH environment variable
- **macOS**: Install via Homebrew or add to PATH

#### "Compiler not found" 
- **Linux**: Install `build-essential` (Ubuntu) or equivalent development tools
- **Windows**: Install Visual Studio with C++ tools or MinGW
- **macOS**: Run `xcode-select --install`

#### "OpenGL headers not found"
- **Linux**: Install mesa development packages (`libgl1-mesa-dev` on Ubuntu)
- **Windows**: Usually included with graphics drivers
- **macOS**: Should be included with Xcode tools

#### "SDL3 build errors"
- SDL3 is included as a submodule and builds automatically
- If issues persist, ensure git submodules are initialized:
  ```bash
  git submodule update --init --recursive
  ```

#### External Dependencies
The project uses several external libraries that are included as Git submodules:
- **SDL3**: Window management and input handling
- **GLAD**: OpenGL loader
- **Flecs**: Entity Component System
- **GLM**: Mathematics library

These are automatically built when you run the CMake build process, but you need to make sure the submodules are properly initialized:
```bash
git submodule update --init --recursive
```

#### Performance Issues
- **Low FPS**: Update graphics drivers and ensure hardware acceleration is enabled
- **High memory usage**: Normal for development builds; optimizations are ongoing
- **Compilation slow**: Use parallel builds with `-j` flag

### Debug Builds
```bash
# Configure for debug build
cmake -S . -B build -DCMAKE_BUILD_TYPE=Debug

# Build debug version
cmake --build build
```

### Release Builds
```bash
# Configure for optimized release
cmake -S . -B build -DCMAKE_BUILD_TYPE=Release

# Build release version
cmake --build build
```

## Advanced Configuration

### Custom Install Location
```bash
cmake -S . -B build -DCMAKE_INSTALL_PREFIX=/your/install/path
cmake --build build
cmake --install build
```

### Cross-compilation
See platform-specific CMake documentation for cross-compilation setups.

## Getting Help

- **Build Issues**: Check `build_log.txt` in the project root
- **Runtime Issues**: Check console output for error messages
- **Performance Problems**: Monitor frame rate output in console
- **Graphics Issues**: Verify OpenGL 3.3+ support with `glxinfo` (Linux)

## Log Files and Debugging

### Available Log Files
Voxel Fortress generates several log files during execution:

1. **`game_output.log`**: Console output redirected to file
2. **`build_log.txt`**: CMake build output and errors
3. **`debug_logs/`**: Directory containing component-specific logs:
   - **`Main_YYYYMMDD_HHMMSS.log`**: Main application log
   - **`InputManager_YYYYMMDD_HHMMSS.log`**: Input handling log
   - **`BlockPlacement_YYYYMMDD_HHMMSS.log`**: Block interaction log
   - And others based on active components

### Reading Debug Logs
Each log file contains timestamped entries with component name and severity level:
```
[2025-06-01 12:34:56.789] [Main] [INFO] Game initialization successful - starting main loop
```

If you encounter issues while running the game, these logs can provide valuable information for troubleshooting.

### Console Output
The game also outputs important information to the console, including:
- Frame rates
- Chunk loading status
- OpenGL errors
- Memory usage statistics

For the most comprehensive debugging experience, run the game from a terminal and watch for any error messages or warnings that appear.

## Contributing

Before contributing:
1. Ensure your build completes without warnings
2. Test that the game runs and basic features work
3. Follow the development workflow in `DEVELOPMENT.md`
4. Update this guide if you encounter new setup requirements
