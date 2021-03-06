// 디폴트 렌더타겟 렌더링용 VS

#version 330 core

layout (location = 0) in vec2 aQuadNDCPos;
layout (location = 1) in vec2 aTexCoords;

out vec2 TexCoords;

uniform mat4 V;
uniform mat4 VP;

void main()
{
    TexCoords = aTexCoords;
    gl_Position = vec4(aQuadNDCPos.x, aQuadNDCPos.y, 0.0, 1.0); 
}
