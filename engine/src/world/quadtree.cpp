#include "world/quadtree.h"
#include <algorithm>

namespace world {

QuadtreeNode::QuadtreeNode(const AABB2D& bounds, int level)
    : bounds(bounds), level(level) {}

void QuadtreeNode::insert(int32_t x, int32_t z, VoxelCastle::World::ChunkColumn* column) {
    if (!bounds.contains(x, z)) return;
    if (children[0]) {
        for (auto& child : children) {
            if (child && child->bounds.contains(x, z)) {
                child->insert(x, z, column);
                return;
            }
        }
    }
    positions.emplace_back(x, z);
    columns.push_back(column);
    if (columns.size() > MAX_OBJECTS && level < MAX_LEVELS) {
        if (!children[0]) subdivide();
        // Re-insert objects into children
        for (size_t i = 0; i < columns.size(); ++i) {
            for (auto& child : children) {
                if (child && child->bounds.contains(positions[i].first, positions[i].second)) {
                    child->insert(positions[i].first, positions[i].second, columns[i]);
                    break;
                }
            }
        }
        positions.clear();
        columns.clear();
    }
}

bool QuadtreeNode::remove(int32_t x, int32_t z) {
    if (!bounds.contains(x, z)) return false;
    for (size_t i = 0; i < positions.size(); ++i) {
        if (positions[i].first == x && positions[i].second == z) {
            positions.erase(positions.begin() + i);
            columns.erase(columns.begin() + i);
            return true;
        }
    }
    if (children[0]) {
        for (auto& child : children) {
            if (child && child->remove(x, z)) return true;
        }
    }
    return false;
}

VoxelCastle::World::ChunkColumn* QuadtreeNode::find(int32_t x, int32_t z) {
    if (!bounds.contains(x, z)) return nullptr;
    for (size_t i = 0; i < positions.size(); ++i) {
        if (positions[i].first == x && positions[i].second == z) {
            return columns[i];
        }
    }
    if (children[0]) {
        for (auto& child : children) {
            if (child && child->bounds.contains(x, z)) {
                return child->find(x, z);
            }
        }
    }
    return nullptr;
}

void QuadtreeNode::queryRegion(const AABB2D& region, std::vector<VoxelCastle::World::ChunkColumn*>& out) const {
    if (!bounds.intersects(region)) return;
    for (size_t i = 0; i < positions.size(); ++i) {
        if (region.contains(positions[i].first, positions[i].second)) {
            out.push_back(columns[i]);
        }
    }
    if (children[0]) {
        for (const auto& child : children) {
            if (child) child->queryRegion(region, out);
        }
    }
}

void QuadtreeNode::subdivide() {
    int32_t xMid = (bounds.xMin + bounds.xMax) / 2;
    int32_t zMid = (bounds.zMin + bounds.zMax) / 2;
    children[0] = std::make_unique<QuadtreeNode>(AABB2D{bounds.xMin, bounds.zMin, xMid, zMid}, level + 1);
    children[1] = std::make_unique<QuadtreeNode>(AABB2D{xMid+1, bounds.zMin, bounds.xMax, zMid}, level + 1);
    children[2] = std::make_unique<QuadtreeNode>(AABB2D{bounds.xMin, zMid+1, xMid, bounds.zMax}, level + 1);
    children[3] = std::make_unique<QuadtreeNode>(AABB2D{xMid+1, zMid+1, bounds.xMax, bounds.zMax}, level + 1);
}

Quadtree::Quadtree(const AABB2D& worldBounds) {
    root = std::make_unique<QuadtreeNode>(worldBounds, 0);
}

void Quadtree::insert(int32_t x, int32_t z, VoxelCastle::World::ChunkColumn* column) {
    if (root) root->insert(x, z, column);
}

bool Quadtree::remove(int32_t x, int32_t z) {
    return root ? root->remove(x, z) : false;
}

VoxelCastle::World::ChunkColumn* Quadtree::find(int32_t x, int32_t z) {
    return root ? root->find(x, z) : nullptr;
}

std::vector<VoxelCastle::World::ChunkColumn*> Quadtree::queryRegion(const AABB2D& region) const {
    std::vector<VoxelCastle::World::ChunkColumn*> result;
    if (root) root->queryRegion(region, result);
    return result;
}

} // namespace world
