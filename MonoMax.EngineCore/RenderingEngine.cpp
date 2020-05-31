#include "RenderingEngine.h"
#include "../SuperPompoko/SPPKGame.h"
#include "../SMGE/CEngineBase.h"

namespace SMGE
{
	namespace nsRE
	{
		////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
		////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

		CVector<ModelAsset*> ModelAsset::instances_;

		ModelAsset::ModelAsset()
		{
			instances_.push_back(this);
		}
		
		ModelAsset::ModelAsset(CRenderingEngine* re) : ModelAsset()
		{
			re_ = re;
		}

		ModelAsset::~ModelAsset()
		{
			destroyBufferObjects();

			auto found = std::find(instances_.begin(), instances_.end(), this);
			instances_.erase(found);
		}

		void ModelAsset::OnScreenResize(int width, int height)
		{
			createBufferObject(width, height);
		}

		void ModelAsset::OnScreenResize_Master(int width, int height)
		{
			for (auto inst : instances_)
			{
				inst->OnScreenResize(width, height);
			}
		}

		bool ModelAsset::m_isInitialized()
		{
			return (m_vao != -1);
		}

		void ModelAsset::initRenderData()
		{
			if (m_isInitialized())
				return;

			glGenVertexArrays(1, &m_vao);
			glGenBuffers(1, &m_vbo);

			// 테스트 코드
			////getShaderCode("../../../../simple_color_vs.glsl"),
			////getShaderCode("../../../../simple_color_fs.glsl")
			//vertShaderPath_ = "simple_color_vs.glsl";
			//fragShaderPath_ = "simple_color_fs.glsl";

			//vertices_ = {
			//0.0f, 0.5f, 0.0f,
			//-0.5f, -0.5f, 0.0f,
			//0.5f, -0.5f, 0.0f,
			//};
			//vertexAttribNumber_ = 3;
		}

		void ModelAsset::initShaders()
		{
			if (m_isInitialized())
				return;

			initRenderData();

			m_prg = glCreateProgram();
			GLUtil::addShader(m_prg, GLUtil::getShaderCode(ToASCII(vertShaderPath_).c_str()), GL_VERTEX_SHADER);
			GLUtil::addShader(m_prg, GLUtil::getShaderCode(ToASCII(fragShaderPath_).c_str()), GL_FRAGMENT_SHADER);
			glLinkProgram(m_prg);

			glBindVertexArray(m_vao);
			{
				auto dataSize = vertices_.size() * sizeof(decltype(vertices_[0]));

				glBindBuffer(GL_ARRAY_BUFFER, m_vbo);
				glBufferData(GL_ARRAY_BUFFER, dataSize, &vertices_[0], GL_STATIC_DRAW);
				glVertexAttribPointer(0, vertexAttribNumber_, GL_FLOAT, GL_FALSE, 0, (void*)0);
				glBindBuffer(GL_ARRAY_BUFFER, 0);
			}
			glBindVertexArray(0);
		}

		void ModelAsset::drawGL(const glm::mat4& worldPos) const
		{
			glBindVertexArray(m_vao);
			{
				glUseProgram(m_prg);
				glUniformMatrix4fv(0, 1, GL_FALSE, &worldPos[0][0]);
				//glUniformMatrix4fv(0, 1, GL_FALSE, glm::value_ptr(worldPos));

				glEnableVertexAttribArray(0);
				glDrawArrays(GL_TRIANGLES, 0, vertexAttribNumber_);
			}
			glBindVertexArray(0);
		}

		void ModelAsset::createBufferObject(int width, int height)
		{
			//destroyBufferObjects();

			//glGenFramebuffers(1, &m_fbo);
			//glGenRenderbuffers(1, &m_rbo);

			//glBindRenderbuffer(GL_RENDERBUFFER, m_rbo);
			//glRenderbufferStorage(GL_RENDERBUFFER, GL_BGRA, width, height);
			//glBindFramebuffer(GL_DRAW_FRAMEBUFFER, m_fbo);
			//glFramebufferRenderbuffer(GL_DRAW_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_RENDERBUFFER, m_rbo);
		}

		void ModelAsset::destroyBufferObjects()
		{
			GLUtil::safeDeleteVertexArray(m_vao);
			GLUtil::safeDeleteBuffer(m_vbo);
			GLUtil::safeDeleteProgram(m_prg);
			GLUtil::safeDeleteFramebuffer(m_fbo);
			GLUtil::safeDeleteFramebuffer(m_rbo);
		}

		void WorldModel::draw()
		{
			modelAsset_.drawGL(modelMat);
		}

		void TestTriangle::draw()
		{
			//modelMat = glm::rotate(glm::mat4(1.0f), rotY, glm::vec3(0.0f, 1.0f, 0.0f));

			WorldModel::draw();

			//rotY += 0.05f;
		}
		////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
		////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

		CRenderingEngine::CRenderingEngine()
		{
		}
		
		CRenderingEngine::~CRenderingEngine()
		{
		}

		const int CRenderingEngine::GetHeight() { return m_height; }
		const int CRenderingEngine::GetWidth() { return m_width; }

