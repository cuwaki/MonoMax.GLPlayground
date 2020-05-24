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

	class ModelAsset
	{
	protected:
		GraphicsEngine* ge_ = nullptr;

		GLuint m_vao, m_vbo;
		GLuint m_rbo, m_fbo;
		GLuint m_prg;

		void initShaders();
		void initRenderData();
		void createBufferObject();
		void destroyBufferObjects();

	public:
		std::string vertShaderPath_;
		std::string fragShaderPath_;
		int vertexAttribNumber_;
		std::vector<float> vertices_;

		ModelAsset(GraphicsEngine* ge);
		~ModelAsset() noexcept;

		void draw(const glm::mat4& worldPos) const;
		void OnScreenResize();

	public:
		static void OnScreenResize_Master();
		static std::vector<ModelAsset*> instances_;
	};

	class ModelWorld
	{
	public:
		GraphicsEngine* ge_ = nullptr;		
		const ModelAsset& modelAsset_;

		glm::mat4 modelMat;

		ModelWorld(GraphicsEngine* ge, const ModelAsset& ma) : ge_(ge), modelAsset_(ma)
		{
			modelMat = glm::mat4(1.0f);
		}

		virtual void draw();
	};

	class TestTriangle : public ModelWorld
	{
	public:
		float rotY;

		TestTriangle(GraphicsEngine* ge, const ModelAsset& ma) : ModelWorld(ge, ma) {}

		virtual void draw() override;
	};

	class GraphicsEngine
	{
	private:
		int m_bufferLength;
		int m_width, m_height;
		int m_colorDepth = 4;	// GL_BGRA, PixelFormats::Pbgr32
		GLFWwindow* m_window = nullptr;
		char* GLRenderHandle = nullptr;
		bool isRunning = false;
		std::vector<ModelWorld*> m_modelList;

		void initWindow();

		class CGEGameBase* smge_game;

	public:
		const int GetWidth();
		const int GetHeight();
		const int GetBufferLenght();
		void Init();
		void DeInit();
		void Resize(int width, int height);
		void Stop();
		
		void Tick();
		void Render(char* imgBuffer);

		std::string getShaderCode(const char* filename);
		void addShader(GLuint prgId, const std::string shadercode, GLenum shadertype);

		void getWriteableBitmapInfo(double& outDpiX, double& outDpiY, int& outColorDepth);

		ModelWorld *AddModel(ModelWorld* model);
		void RemoveModel(ModelWorld* model);
	};
}

