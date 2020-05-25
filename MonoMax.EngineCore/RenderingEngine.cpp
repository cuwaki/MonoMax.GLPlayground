#include "RenderingEngine.h"
#include "../SuperPompoko/SPPKGame.h"

namespace SMGE
{
	namespace nsRE
	{
		////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
		////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

		ModelAsset::ModelAsset(CRenderingEngine* ge) : ge_(ge)
		{
			instances_.push_back(this);

			initShaders();
		}

		ModelAsset::~ModelAsset()
		{
			destroyBufferObjects();

			auto found = std::find(instances_.begin(), instances_.end(), this);
			instances_.erase(found);
		}

		void ModelAsset::OnScreenResize()
		{
			createBufferObject();
		}

		std::vector<ModelAsset*> ModelAsset::instances_;
		void ModelAsset::OnScreenResize_Master()
		{
			for (auto inst : instances_)
			{
				inst->OnScreenResize();
			}
		}

		void ModelAsset::initRenderData()
		{
			glGenVertexArrays(1, &m_vao);
			glGenBuffers(1, &m_vbo);

			//getShaderCode("../../../../simple_color_vs.glsl"),
			//getShaderCode("../../../../simple_color_fs.glsl")
			vertShaderPath_ = "simple_color_vs.glsl";
			fragShaderPath_ = "simple_color_fs.glsl";

			vertices_ = {
			0.0f, 0.5f, 0.0f,
			-0.5f, -0.5f, 0.0f,
			0.5f, -0.5f, 0.0f,
			};
			vertexAttribNumber_ = 3;
		}

		void ModelAsset::initShaders()
		{
			initRenderData();

			m_prg = glCreateProgram();
			ge_->addShader(m_prg, vertShaderPath_, GL_VERTEX_SHADER);
			ge_->addShader(m_prg, fragShaderPath_, GL_FRAGMENT_SHADER);
			glLinkProgram(m_prg);

			glBindVertexArray(m_vao);
			{
				glBindBuffer(GL_ARRAY_BUFFER, m_vbo);
				glBufferData(GL_ARRAY_BUFFER, vertices_.size() * sizeof(decltype(vertices_[0])), &vertices_[0], GL_STATIC_DRAW);
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
				glUniformMatrix4fv(0, 1, GL_FALSE, glm::value_ptr(worldPos));

				glEnableVertexAttribArray(0);
				glDrawArrays(GL_TRIANGLES, 0, vertexAttribNumber_);
			}
			glBindVertexArray(0);
		}

		void ModelAsset::createBufferObject()
		{
			destroyBufferObjects();

			//glGenFramebuffers(1, &m_fbo);
			//glGenRenderbuffers(1, &m_rbo);

			//glBindRenderbuffer(GL_RENDERBUFFER, m_rbo);
			//glRenderbufferStorage(GL_RENDERBUFFER, GL_BGRA, m_width, m_height);
			//glBindFramebuffer(GL_DRAW_FRAMEBUFFER, m_fbo);
			//glFramebufferRenderbuffer(GL_DRAW_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_RENDERBUFFER, m_rbo);
		}

		void ModelAsset::destroyBufferObjects()
		{
			if (m_fbo > 0)
				glDeleteFramebuffers(1, &m_fbo);
			m_fbo = -1;

			if (m_rbo > 0)
				glDeleteFramebuffers(1, &m_rbo);
			m_rbo = -1;
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

		std::string CRenderingEngine::getShaderCode(const char* filename)
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

		void CRenderingEngine::addShader(GLuint prgId, const std::string shadercode, GLenum shadertype)
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

			ModelAsset::OnScreenResize_Master();

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

			smge_game = new SMGE::SPPKGame();
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
			smge_game->EditorTick(0.01f);
		}

		void CRenderingEngine::Render(char* imgBuffer)
		{
			glClearColor(0.8f, 0.8f, 0.6f, 1.0f);
			glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
			glEnable(GL_MULTISAMPLE);

			smge_game->EditorRender(0.01f);

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
	}
}
