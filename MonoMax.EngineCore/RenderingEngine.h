#pragma once
#include "common.h"
#include <iostream>
#include <ostream>
#include <fstream>
#include <functional>
#include <vector>

namespace SMGE
{
	namespace nsGE
	{
		class CGEGameBase;
	}

	namespace nsRE
	{
		class CRenderingEngine;

		class ModelAsset
		{
		protected:
			CRenderingEngine* ge_ = nullptr;

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

			ModelAsset(CRenderingEngine* ge);
			~ModelAsset() noexcept;

			void drawGL(const glm::mat4& worldPos) const;
			void OnScreenResize();

		public:
			static void OnScreenResize_Master();
			static std::vector<ModelAsset*> instances_;
		};

		class WorldModel
		{
		public:
			CRenderingEngine* ge_ = nullptr;
			const ModelAsset& modelAsset_;

			glm::mat4 modelMat;

			WorldModel(CRenderingEngine* ge, const ModelAsset& ma) : ge_(ge), modelAsset_(ma)
			{
				modelMat = glm::mat4(1.0f);
			}

			virtual void draw();
		};

		class TestTriangle : public WorldModel
		{
		public:
			float rotY;

			TestTriangle(CRenderingEngine* ge, const ModelAsset& ma) : WorldModel(ge, ma) {}

			virtual void draw() override;
		};

		class CRenderingEngine
		{
		private:
			int m_bufferLength;
			int m_width, m_height;
			int m_colorDepth = 4;	// GL_BGRA, PixelFormats::Pbgr32
			GLFWwindow* m_window = nullptr;
			char* GLRenderHandle = nullptr;
			bool isRunning = false;
			std::vector<WorldModel*> m_worldModelList;

			void initWindow();

			class nsGE::CGEGameBase* smge_game;

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

			WorldModel* AddWorldModel(WorldModel* model);
			void RemoveWorldModel(WorldModel* model);
		};
	}
}

