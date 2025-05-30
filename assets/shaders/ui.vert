#version 330 core

layout (location = 0) in vec2 aPosition;
layout (location = 1) in vec2 aTexCoord;

out vec2 TexCoord;

uniform mat4 projection;
uniform mat4 model;

void main() {
    TexCoord = aTexCoord;
    gl_Position = projection * model * vec4(aPosition, 0.0, 1.0);
}
