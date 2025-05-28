# Task: Implement Basic Lighting Model

**Date Created:** 2025-05-12
**Status:** In Progress
**Depends On:** Mesh Generation, Rendering Pipeline
**Leads To:** Improved Visuals, Atmosphere

## 1. Overview
This task involves implementing a simple lighting model to enhance the visual appearance of the rendered voxel world. The infrastructure appears to be largely in place, with shaders already having lighting inputs and commented lighting calculations. The focus will be on enabling and configuring the lighting system.

## 2. Goals
*   Enable the existing lighting infrastructure in shaders
*   Implement a basic lighting model (ambient + directional light)
*   Add proper lighting uniforms to the rendering pipeline
*   Ensure proper normal calculation for voxel faces

## 3. Sub-Tasks

### Phase 1: Assess Current Lighting Infrastructure
*   [x] **3.1. Analyze Current Shader Lighting Support**
    *   [x] 3.1.1. Review vertex shader `aLight` input and `vLight` output to understand current lighting data flow
    *   [x] 3.1.2. Examine fragment shader commented lighting code: `// FragColor.rgb *= vLight;`
    *   [x] 3.1.3. Check if mesh generation properly calculates and assigns lighting values per vertex
    *   [x] 3.1.4. Verify that normal vectors are correctly calculated and passed to shaders

*   [x] **3.2. Define Basic Lighting Model**
    *   [x] 3.2.1. Decide on lighting model: Ambient + Single Directional Light (sun-like)
    *   [x] 3.2.2. Define lighting parameters:
        *   Ambient light color/intensity (e.g., soft gray/blue for overall illumination)
        *   Directional light direction (e.g., sun from above-southwest)
        *   Directional light color/intensity (e.g., warm yellow/white)
    *   [x] 3.2.3. Choose simple lighting calculation: Lambert diffuse model (dot product of normal and light direction)

### Phase 2: Update Shaders for Lighting
*   [x] **3.3. Add Lighting Uniforms to Shaders**
    *   [x] 3.3.1. Add lighting uniforms to vertex or fragment shader:
        *   `uniform vec3 uLightDirection;` (normalized directional light direction)
        *   `uniform vec3 uLightColor;` (directional light color)
        *   `uniform vec3 uAmbientColor;` (ambient light color)
        *   `uniform float uAmbientStrength;` (ambient light intensity)
    *   [x] 3.3.2. Pass Normal from vertex to fragment shader (already done: `out vec3 Normal;`)

*   [x] **3.4. Implement Lighting Calculation in Fragment Shader**
    *   [x] 3.4.1. Uncomment and enhance the lighting code in `voxel.frag`
    *   [x] 3.4.2. Implement basic Lambert diffuse lighting:
        *   Calculate `diffuse = max(dot(normalize(Normal), -uLightDirection), 0.0);`
        *   Combine: `vec3 lighting = uAmbientColor * uAmbientStrength + uLightColor * diffuse;`
        *   Apply: `FragColor.rgb *= lighting;`
    *   [x] 3.4.3. Ensure lighting doesn't oversaturate colors (clamp if necessary)

### Phase 3: Integrate Lighting Uniforms into Rendering Pipeline
*   [x] **3.5. Add Lighting Uniforms to MeshRenderer**
    *   [x] 3.5.1. Modify `MeshRenderer::draw()` or related rendering code to set lighting uniforms
    *   [x] 3.5.2. Define default lighting values:
        *   Light direction: `glm::vec3(-0.5f, -1.0f, -0.3f)` (sun from above-southwest)
        *   Light color: `glm::vec3(1.0f, 0.95f, 0.8f)` (warm white)
        *   Ambient color: `glm::vec3(0.3f, 0.4f, 0.6f)` (cool ambient)
        *   Ambient strength: `0.3f`
    *   [x] 3.5.3. Use `glUniform3fv()` and `glUniform1f()` to pass values to shaders

### Phase 4: Test and Refine
*   [x] **3.6. Test Basic Lighting**
    *   [x] 3.6.1. Build and run the application
    *   [x] 3.6.2. Observe voxel surfaces for proper lighting:
        *   Top faces should be brighter (facing upward toward light)
        *   Side faces should have varying brightness based on orientation
        *   Shadowed areas should be visible but not completely dark (ambient lighting)
    *   [x] 3.6.3. Verify there are no rendering artifacts or broken lighting

*   [x] **3.7. Fine-tune Lighting Parameters**
    *   [x] 3.7.1. Adjust light direction for best visual effect
    *   [x] 3.7.2. Balance ambient vs directional lighting for good contrast without harsh shadows
    *   [x] 3.7.3. Ensure lighting works well with the procedurally generated terrain

## 4. Notes
*   The lighting infrastructure appears to be largely implemented but disabled
*   Focus on enabling existing systems rather than building from scratch
*   Start with simple ambient + directional lighting for immediate visual improvement
*   Normal calculation in greedy meshing should already be correct for face-based lighting
*   Consider performance impact minimal since lighting calculation is per-fragment

---
**Last Updated:** 2025-05-28
