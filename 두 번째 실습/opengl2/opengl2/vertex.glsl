#version 330 core
// -- postion: attribute index 0
// -- color: attribute index 1

layout(location = 0) in vec3 vPosition;
layout(location = 1) in vec3 vColor;
out vec3 out_Color;

void main()
{
	gl_Position = vec4(vPosition, 1.0); //동차 좌표계로 값을 맞춤
	out_Color = vColor;
}