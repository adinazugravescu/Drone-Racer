#version 330 core

in vec3 frag_position;
in vec3 frag_normal;
flat in int v_isGrid;

uniform vec3 objectColor;

out vec4 out_color;

void main() {
    vec3 color = objectColor;

    if (v_isGrid == 1) {
        vec3 low_color = vec3(0.2, 0.6, 0.2);  // Green
        vec3 high_color = vec3(1.0, 0.0, 0.0); // Yellow
        float height_factor = clamp(frag_position.y / 0.75, 0.0, 1.0);
        color = mix(low_color, high_color, height_factor);
    }

    out_color = vec4(color, 1.0);
}
