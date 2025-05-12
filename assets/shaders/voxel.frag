#version 330 core
in vec3 FragPos; // World space position
in vec3 Normal;  // World space normal
in float vLight;

out vec4 FragColor;

void main() {
    vec3 baseColor = vec3(1.0, 0.8, 0.5); // Sandstone-like for demo
    float light = clamp(vLight, 0.0, 1.0);
    FragColor = vec4(baseColor * light, 1.0);
}
