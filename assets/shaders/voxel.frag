#version 330 core
out vec4 FragColor;

in vec2 vTexCoord;

uniform sampler2D uTextureSampler;

void main()
{
    FragColor = texture(uTextureSampler, vTexCoord);
    if(FragColor.a < 0.1) {
        discard;
    }
}
