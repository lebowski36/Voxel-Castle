#version 330 core
in vec3 FragPos;
in vec3 Normal;
in vec2 TexCoord;

out vec4 FragColor;

void main() {
    // Output solid red for debug visibility
    FragColor = vec4(1.0, 0.0, 0.0, 1.0);
}
