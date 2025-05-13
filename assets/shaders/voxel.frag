#version 330 core
out vec4 FragColor;

in vec2 vTexCoord; // Changed from TexCoords to match vertex shader

uniform sampler2D uTextureSampler; // Changed from sampler2DArray

void main()
{
    // Visualize texture coordinates
    FragColor = vec4(vTexCoord.x, vTexCoord.y, 0.0, 1.0); 

    // Original texturing code:
    // FragColor = texture(uTextureSampler, vTexCoord); // Sample as a 2D texture

    // if (FragColor.a < 0.1)
    //     discard; // Discard transparent fragments
}
