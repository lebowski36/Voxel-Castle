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
    // Render a solid green color for debugging
    FragColor = vec4(0.0, 1.0, 0.0, 1.0);
}
