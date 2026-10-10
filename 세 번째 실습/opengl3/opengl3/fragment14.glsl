#version 330 core

in vec3 vertexColor;
uniform vec3 faceColor;
// false: coordinate axes use a uniform color; true: objects use vertex colors.
uniform bool useVertexColor;
out vec4 Frag_Color;

void main()
{
    if (useVertexColor)
        Frag_Color = vec4(vertexColor, 1.0);
    else
        Frag_Color = vec4(faceColor, 1.0);
}
