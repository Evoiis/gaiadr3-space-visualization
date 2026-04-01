#version 330 core
layout(location = 0) in vec3 in_pos;
// layout(location = 1) in vec3 in_next_pos;
layout(location = 2) in vec3 in_color_rgb;
layout(location = 3) in float in_size;
layout(location = 4) in float in_brightness;

uniform mat4 mvp_composite;
uniform float uPointScale;

out vec3 out_color_rgb;
out float out_brightness;
out float out_size;

void main() {
    vec4 pos = mvp_composite * vec4(in_pos, 1.0);
    gl_Position = pos;

    out_color_rgb = in_color_rgb;
    out_brightness = in_brightness;
    out_size = in_size;

    gl_PointSize = clamp((uPointScale / pos.w) * in_brightness * in_brightness * 4, 1.0, 512.0);
}
