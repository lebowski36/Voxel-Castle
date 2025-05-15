#version 330 core
layout(location = 0) in vec3 aPos;
layout(location = 1) in vec3 aNormal;
layout(location = 2) in vec2 a_quad_uv; // Changed from aTexCoord
layout(location = 3) in vec2 a_atlas_tile_origin_uv; // New attribute
layout(location = 4) in float aLight; // Adjusted location
layout(location = 5) in vec4 aDebugColor; // Debug color for face ID visualization

uniform mat4 uModel;
uniform mat4 uView;
uniform mat4 uProjection;

out vec3 Normal;
out vec3 FragPos;
out float vLight;
out vec2 v_quad_uv; // Changed from vTexCoord
out vec2 v_atlas_tile_origin_uv; // New output
out vec4 vDebugColor;

void main() {
    gl_Position = uProjection * uView * uModel * vec4(aPos, 1.0);
    FragPos = vec3(uModel * vec4(aPos, 1.0));
    Normal = mat3(transpose(inverse(uModel))) * aNormal;
    vLight = aLight;
    v_quad_uv = a_quad_uv; // Pass quad_uv to fragment shader
    v_atlas_tile_origin_uv = a_atlas_tile_origin_uv; // Pass atlas_tile_origin_uv to fragment shader
    vDebugColor = aDebugColor;
}
