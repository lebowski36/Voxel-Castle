#version 330 core
out vec4 FragColor;

in vec2 vTexCoord;
// in vec3 Normal; // Unused for now
// in vec3 FragPos; // Unused for now
// in float vLight; // Unused for now

uniform sampler2D uTextureSampler;

void main() {
    // Check for invalid texture coordinates first.
    if (vTexCoord.x < 0.0 || vTexCoord.x > 1.0 || vTexCoord.y < 0.0 || vTexCoord.y > 1.0) {
        FragColor = vec4(1.0, 0.0, 1.0, 1.0); // Magenta for out-of-bounds UVs
        return;
    }

    vec4 sampledColor = texture(uTextureSampler, vTexCoord);
    FragColor = sampledColor; // Output raw texture color

    // If raw texture color's alpha is very low, make it red to see if it's transparent (like AIR)
    if (FragColor.a < 0.1) {
        FragColor = vec4(1.0, 0.0, 0.0, 1.0); // Red for transparent
    } else {
        // If it's opaque, but its RGB is black, make it blue
        if (FragColor.r < 0.01 && FragColor.g < 0.01 && FragColor.b < 0.01) {
            FragColor = vec4(0.0, 0.0, 1.0, 1.0); // Blue for opaque black
        }
        // Otherwise, FragColor remains the sampledColor (should be grey for STONE)
    }
}
