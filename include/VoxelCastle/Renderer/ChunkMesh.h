#ifndef CHUNK_MESH_H
#define CHUNK_MESH_H

#include "VoxelType.h"

class ChunkMesh {
public:
    // Helper method for face-based texture selection
    static int getTextureIdForFace(VoxelType blockType, CubeFace face);

    // ...existing code...
};

#endif // CHUNK_MESH_H