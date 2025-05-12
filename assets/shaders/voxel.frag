#version 330 core
in vec3 FragPos; 
in vec3 Normal;  
in float vLight;
in vec2 vTexCoord; // Added: Input for texture coordinates

out vec4 FragColor;

uniform sampler2D uTextureSampler; // Added: Texture sampler

void main() {
    // --- DEBUG OUTPUT MODES ---
    // Uncomment ONE of the following blocks at a time to debug:

    // 1. Output constant color (should show red floor if shader is running)
    //FragColor = vec4(1.0, 0.0, 0.0, 1.0); return;

    // 2. Output UVs as color (should show a gradient if UVs are in [0,1])
    FragColor = vec4(vTexCoord, 0.0, 1.0); return;

    // 3. Output sampled texture color only (no lighting)
    vec4 texColor = texture(uTextureSampler, vTexCoord);
    FragColor = texColor;
    //return;

    // 4. Output sampled texture color with lighting (original)
    //if(texColor.a < 0.1) {
    //    discard;
    //}
    //float light = clamp(vLight, 0.2, 1.0);
    //FragColor = vec4(texColor.rgb * light, texColor.a);
}
