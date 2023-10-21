#version 330 core
in vec3 fcolor;
in vec2 ftex_vertex;

out vec4 FragColor;
    
uniform sampler2D ftexture;

void main()
{
    FragColor = texture(ftexture, ftex_vertex);
}