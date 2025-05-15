#version 330 core
out vec4 FragColor;

in vec2 v_quad_uv; // Changed from vTexCoord
in vec2 v_atlas_tile_origin_uv; // New input
in vec4 vDebugColor;
// in float vLight; // Assuming vLight might be used later for lighting calculations

uniform sampler2D uTextureSampler; // Existing texture atlas sampler
uniform vec2 u_tile_uv_span;      // New uniform: vec2(TILE_UV_WIDTH, TILE_UV_HEIGHT)
uniform int uDebugRenderMode;     // 0=normal, 1=wireframe, 2=face debug

void main()
{
    // Calculate the UV within the single tile (0-1 range repeating)
    vec2 tiled_uv = fract(v_quad_uv);

    // Map the tiled UV to the correct tile in the atlas
    vec2 final_atlas_uv = v_atlas_tile_origin_uv + tiled_uv * u_tile_uv_span;

    if (uDebugRenderMode == 2) {
        FragColor = vDebugColor;
        FragColor.a = 1.0;
    } else {
        FragColor = texture(uTextureSampler, final_atlas_uv);
        // Discard fragment if alpha is below a threshold (e.g., for transparent parts of textures)
        if(FragColor.a < 0.1) {
            discard;
        }
        // Placeholder for lighting - can be re-added if vLight is passed and used
        // FragColor.rgb *= vLight; 
    }
}
