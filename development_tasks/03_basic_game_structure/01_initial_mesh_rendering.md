# 03.01 Initial Mesh Rendering

## Goal
Render a generated voxel mesh (from the greedy meshing system) in the OpenGL window so the user can see the result visually, not just in the console.

## Steps

1. **Prepare Mesh Data for Rendering**
    - [ ] Define a simple MeshRenderer class or function to upload mesh data (vertices, indices) to OpenGL buffers (VBO, VAO, EBO).
    - [ ] Ensure the mesh vertex structure matches the shader input (position, normal, UV if needed).

2. **Write Basic Shaders**
    - [ ] Create a simple vertex shader and fragment shader for colored or basic lit rendering.
    - [ ] Load and compile shaders at startup.

3. **Integrate Mesh Generation and Rendering**
    - [ ] In your main loop, generate a test chunk and build its mesh using the greedy meshing system.
    - [ ] Upload the mesh to OpenGL buffers.
    - [ ] Render the mesh each frame.

4. **Camera Setup**
    - [ ] Set up a basic camera/view matrix (static or simple orbit for now).
    - [ ] (Optional) Add basic camera controls (WASD, mouse look, or arrow keys).

5. **Test and Verify**
    - [ ] Build and run the project.
    - [ ] Confirm that a voxel mesh appears in the window.
    - [ ] Adjust camera or mesh as needed for visibility.

## Notes
- Use a simple colored output for now; texture mapping can be added later.
- Keep the rendering code modular so it can be extended for chunk streaming, LOD, etc.

---
*Created: May 11, 2025 by GitHub Copilot*
