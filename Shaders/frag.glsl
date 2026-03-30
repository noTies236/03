#version 410 core

in vec2 v_coord;

uniform sampler2D u_texture;

out vec4 color;

void main()
{
    color = texture(u_texture, v_coord) * vec4(0.7f, 0.4f, 0.4f, 0.9f);
} 