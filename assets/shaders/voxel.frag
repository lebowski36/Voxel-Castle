#version 330 core
out vec4 FragColor;

in vec2 TexCoords;
in float Light;

uniform sampler2D uTextureSampler;

void main() {
    vec4 texColor = texture(uTextureSampler, TexCoords);
    
    // Optional: Magenta for out-of-bounds UVs (helps catch UV issues)
    if (TexCoords.x < 0.0 || TexCoords.x > 1.0 || TexCoords.y < 0.0 || TexCoords.y > 1.0) {
        FragColor = vec4(1.0, 0.0, 1.0, 1.0); // Magenta
        return;
    }

    if (texColor.a < 0.1) {
        discard; // Discard transparent fragments
    }

    FragColor = texColor * Light;
}
