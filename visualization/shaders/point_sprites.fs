#version 330 core
in vec3 out_color_rgb;
in float out_brightness;
in float out_size;

out vec4 fragColor;


void main() {

    vec2 uv = gl_PointCoord - vec2(0.5);
    float d = length(uv);
    if (d > 0.5) discard;

    // tight bright core
    float core  = exp(-d * 20.0) * out_brightness;

    // medium halo
    float halo  = exp(-d * 6.0) * out_brightness * .4;

    // wide soft glow
    float glow = exp(-d * 2.) * out_brightness * out_brightness * 0.1;

    float totalBrightness = (core + halo + glow);
    // float totalBrightness = core * out_size;
    // float totalBrightness = core * 2.; // Can grow size & brightness with multiplication

    float alpha = clamp(totalBrightness, 0.0, 1.0);

    fragColor = vec4(out_color_rgb * totalBrightness, alpha);
}
