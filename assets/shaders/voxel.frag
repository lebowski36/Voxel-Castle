#version 330 core
in vec3 FragPos; // World space position
in vec3 Normal;  // World space normal

out vec4 FragColor;

void main() {
    vec3 lightDir = normalize(vec3(1.0, 1.0, 1.0)); // Changed light direction
    vec3 norm = normalize(Normal);
    float diff = max(dot(norm, lightDir), 0.0);
    vec3 diffuseColor = vec3(1.0, 0.0, 0.0) * diff; // Red color modulated by diffuse light

    // Add a little ambient light so it's not completely black in shadow
    vec3 ambientColor = vec3(1.0, 0.0, 0.0) * 0.5; // Further increased ambient intensity
    
    FragColor = vec4(ambientColor + diffuseColor, 1.0);
}
