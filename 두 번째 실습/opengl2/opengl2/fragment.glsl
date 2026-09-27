#version 330 core
// -- out _Color: 버텍스 셰이더에서 전달받은 색상값을 프래그먼트 셰이더에서 출력하기 위해 선언
// -- Frag_Color: 출력할 색상의 값으로 프레임 버퍼로 전달 됨

in vec3 out_Color;
out vec4 Frag_Color;

void main()
{
	Frag_Color = vec4(out_Color, 1.0); //1.0은 알파값으로 투명도를 나타냄. 1.0은 불투명, 0.0은 완전 투명
}