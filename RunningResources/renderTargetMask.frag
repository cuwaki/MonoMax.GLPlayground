// 마스킹용 렌더타겟 렌더링용 FS

#version 330 core

out vec4 FragColor;
in vec2 TexCoords;

uniform sampler2D readColorTexture;

#define MASKING_THREASHOLD 0.5f

void main()
{
    vec4 col = texture(readColorTexture, TexCoords).rgba;
	
	// 마스킹
	if(col.a < MASKING_THREASHOLD)
		discard;
		
    FragColor = col;
}
