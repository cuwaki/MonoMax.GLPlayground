#version 330 core

// Input vertex data, different for all executions of this shader.
layout(location = 0) in vec3 vertexPosition_modelspace;
layout(location = 1) in vec2 vertexUV;
layout(location = 2) in vec3 vertexNormal_modelspace;
layout(location = 3) in vec3 vertexColor;

// Output data ; will be interpolated for each fragment.
// 정점마다 계산해서 넘기면, 프래그먼트마다 보간되서 들어간다는 말인데??
out vec2 UV;
out vec3 VertexPosition_worldspace;
out vec3 Normal_cameraspace;
out vec3 VertexToEye_cameraspace;
out vec3 PointLightDirection_cameraspace;
out vec3 VertexColorForFragment;

// Values that stay constant for the whole mesh.
uniform mat4 MVP;
uniform mat4 V;
uniform mat4 M;
uniform vec3 LightPosition_worldspace;

void main(){

	// Output position of the vertex, in clip space : MVP * position
	gl_Position =  MVP * vec4(vertexPosition_modelspace,1);
	
	// Position of the vertex, in worldspace : M * position
	VertexPosition_worldspace = (M * vec4(vertexPosition_modelspace,1)).xyz;
	
	// Vector that goes from the vertex to the camera, in camera space.
	// In camera space, the camera is at the origin (0,0,0).
	vec3 vertexPosition_cameraspace = ( V * M * vec4(vertexPosition_modelspace,1)).xyz;
	VertexToEye_cameraspace = vec3(0,0,0) - vertexPosition_cameraspace;

	// 여기서의 라이트는 디렉셔널이 아니고 포인트 라이트이다
	// 그래서 Vertext 마다 LightDirection_cam 이게 다 다르게 나간다

	// Vector that goes from the vertex to the light, in camera space. M is ommited because it's identity.
	vec3 LightPosition_cameraspace = ( V * vec4(LightPosition_worldspace,1)).xyz;
	PointLightDirection_cameraspace = LightPosition_cameraspace + VertexToEye_cameraspace;
	
	// Normal of the the vertex, in camera space
	Normal_cameraspace = ( V * M * vec4(vertexNormal_modelspace,0)).xyz; // Only correct if ModelMatrix does not scale the model ! Use its inverse transpose if not.
	
	// UV of the vertex. No special space for this one.
	UV = vertexUV;

	VertexColorForFragment = vertexColor;	// out
}

