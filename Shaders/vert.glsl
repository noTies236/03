#version 410 core

layout(location = 0) in vec3 position;
layout(location = 1) in vec2 coord;

out vec2 v_coord;

void main()
{
    v_coord = coord * 2;
    gl_Position = vec4(position.x, position.y, position.z, 1.0f);
}
