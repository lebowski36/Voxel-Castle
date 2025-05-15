// Vertex shader for text rendering

#version 330 core
layout (location = 0) in vec2 localPosition; // Corrected from '비용'
layout (location = 1) in vec2 texCoords_in; 
out vec2 TexCoords;

uniform mat4 projection;
uniform mat4 view;
uniform mat4 model; // Transforms local quad to world space

void main() {
    gl_Position = projection * view * model * vec4(localPosition, 0.0, 1.0);
    TexCoords = texCoords_in;
}
