#version 330 core
out vec4 FragColor;

in vec3 Normal;
in vec3 FragPos;
in float vLight;
in vec2 v_quad_uv; // Changed from vTexCoord
in vec2 v_atlas_tile_origin_uv; // New input

uniform sampler2D uTextureSampler; // Existing texture atlas sampler
uniform vec2 u_tile_uv_span;      // New uniform: vec2(TILE_UV_WIDTH, TILE_UV_HEIGHT)
uniform int uDebugRenderMode;     // 0=normal, 1=wireframe

// Lighting uniforms
uniform vec3 uLightDirection;     // Normalized directional light direction
uniform vec3 uLightColor;         // Directional light color
uniform vec3 uAmbientColor;       // Ambient light color
uniform float uAmbientStrength;   // Ambient light intensity

void main()
{
    // Calculate the UV within the single tile (0-1 range repeating)
    vec2 tiled_uv = fract(v_quad_uv);

    // Map the tiled UV to the correct tile in the atlas
    vec2 final_atlas_uv = v_atlas_tile_origin_uv + tiled_uv * u_tile_uv_span;

    FragColor = texture(uTextureSampler, final_atlas_uv);
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
