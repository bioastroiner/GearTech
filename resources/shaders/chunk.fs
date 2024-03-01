#version 330 core
out vec4 FragColor;
uniform float time;
in vec2 uv;
in float dist;
in float lightlevel;
uniform sampler2D text;

uniform float fog_level = 50;

void main()
{
    // dist / fog_level
    if (dist - 20 > fog_level) discard;
    float fog_mul = clamp(dist / (fog_level), 0.0f, 1.0f);
    vec4 text_color = texture(text, uv);
    FragColor = mix(text_color, vec4(1.0f), fog_mul);
}