		void CRenderingEngine::initWindow()
		{
			if (!glfwInit())
				throw std::exception();

			glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
			glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
			glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
			glfwWindowHint(GLFW_VISIBLE, GLFW_FALSE);

			//glfwWindowHint(GLFW_SAMPLES, 4);

			// 윈도우 독립 시키기
			m_window = glfwCreateWindow(640, 480, "Hidden OpenGL window", NULL, NULL);

			if (!m_window)
			{
				glfwTerminate();
				throw std::exception();
			}

			glfwMakeContextCurrent(m_window);
			if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
			{
				glfwTerminate();
				throw std::exception();
			}
		}

		void CRenderingEngine::Resize(int width, int height)
		{
			glfwSetWindowSize(m_window, width, height);

			m_width = width;
			m_height = height;
			m_bufferLength = width * height * m_colorDepth;

			free(GLRenderHandle);
			GLRenderHandle = (char*)malloc(m_bufferLength);

			ModelAsset::OnScreenResize_Master(m_width, m_height);

			glViewport(0, 0, width, height);
		}

		const int CRenderingEngine::GetBufferLenght()
		{
			return m_bufferLength;
		}

		void CRenderingEngine::Init()
		{
			if (m_window != nullptr)
				throw std::exception("it is trying reinit");

			initWindow();

			// 테스트 코드
			//static ModelAsset testTriangle(this);
			//AddWorldModel(new TestTriangle(this, testTriangle));
			//auto added = DCast<TestTriangle*>( AddWorldModel(new TestTriangle(this, testTriangle)) );
			//added->modelMat = glm::translate(added->modelMat, { 0.5, 0.5, 0 });

			// 테스트 코드
			//auto& x = added->modelMat[3][0];
			//auto& y = added->modelMat[3][1];
			//auto& z = added->modelMat[3][2];
			//x = 0.5;
			//y = 0.5;
			//z = 0.5;

			smge_game = new SMGE::SPPKGame(nullptr);
			smge_game->GetEngine()->SetRenderingEngine(this);
		}

		void CRenderingEngine::DeInit()
		{
			for (auto model : m_worldModelList)
			{
				RemoveWorldModel(model);
				delete model;
			}

			delete smge_game;
		}

		void CRenderingEngine::Tick()
		{
			smge_game->GetEngine()->Tick(0.01f);
		}

		void CRenderingEngine::Render(char* imgBuffer)
		{
			glClearColor(0.8f, 0.8f, 0.6f, 1.0f);
			glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
			glEnable(GL_MULTISAMPLE);

			smge_game->GetEngine()->Render(0.01f);

			for (auto model : m_worldModelList)
			{
				model->draw();
			}

			if (imgBuffer != nullptr)
				glReadPixels(0, 0, m_width, m_height, GL_BGRA, GL_UNSIGNED_BYTE, imgBuffer);	// m_colorDepth, PixelFormats::Pbgr32

			glfwSwapBuffers(m_window);
			glfwPollEvents();
		}


		void CRenderingEngine::Stop()
		{
			isRunning = false;
		}

		WorldModel* CRenderingEngine::AddWorldModel(WorldModel* model)
		{
			if (model == nullptr)
				return nullptr;

			m_worldModelList.push_back(model);
			
			model->SetRenderingEngine(this);
			return model;
		}

		void CRenderingEngine::RemoveWorldModel(WorldModel* model)
		{
			if (model == nullptr)
				return;

			auto found = std::find(m_worldModelList.begin(), m_worldModelList.end(), model);
			m_worldModelList.erase(found);
		}

		void CRenderingEngine::getWriteableBitmapInfo(double& outDpiX, double& outDpiY, int& outColorDepth)
		{
			outDpiX = 96;
			outDpiY = 96;
			outColorDepth = m_colorDepth;	// 4 for PixelFormats::Pbgr32
		}

		////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
		////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

		namespace GLUtil
		{
			std::string getShaderCode(const char* filename)
			{
				std::string shaderCode;
				std::ifstream file(filename, std::ios::in);

				if (!file.good())
				{
					throw new std::exception();
				}

				file.seekg(0, std::ios::end);
				shaderCode.resize((unsigned int)file.tellg());
				file.seekg(0, std::ios::beg);
				file.read(&shaderCode[0], shaderCode.size());
				file.close();

				return shaderCode;
			}

			void addShader(GLuint prgId, const std::string shadercode, GLenum shadertype)
			{
				if (prgId < 0)
					throw new std::exception();

				GLuint id = glCreateShader(shadertype);

				if (id < 0)
					throw new std::exception();

				const char* code = shadercode.c_str();

				glShaderSource(id, 1, &code, NULL);
				glCompileShader(id);
				glAttachShader(prgId, id);
				glDeleteShader(id);
			}

			void safeDeleteVertexArray(GLuint& vao)
			{
				if (vao > 0)
					glDeleteVertexArrays(1, &vao);
				vao = -1;
			}

			void safeDeleteBuffer(GLuint& vbo)
			{
				if (vbo > 0)
					glDeleteBuffers(1, &vbo);
				vbo = -1;
			}

			void safeDeleteProgram(GLuint& prog)
			{
				if (prog > 0)
					glDeleteProgram(prog);
				prog = -1;
			}

			void safeDeleteFramebuffer(GLuint& fbo)
			{
				if (fbo > 0)
					glDeleteFramebuffers(1, &fbo);
				fbo = -1;
			}
		}
	}
}
