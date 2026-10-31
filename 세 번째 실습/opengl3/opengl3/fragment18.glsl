#version 330 core

uniform vec3 objectColor;

out vec4 Frag_Color;

void main()
{
    Frag_Color = vec4(objectColor, 1.0);
}