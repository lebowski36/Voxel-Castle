# Two-Phase Greedy Meshing Algorithm

## Current Issue
Missing faces in the greedy mesh generation, especially when an entire rectangular face should be generated but isn't. Meshes are missing even though the faces should be visible.

## Root Cause Analysis
The current greedy meshing algorithm has issues with:
1. Mask handling (inconsistent marking)
2. Face visibility detection
3. Merging logic that may skip faces

## Two-Phase Solution

### Phase 1: Face Detection
- Mark all visible faces first in a dedicated mask
- Detect every face independently of merging
- Store voxel types for each face

### Phase 2: Greedy Merging
- Use the visibility mask as input for greedy merging
- Process only marked faces from Phase 1
- Merge contiguous faces of the same type

## Helper Methods

```cpp
// Helper to determine if a face is visible
bool isFaceVisible(const int x[3], const int q[3], const std::function<Voxel(int,int,int)>& getVoxel);

// Helper to check voxel properties (solid, same type)
bool isSameVoxelType(const int coords[3], VoxelType type, const std::function<Voxel(int,int,int)>& getVoxel);

// Helper to process a single slice along an axis
void processSlice(VoxelMesh& mesh, int d, int u, int v, int dir, int x[3], int q[3], 
                 const int chunk_dims[3], const TextureAtlas& atlas,
                 const std::function<Voxel(int,int,int)>& getVoxel);

// Helper for greedy expansion logic
void computeQuadDimensions(int x[3], int u, int v, int& h_quad, int& w_quad, 
                          const int chunk_dims[3], VoxelType voxelType,
                          const std::vector<bool>& visible_faces_mask,
                          const std::function<Voxel(int,int,int)>& getVoxel);

// Helper for quad generation
void generateQuad(VoxelMesh& mesh, int d, int u, int v, int dir, 
                 const int x[3], const int q[3],
                 int h_quad, int w_quad, 
                 VoxelType voxelType, const TextureAtlas& atlas);
```

## Algorithm Flow

1. For each axis (d = 0,1,2) and direction (dir = -1,+1):
   - Process each slice along the axis

2. For each slice:
   - Phase 1: Build visibility mask by checking all voxels
   - Phase 2: Apply greedy merging only to visible faces

3. For each visible, unprocessed face:
   - Compute greedy quad dimensions (width/height)
   - Generate the quad
   - Mark processed faces

## Benefits

1. Guaranteed face detection (no missing faces)
2. Separation of concerns (face detection vs. merging)
3. More maintainable code with helper methods
4. Better debugging capabilities

## Notes

- The existing conditional mask marking was causing issues:
  ```cpp
  // Only mark the mask if the voxel exists and has the same properties
  Voxel check_voxel = getVoxel(check_coords[0], check_coords[1], check_coords[2]);
  if (check_voxel.id == static_cast<uint8_t>(current_face_voxel_type)) {
      mask[mask_index] = true;
  }
  ```
  This was unreliable and could skip marking some faces.

- The two-phase approach ensures all visible faces are accounted for before the greedy merging begins.
