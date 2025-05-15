// Fragment shader for text rendering

#version 330 core
in vec2 TexCoords;
out vec4 color;

uniform sampler2D textAtlas; // Changed from textTextureSampler to textAtlas
uniform vec3 textColor;

void main() {
    // Sample the red channel of the texture for alpha
    vec4 sampled = vec4(1.0, 1.0, 1.0, texture(textAtlas, TexCoords).r);
    color = vec4(textColor, 1.0) * sampled;
}
