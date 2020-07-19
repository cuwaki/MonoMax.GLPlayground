#pragma once
#include "common.h"
#include <iostream>
#include <ostream>
#include <fstream>
#include <functional>
#include <vector>
#include "../SMGE/GECommonIncludes.h"
#include "../SMGE/GEContainers.h"

namespace SMGE
{
	namespace nsGE
	{
		class CGameBase;
	}

	namespace nsRE
	{
		class CRenderingEngine;

		class ModelAsset
		{
		protected:
			CRenderingEngine* re_ = nullptr;

			GLuint m_vao = -1, m_vbo = -1;
			GLuint m_rbo = -1, m_fbo = -1;
			GLuint m_prg = -1;

			bool m_isInitialized();
			void initRenderData();
			void createBufferObject(int width, int height);
			void destroyBufferObjects();

		public:
			CWString vertShaderPath_;
			CWString fragShaderPath_;
			int vertexAttribNumber_ = 0;
			CVector<float> vertices_;

			ModelAsset();
			ModelAsset(CRenderingEngine* re);
			~ModelAsset() noexcept;

			void initShaders();
			void drawGL(const glm::mat4& worldPos) const;
			void OnScreenResize(int width, int height);

		public:
			static void OnScreenResize_Master(int width, int height);
			static CVector<ModelAsset*> instances_;
		};

		class WorldModel
		{
		protected:
			CRenderingEngine* re_ = nullptr;
			const ModelAsset& modelAsset_;

		public:
			glm::mat4 modelMat;

			WorldModel(const ModelAsset& ma) : modelAsset_(ma)
			{
				modelMat = glm::mat4(1.0f);
			}

			void SetRenderingEngine(CRenderingEngine* re)
			{
				re_ = re;
			}

			virtual void draw();
		};

		class TestTriangle : public WorldModel
		{
		public:
			float rotY = 0;

			TestTriangle(const ModelAsset& ma) : WorldModel(ma) {}

			virtual void draw() override;
		};

		namespace GLUtil
		{
			std::string getShaderCode(const char* filename);
			void addShader(GLuint prgId, const std::string shadercode, GLenum shadertype);
			void safeDeleteVertexArray(GLuint& vao);
			void safeDeleteBuffer(GLuint& vbo);
			void safeDeleteProgram(GLuint& prog);
			void safeDeleteFramebuffer(GLuint& fbo);
		}

		class CRenderingEngine
		{
		private:
			int m_bufferLengthW, m_bufferLengthF;	// window 기반 버퍼 크기와 frame 기반 버퍼 크기
			int m_width, m_height;
			int m_framebufferWith, m_framebufferHeight;
			int m_colorDepth = 4;	// GL_BGRA, PixelFormats::Pbgr32
			glm::vec4 m_clearColor;
			GLFWwindow* m_window = nullptr;
			char* GLRenderHandle = nullptr;
			bool isRunning = false;
			CVector<WorldModel*> m_worldModelList;

			void initWindow();

			class nsGE::CGameBase* smge_game;

		public:
			CRenderingEngine();
			virtual ~CRenderingEngine();

			const int GetWidth();
			const int GetHeight();
			const int GetBufferLenght();
			void Init();
			void DeInit();
			void Resize(int width, int height);
			void Stop();

			void Tick();
			void Render(char* imgBuffer);

			void getWriteableBitmapInfo(double& outDpiX, double& outDpiY, int& outColorDepth);

			WorldModel* AddWorldModel(WorldModel* model);
			void RemoveWorldModel(WorldModel* model);
		};
	}
}
