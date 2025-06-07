#version 330 core
out vec4 FragColor;

in vec3 Normal;
in vec3 FragPos;
in float vLight;
in vec2 v_quad_uv; // Changed from vTexCoord
in vec2 v_atlas_tile_origin_uv; // New input
flat in int v_atlas_id; // Atlas ID (flat for integer)

// Multi-atlas support
uniform sampler2D uMainAtlasSampler;   // Main atlas (0)
uniform sampler2D uSideAtlasSampler;   // Side atlas (1)
uniform sampler2D uBottomAtlasSampler; // Bottom atlas (2)

// Legacy support
uniform sampler2D uTextureSampler;     // Fallback to main atlas for compatibility

uniform vec2 u_tile_uv_span;          // New uniform: vec2(TILE_UV_WIDTH, TILE_UV_HEIGHT)
uniform int uDebugRenderMode;         // 0=normal, 1=wireframe

// Lighting uniforms
uniform vec3 uLightDirection;         // Normalized directional light direction
uniform vec3 uLightColor;             // Directional light color
uniform vec3 uAmbientColor;           // Ambient light color
uniform float uAmbientStrength;       // Ambient light intensity

void main()
{
    // Calculate the UV within the single tile (0-1 range repeating)
    vec2 tiled_uv = fract(v_quad_uv);

    // Determine the correct tile UV span based on atlas type
    vec2 tile_uv_span;
    if (v_atlas_id == 0) {
        // Main atlas: 512x512 pixels, 16x16 tiles -> tile span (1/16, 1/16)
        tile_uv_span = vec2(1.0/16.0, 1.0/16.0);
    } else if (v_atlas_id == 1) {
        // Side atlas: 96x96 pixels, 32x32 tiles -> tile span (32/96, 32/96) = (1/3, 1/3)
        tile_uv_span = vec2(1.0/3.0, 1.0/3.0);
    } else if (v_atlas_id == 2) {
        // Bottom atlas: 32x32 pixels, single tile -> tile span (1.0, 1.0)
        tile_uv_span = vec2(1.0, 1.0);
    } else {
        // Fallback to main atlas tile span
        tile_uv_span = vec2(1.0/32.0, 1.0/32.0);
    }

    // Map the tiled UV to the correct tile in the atlas
    vec2 final_atlas_uv = v_atlas_tile_origin_uv + tiled_uv * tile_uv_span;

    // Sample from the correct atlas based on atlas_id
    vec4 texColor;
    if (v_atlas_id == 0) {
        texColor = texture(uMainAtlasSampler, final_atlas_uv);
    } else if (v_atlas_id == 1) {
        texColor = texture(uSideAtlasSampler, final_atlas_uv);
    } else if (v_atlas_id == 2) {
        texColor = texture(uBottomAtlasSampler, final_atlas_uv);
    } else {
        // Fallback to main atlas for unknown atlas IDs
        texColor = texture(uMainAtlasSampler, final_atlas_uv);
    }
    
    FragColor = texColor;
    
    // Discard fragment if alpha is below a threshold (e.g., for transparent parts of textures)
    if(FragColor.a < 0.1) {
        discard;
    }
    
    // Basic Lambert diffuse lighting calculation
    vec3 norm = normalize(Normal);
    float diffuse = max(dot(norm, -uLightDirection), 0.0);
    
    // Combine ambient and diffuse lighting
    vec3 ambient = uAmbientColor * uAmbientStrength;
    vec3 lighting = ambient + uLightColor * diffuse;
    
    // Apply lighting to texture color
    FragColor.rgb *= lighting;
}
