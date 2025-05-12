#version 330 core
out vec4 FragColor;

in vec2 vTexCoord;
in vec3 Normal; // Renamed from vNormal to match vertex shader output
in vec3 FragPos; // Added to match vertex shader output
in float vLight; // Added to match vertex shader output

uniform sampler2D uTextureSampler; // The texture sampler uniform
// uniform vec3 uLightPos; // Example, if lighting is added
// uniform vec3 uViewPos;  // Example, if lighting is added
// uniform vec3 uLightColor; // Example, if lighting is added

void main() {
    vec4 sampledColor = texture(uTextureSampler, vTexCoord);
    
    // Apply lighting to make texture more visible
    float lightIntensity = max(0.5, vLight); // Ensure minimum lighting
    
    // Simply render the texture color with lighting applied
    FragColor = vec4(sampledColor.rgb * lightIntensity, sampledColor.a);
    
    // Add a small debug indicator in the bottom-left corner
    if (vTexCoord.x < 0.02 && vTexCoord.y < 0.02) {
        FragColor = vec4(1.0, 1.0, 0.0, 1.0); // Yellow dot to verify shader execution
    }
    
    // Fallback - if we get invalid texture coordinates or transparent pixels, show a solid color
    if (vTexCoord.x < 0.0 || vTexCoord.x > 1.0 || vTexCoord.y < 0.0 || vTexCoord.y > 1.0 || sampledColor.a < 0.1) {
        FragColor = vec4(0.7, 0.3, 0.3, 1.0); // Red-brown fallback color
    }
}
