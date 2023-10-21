#version 330 core
layout (location = 0) in vec3 position;
layout (location = 1) in vec3 color;
layout (location = 2) in vec2 tex_vertex;

out vec3 fcolor;
out vec2 ftex_vertex;

void main()
{
    gl_Position = vec4(position.x, position.y, position.z, 1.0);
    fcolor = color;
    ftex_vertex = tex_vertex;
}