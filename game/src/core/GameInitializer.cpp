#include "core/GameInitializer.h"
#include "platform/Window.h"
#include "flecs.h"
#include "world/world_manager.h"
#include "world/world_generator.h"
#include "rendering/texture_atlas.h"
#include "rendering/mesh_builder.h"
#include "rendering/mesh_renderer.h"
#include "SpectatorCamera.h"
#include "rendering/debug_utils.h"
#include "utils/debug_logger.h"
#include <iostream>
#include <glad/glad.h>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <chrono>

GameInitializer::InitResult GameInitializer::initialize(int screenWidth, int screenHeight, const char* projectRoot) {
    InitResult result;
    result.isRunning = false;
    result.gameWindow = std::make_unique<Window>("Voxel Fortress - Alpha", screenWidth, screenHeight);
    if (!result.gameWindow || !result.gameWindow->init()) {
        std::cerr << "Failed to initialize the game window!" << std::endl;
        return result;
    }
    glEnable(GL_DEPTH_TEST);
    glDepthFunc(GL_LESS);
    glEnable(GL_CULL_FACE);
    glCullFace(GL_BACK);
    glFrontFace(GL_CCW);
    glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
    result.ecs = std::make_unique<flecs::world>();
    result.worldManager = std::make_unique<VoxelCastle::World::WorldManager>();
    result.worldGenerator = std::make_unique<VoxelCastle::World::WorldGenerator>();
    result.textureAtlas = std::make_unique<VoxelEngine::Rendering::TextureAtlas>();
    result.meshBuilder = std::make_unique<VoxelEngine::Rendering::MeshBuilder>();
    result.meshRenderer = std::make_unique<VoxelEngine::Rendering::MeshRenderer>();
    VoxelEngine::Rendering::Debug::setupDebugAtlasQuad(std::string(projectRoot), screenWidth, screenHeight);
    VoxelEngine::Rendering::Debug::setupSingleTileDebugQuad(screenWidth, screenHeight);

    result.camera = std::make_unique<SpectatorCamera>(
        glm::vec3(16.0f, 24.0f, 48.0f), -90.0f, 0.0f, 70.0f,
        static_cast<float>(screenWidth) / static_cast<float>(screenHeight), 0.1f, 500.0f);
    glClearColor(0.1f, 0.2f, 0.3f, 1.0f);
    result.lastFrameTime = std::chrono::steady_clock::now();
    result.isRunning = true;
    return result;
}

void GameInitializer::shutdown(InitResult& resources, int screenWidth, int screenHeight, const char* projectRoot) {
    VoxelEngine::Rendering::Debug::cleanupDebugQuads();
    if (resources.gameWindow) {
        resources.gameWindow->cleanUp();
        resources.gameWindow.reset();
    }
    if (resources.meshRenderer) resources.meshRenderer.reset();
    if (resources.meshBuilder) resources.meshBuilder.reset();
    if (resources.worldManager) resources.worldManager.reset();
    if (resources.textureAtlas) resources.textureAtlas.reset();
    if (resources.ecs) resources.ecs.reset();
    if (resources.camera) resources.camera.reset();
    if (resources.worldGenerator) resources.worldGenerator.reset();

    resources.isRunning = false;
}
