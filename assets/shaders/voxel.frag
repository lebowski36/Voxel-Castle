#version 330 core
in vec3 FragPos; 
in vec3 Normal;  
in float vLight;
in vec2 vTexCoord; // Added: Input for texture coordinates

out vec4 FragColor;

uniform sampler2D uTextureSampler; // Added: Texture sampler

void main() {
    vec4 texColor = texture(uTextureSampler, vTexCoord); // Sample from texture

    // Discard fragment if texture alpha is very low (e.g., for AIR blocks)
    if(texColor.a < 0.1) {
        discard;
    }

    float light = clamp(vLight, 0.2, 1.0); // Ensure a minimum light level
    FragColor = vec4(texColor.rgb * light, texColor.a); // Combine texture color with light and use texture alpha
}
