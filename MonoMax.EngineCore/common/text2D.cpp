#include "../common.h"

#include <vector>
#include <cstring>

//#include <GL/glew.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
using namespace glm;

#include "shader.hpp"
#include "texture.hpp"

#include "text2D.hpp"

unsigned int Text2DTextureID;
unsigned int Text2DVertexBufferID;
unsigned int Text2DUVBufferID;
unsigned int Text2DShaderID;
unsigned int Text2DUniformID;

int TextScreen_HalfWidth;
int TextScreen_HalfHeight;

void initText2D(const wchar_t* texturePath)
{
	// Initialize texture
	//Text2DTextureID = loadDDS(texturePath);	// 차후 정상화해라

	// Initialize VBO
	glGenBuffers(1, &Text2DVertexBufferID);
	glGenBuffers(1, &Text2DUVBufferID);

	// Initialize Shader
	Text2DShaderID = LoadShaders(L"TextVertexShader.vertexshader", L"TextVertexShader.fragmentshader");

	// Initialize uniforms' IDs
	Text2DUniformID = glGetUniformLocation(Text2DShaderID, "myTextureSampler");

	TextScreen_HalfWidth = glGetUniformLocation(Text2DShaderID, "Screen_HalfWidth");
	TextScreen_HalfHeight = glGetUniformLocation(Text2DShaderID, "Screen_HalfHeight");
}

void printText2D(const wchar_t* text, int x, int y, int fontSize)
{
	size_t length = wcslen(text);

	// Fill buffers
	std::vector<glm::vec2> vertices;
	std::vector<glm::vec2> UVs;
	for (unsigned int i = 0; i < length; i++)
	{
		glm::vec2 quad_up_left = glm::vec2(x + i * fontSize, y + fontSize);
		glm::vec2 quad_up_right = glm::vec2(x + i * fontSize + fontSize, y + fontSize);
		glm::vec2 quad_down_right = glm::vec2(x + i * fontSize + fontSize, y);
		glm::vec2 quad_down_left = glm::vec2(x + i * fontSize, y);

		// ccw로 정의한다 - 삼각형 1
		vertices.push_back(quad_up_left);
		vertices.push_back(quad_down_left);
		vertices.push_back(quad_up_right);

		// ccw로 정의한다 - 삼각형 2
		vertices.push_back(quad_down_right);
		vertices.push_back(quad_up_right);
		vertices.push_back(quad_down_left);

		wchar_t character = text[i];

		int charXIndex = (character % 16),	// 0 <= 15
			charYIndex = (character / 16);	// 0 <= 15.9 니까 15

		float uv_x = charXIndex / 16.0f;	// 0 ~ 0.9~ // 16 == 폰트가 텍스처상에 가로로 나열된 개수
		float uv_y = charYIndex / 16.0f;	// 0 ~ 0.9~ // 16 == 폰트가 텍스처상에 세로로 나열된 개수

		glm::vec2 uv_up_left = glm::vec2(uv_x, uv_y);
		glm::vec2 uv_up_right = glm::vec2(uv_x + 1.0f / 16.0f, uv_y);
		glm::vec2 uv_down_right = glm::vec2(uv_x + 1.0f / 16.0f, (uv_y + 1.0f / 16.0f));
		glm::vec2 uv_down_left = glm::vec2(uv_x, (uv_y + 1.0f / 16.0f));
		UVs.push_back(uv_up_left);
		UVs.push_back(uv_down_left);
		UVs.push_back(uv_up_right);

		UVs.push_back(uv_down_right);
		UVs.push_back(uv_up_right);
		UVs.push_back(uv_down_left);
	}

	glBindBuffer(GL_ARRAY_BUFFER, Text2DVertexBufferID);
	glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(glm::vec2), &vertices[0], GL_STATIC_DRAW);
	glBindBuffer(GL_ARRAY_BUFFER, Text2DUVBufferID);
	glBufferData(GL_ARRAY_BUFFER, UVs.size() * sizeof(glm::vec2), &UVs[0], GL_STATIC_DRAW);

	// Bind shader
	glUseProgram(Text2DShaderID);

	// Bind texture
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, Text2DTextureID);
	// Set our "myTextureSampler" sampler to user Texture Unit 0
	glUniform1i(Text2DUniformID, 0);

	glUniform1i(TextScreen_HalfWidth, 1024 / 2);
	glUniform1i(TextScreen_HalfHeight, 768 / 2);

	// 1rst attribute buffer : vertices
	glEnableVertexAttribArray(0);
	glBindBuffer(GL_ARRAY_BUFFER, Text2DVertexBufferID);
	glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 0, (void*)0);

	// 2nd attribute buffer : UVs
	glEnableVertexAttribArray(1);
	glBindBuffer(GL_ARRAY_BUFFER, Text2DUVBufferID);
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 0, (void*)0);

	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	// Draw call
	glDrawArrays(GL_TRIANGLES, 0, vertices.size());

	glDisable(GL_BLEND);

	glDisableVertexAttribArray(0);
	glDisableVertexAttribArray(1);

}

void cleanupText2D() {

	// Delete buffers
	glDeleteBuffers(1, &Text2DVertexBufferID);
	glDeleteBuffers(1, &Text2DUVBufferID);

	// Delete texture
	glDeleteTextures(1, &Text2DTextureID);

	// Delete shader
	glDeleteProgram(Text2DShaderID);
}
