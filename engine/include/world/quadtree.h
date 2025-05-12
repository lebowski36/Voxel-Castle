#pragma once

#include <vector>
#include <memory>
#include <cstdint>

namespace VoxelCastle { namespace World { class ChunkColumn; } }
namespace world {

// Axis-aligned bounding box for region queries
struct AABB2D {
    int32_t xMin, zMin, xMax, zMax;
    bool contains(int32_t x, int32_t z) const {
        return x >= xMin && x <= xMax && z >= zMin && z <= zMax;
    }
    bool intersects(const AABB2D& other) const {
        return !(xMax < other.xMin || xMin > other.xMax || zMax < other.zMin || zMin > other.zMax);
    }
};

class QuadtreeNode {
public:
    static constexpr int MAX_OBJECTS = 8;
    static constexpr int MAX_LEVELS = 8;

    AABB2D bounds;
    int level;
    std::vector<std::pair<int32_t, int32_t>> positions; // (x, z) positions
    std::vector<VoxelCastle::World::ChunkColumn*> columns;
    std::unique_ptr<QuadtreeNode> children[4];

    QuadtreeNode(const AABB2D& bounds, int level);
    void insert(int32_t x, int32_t z, VoxelCastle::World::ChunkColumn* column);
    bool remove(int32_t x, int32_t z);
    VoxelCastle::World::ChunkColumn* find(int32_t x, int32_t z);
    void queryRegion(const AABB2D& region, std::vector<VoxelCastle::World::ChunkColumn*>& out) const;
    void subdivide();
};

class Quadtree {
public:
    Quadtree(const AABB2D& worldBounds);
    void insert(int32_t x, int32_t z, VoxelCastle::World::ChunkColumn* column);
    bool remove(int32_t x, int32_t z);
    VoxelCastle::World::ChunkColumn* find(int32_t x, int32_t z);
    std::vector<VoxelCastle::World::ChunkColumn*> queryRegion(const AABB2D& region) const;
private:
    std::unique_ptr<QuadtreeNode> root;
};

} // namespace world
