#version 330 core

// Interpolated values from the vertex shaders
in vec2 UV;
in vec3 VertexPosition_worldspace;

in vec3 Normal_cameraspace;
in vec3 VertexToEye_cameraspace;
in vec3 PointLightDirection_cameraspace;
in vec3 VertexColorForFragment;

// Ouput data
out vec3 color;

// Values that stay constant for the whole mesh.
uniform sampler2D myTextureSampler;
uniform mat4 MV;
uniform vec3 LightPosition_worldspace;

void main(){

	// Light emission properties
	// You probably want to put them as uniforms
	vec3 LightColor = vec3(1,1,1);
	float LightPower = 100.0f;
	
	// Material properties
	vec3 MaterialDiffuseColor = texture( myTextureSampler, UV ).rgb;
	vec3 MaterialAmbientColor = vec3(0.1,0.1, 0.1) * MaterialDiffuseColor;
	vec3 MaterialSpecularColor = vec3(0.6,0.6,0.6);

	// Distance to the light
	float distancePowered = length( LightPosition_worldspace - VertexPosition_worldspace );
	distancePowered *= distancePowered;

	// Normal of the computed fragment, in camera space
	vec3 n = normalize( Normal_cameraspace );

	// Direction of the light (from the fragment to the light)
	vec3 l = normalize( PointLightDirection_cameraspace );

	// Cosine of the angle between the normal and the light direction, 
	// clamped above 0
	//  - light is at the vertical of the triangle -> 1
	//  - light is perpendicular to the triangle -> 0
	//  - light is behind the triangle -> 0
	float cosTheta = clamp( dot( n,l ), 0, 1 );
	
	// Eye vector (towards the camera)
	vec3 E = normalize(VertexToEye_cameraspace);

	// ����Ʈ ������ ����� �����Ѵ�, ����Ʈ�� �ݻ�Ǽ� ī�޶�� ���� ������ ��Ÿ�����ϹǷ�
	// Direction in which the triangle reflects the light
	vec3 R = reflect(-l,n);

	// Cosine of the angle between the Eye vector and the Reflect vector,
	// clamped to 0
	//  - Looking into the reflection -> 1
	//  - Looking elsewhere -> < 1
	float cosAlpha = clamp( dot( E,R ), 0,1 );

	float specularPower = 5;
	
	color = 
		// Ambient : simulates indirect lighting
		MaterialAmbientColor +
		// Diffuse : "color" of the object
		MaterialDiffuseColor * LightColor * LightPower * cosTheta / distancePowered +
		// Specular : reflective highlight, like a mirror
		MaterialSpecularColor * LightColor * LightPower * pow(cosAlpha,specularPower) / distancePowered;

	// �� ���� ���ؽ��÷��ϰ� �� �÷��� ��� ������ ���̳�?
	// ���� ���� diffuse �� ��������� �� ����!
	color *= VertexColorForFragment;
}