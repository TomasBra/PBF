#version 430 core
out vec4 FragColor;

uniform vec3 color;

in vec3 pos;

void main()
{
   FragColor = vec4(pos.xyz, 1.0f);
   FragColor = vec4(0.2f, 0.5f, 0.2, 1.0f);
}