#pragma once
#include "common.h"
#include <iostream>
#include <ostream>
#include <fstream>
#include <functional>
#include <vector>

namespace MonoMaxGraphics
{
	class GraphicsEngine;

	class Model
	{
	public:
		GraphicsEngine* ge_ = nullptr;

		Model(GraphicsEngine* ge) : ge_(ge)
		{
		}

		virtual void draw() = 0;
		virtual void OnAfterAdded() = 0;
		virtual void OnBeforeRemove() = 0;
		virtual void OnScreenResize() = 0;
	};

	class TestTriangle : public Model
	{
	public:
		GLuint m_vao, m_vbo;
		GLuint m_rbo, m_fbo;
		GLuint m_prg;

		glm::mat4 modelMat;
		float rotY;
		int vertexAttribNumber = 3;
		std::vector<float> vertices;

		TestTriangle(GraphicsEngine* ge);

		void initShaders();
		void initRenderData();
		void createBufferObject();
		void destroyBufferObjects();

		virtual void draw() override;
		virtual void OnAfterAdded() override;
		virtual void OnBeforeRemove() override;
		virtual void OnScreenResize() override;
	};

	class GraphicsEngine
	{
	private:
		int m_bufferLength;
		int m_width, m_height;
		int m_colorDepth = 4;	// 32bit color
		GLFWwindow* m_window = nullptr;
		char* GLRenderHandle = nullptr;
		bool isRunning = false;

		void initWindow();
		std::function<void()> m_rendercallback;

		std::vector<Model*> m_modelList;

	public:
		const int GetWidth();
		const int GetHeight();
		const int GetBufferLenght();
		void Init();
		void DeInit();
		void Resize(int width, int height);
		void Stop();
		void Render(char* imgBuffer);

		std::string getShaderCode(const char* filename);
		void addShader(GLuint prgId, const std::string shadercode, GLenum shadertype);

		void getWriteableBitmapInfo(double& outDpiX, double& outDpiY, int& outColorDepth);

		Model *AddModel(Model* model);
		void RemoveModel(Model* model);
	};
}

