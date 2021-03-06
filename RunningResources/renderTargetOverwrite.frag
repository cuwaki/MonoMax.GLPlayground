// 디폴트 렌더타겟 렌더링용 FS
// 그냥 다 덮어쓰는 디폴트 작동

#version 330 core

out vec4 FragColor;
in vec2 TexCoords;

uniform sampler2D readColorTexture;

void main()
{
    FragColor = texture(readColorTexture, TexCoords).rgba;
}
