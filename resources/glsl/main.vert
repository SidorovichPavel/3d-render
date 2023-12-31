#version 330 core
layout (location = 0) in vec2 position;
layout (location = 1) in vec3 color;

out vec3 fcolor;

void main()
{
    gl_Position = vec4(position.x, position.y, 1.f, 1.0);
    fcolor = color;
}