#version 330 core
// -- postion: attribute index 0
// -- color: attribute index 1

layout(location = 0) in vec3 vPosition;
uniform mat4 modelTransform;

void main()
{
	gl_Position = modelTransform* vec4(vPosition, 1.0); //동차 좌표계로 값을 맞춤
}