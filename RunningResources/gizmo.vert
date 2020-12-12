#version 330 core

layout(location = 0) in vec3 vertexPosition_modelspace;

uniform mat4 MVP;

// 여기 - 기즈모 렌더링 시스템으로 통합해야함
// 점 크기 조절에 대한 문서
// https://stackoverflow.com/questions/37753951/determine-gl-pointsize-from-distance-of-a-gl-point-to-the-camera
// https://learnopengl.com/Advanced-OpenGL/Advanced-GLSL
const float PointSize = 3.f;

void main()
{
	gl_PointSize = PointSize;
	
	gl_Position =  MVP * vec4(vertexPosition_modelspace,1.0f);
}
