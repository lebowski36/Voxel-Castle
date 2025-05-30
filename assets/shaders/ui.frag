#version 330 core

in vec2 TexCoord;
out vec4 FragColor;

uniform sampler2D uTexture;
uniform vec4 uColor;
uniform int uUseTexture;  // Using int instead of bool for compatibility

void main() {
    if (uUseTexture != 0) {
        vec4 texColor = texture(uTexture, TexCoord);
        // Multiply by color for tinting and preserve alpha channel
        // Make colors more vibrant for debugging
        FragColor = texColor * uColor;
        
        // Increase alpha by 50% for debugging visibility (but keep 0 as 0)
        if (FragColor.a > 0.0) {
            FragColor.a = min(FragColor.a * 1.5, 1.0);
        }
        
        // Only discard completely transparent pixels
        if (FragColor.a < 0.01) {
            discard;
        }
    } else {
        // For solid color UI elements, just use the color as is
        FragColor = uColor;
    }
}
