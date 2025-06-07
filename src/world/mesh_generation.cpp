#include "MeshGeneration.h"

// Helper function to get the correct texture ID for a specific face of a block
int getTextureIdForFace(VoxelType blockType, CubeFace face) {
    switch (blockType) {
        case VoxelType::GRASS:
            switch (face) {
                case CubeFace::TOP:
                    return 3; // Grass top texture
                case CubeFace::BOTTOM:
                    return 2; // Dirt bottom texture
                default: // FRONT, BACK, LEFT, RIGHT
                    return 2; // Dirt side texture
            }
            break;
            
        case VoxelType::WOOD:
            switch (face) {
                case CubeFace::TOP:
                case CubeFace::BOTTOM:
                    return 4; // Wood end grain texture
                default: // FRONT, BACK, LEFT, RIGHT
                    return 4; // Wood bark texture (same for now, can differentiate later)
            }
            break;
            
        case VoxelType::SAND:
            return 7; // Sand uses same texture for all faces
            
        case VoxelType::STONE:
            return 1; // Stone uses same texture for all faces
            
        case VoxelType::DIRT:
            return 2; // Dirt uses same texture for all faces
            
        case VoxelType::LEAVES:
            return 5; // Leaves use same texture for all faces
            
        case VoxelType::WATER:
            return 6; // Water uses same texture for all faces
            
        case VoxelType::AIR:
        default:
            return 0; // Air/default
    }
}

void MeshGeneration::addQuad(std::vector<Vertex>& vertices, std::vector<unsigned int>& indices,
                           const glm::vec3& position, CubeFace face, VoxelType blockType) {
    // Get the appropriate texture ID for this face
    int textureId = getTextureIdForFace(blockType, face);
    
    // Convert texture ID to UV coordinates
    float u = (textureId % 32) / 32.0f;
    float v = (textureId / 32) / 32.0f;
    float uvStep = 1.0f / 32.0f;
    
    // ...existing code for face vertices and normals...
    
    // Update texture coordinates based on the specific texture ID
    glm::vec2 uvBottomLeft(u, v + uvStep);
    glm::vec2 uvBottomRight(u + uvStep, v + uvStep);
    glm::vec2 uvTopLeft(u, v);
    glm::vec2 uvTopRight(u + uvStep, v);
    
    // ...existing code for adding vertices with updated UV coordinates...
}