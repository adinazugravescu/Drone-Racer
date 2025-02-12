#version 330 core

layout(location = 0) in vec3 in_position;
layout(location = 1) in vec3 in_normal;

uniform mat4 Model;
uniform mat4 View;
uniform mat4 Projection;
uniform int isGrid; // 1 for grid, 0 for objects

out vec3 frag_position;
out vec3 frag_normal;
flat out int v_isGrid;

// ~ maps p to a random value in [0, 1)
float noise(vec2 p) {
    return fract(sin(dot(p, vec2(13, 78))) * 43750);
}

void main() {
    vec3 position = in_position;

    if (isGrid == 1) {
        float frequency = 0.2;
        float amplitude = 0.2;
        // y coordinate is adjusted by a random value based on its x and z coordinates
        position.y += noise(position.xz * frequency) * amplitude;
    }

    frag_position = vec3(Model * vec4(position, 1.0));
    frag_normal = normalize(mat3(transpose(inverse(Model))) * in_normal);

    v_isGrid = isGrid;

    gl_Position = Projection * View * vec4(frag_position, 1.0);
}
