#include "GraphicsEngine.h"
#include "../SuperPompoko/SPPKGame.h"

namespace MonoMaxGraphics
{
	void testSMGE()
	{
		static SPPKGame game;
		game.Tick(0.01f);
	}

	////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

	TestTriangle::TestTriangle(GraphicsEngine* ge) : Model(ge)
	{
	}

	void TestTriangle::initRenderData()
	{
		glGenVertexArrays(1, &m_vao);
		glGenBuffers(1, &m_vbo);

		vertices = {
		0.0f, 0.5f, 0.0f,
		-0.5f, -0.5f, 0.0f,
		0.5f, -0.5f, 0.0f,
		};
		vertexAttribNumber = 3;

		modelMat = glm::mat4(1.0f);
	}

	void TestTriangle::initShaders()
	{
		std::string shaders[] =
		{
			//getShaderCode("../../../../simple_color_vs.glsl"),
			//getShaderCode("../../../../simple_color_fs.glsl")
			ge_->getShaderCode("simple_color_vs.glsl"),
			ge_->getShaderCode("simple_color_fs.glsl")
		};

		m_prg = glCreateProgram();

		ge_->addShader(m_prg, shaders[0], GL_VERTEX_SHADER);
		ge_->addShader(m_prg, shaders[1], GL_FRAGMENT_SHADER);
		glLinkProgram(m_prg);

		glBindVertexArray(m_vao);
		{
			glBindBuffer(GL_ARRAY_BUFFER, m_vbo);
			glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(decltype(vertices[0])), &vertices[0], GL_STATIC_DRAW);
			glVertexAttribPointer(0, vertexAttribNumber, GL_FLOAT, GL_FALSE, 0, (void*)0);
			glBindBuffer(GL_ARRAY_BUFFER, 0);
		}
		glBindVertexArray(0);
	}

	void TestTriangle::createBufferObject()
	{
		destroyBufferObjects();

		//glGenFramebuffers(1, &m_fbo);
		//glGenRenderbuffers(1, &m_rbo);

		//glBindRenderbuffer(GL_RENDERBUFFER, m_rbo);
		//glRenderbufferStorage(GL_RENDERBUFFER, GL_BGRA, m_width, m_height);
		//glBindFramebuffer(GL_DRAW_FRAMEBUFFER, m_fbo);
		//glFramebufferRenderbuffer(GL_DRAW_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_RENDERBUFFER, m_rbo);
	}

	void TestTriangle::destroyBufferObjects()
	{
		if (m_fbo > 0)
			glDeleteFramebuffers(1, &m_fbo);
		m_fbo = -1;

		if (m_rbo > 0)
			glDeleteFramebuffers(1, &m_rbo);
		m_rbo = -1;
	}

	void TestTriangle::draw()
	{
		//modelMat = glm::rotate(glm::mat4(1.0f), rotY, glm::vec3(0.0f, 1.0f, 0.0f));

		glBindVertexArray(m_vao);
		{
			glUseProgram(m_prg);
			glUniformMatrix4fv(0, 1, GL_FALSE, glm::value_ptr(modelMat));

			glEnableVertexAttribArray(0);
			glDrawArrays(GL_TRIANGLES, 0, vertexAttribNumber);
		}
		glBindVertexArray(0);

		rotY += 0.05f;
	}

	void TestTriangle::OnAfterAdded()
	{
		initRenderData();
		initShaders();
	}

	void TestTriangle::OnBeforeRemove()
	{
		destroyBufferObjects();
	}

	void TestTriangle::OnScreenResize()
	{
		createBufferObject();
	}

	////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

	std::string GraphicsEngine::getShaderCode(const char* filename)
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

	void GraphicsEngine::addShader(GLuint prgId, const std::string shadercode, GLenum shadertype)
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

	void GraphicsEngine::Render(char* imgBuffer)
	{
		glClearColor(0.8f, 0.8f, 0.6f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		glEnable(GL_MULTISAMPLE);

		testSMGE();
		for (auto model : m_modelList)
		{
			model->draw();
		}

		if(imgBuffer != nullptr)
			glReadPixels(0, 0, m_width, m_height, GL_BGRA, GL_UNSIGNED_BYTE, imgBuffer);

		glfwSwapBuffers(m_window);
		glfwPollEvents();
	}

	const int GraphicsEngine::GetHeight() { return m_height; }
	const int GraphicsEngine::GetWidth() { return m_width; }

	void GraphicsEngine::initWindow()
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

	void GraphicsEngine::Resize(int width, int height)
	{
		glfwSetWindowSize(m_window, width, height);

		m_width = width;
		m_height = height;
		m_bufferLength = width * height * m_colorDepth;

		free(GLRenderHandle);
		GLRenderHandle = (char*)malloc(m_bufferLength);

		for (auto model : m_modelList)
		{
			model->OnScreenResize();
		}

		glViewport(0, 0, width, height);
	}

	const int GraphicsEngine::GetBufferLenght()
	{
		return m_bufferLength;
	}

	void GraphicsEngine::Init()
	{
		if(m_window != nullptr)
			throw std::exception("it is trying reinit");

		initWindow();

		// 테스트 코드
		AddModel(new TestTriangle(this));
		
		auto added = DCast<TestTriangle*>( AddModel(new TestTriangle(this)) );
		//added->modelMat = glm::translate(added->modelMat, { 0.5, 0.5, 0 });
		auto& x = added->modelMat[3][0];
		auto& y = added->modelMat[3][1];
		auto& z = added->modelMat[3][2];

		x = 0.5;
		y = 0.5;
		//z = 0.5;
	}

	void GraphicsEngine::DeInit()
	{
		for (auto model : m_modelList)
		{
			RemoveModel(model);
			delete model;
		}
	}

	void GraphicsEngine::Stop()
	{
		isRunning = false;
	}

	Model *GraphicsEngine::AddModel(Model* model)
	{
		if (model == nullptr)
			return nullptr;

		m_modelList.push_back(model);

		model->OnAfterAdded();
		return model;
	}

	void GraphicsEngine::RemoveModel(Model* model)
	{
		if (model == nullptr)
			return;

		model->OnBeforeRemove();

		auto found = std::find(m_modelList.begin(), m_modelList.end(), model);
		m_modelList.erase(found);
	}

	void GraphicsEngine::getWriteableBitmapInfo(double& outDpiX, double& outDpiY, int& outColorDepth)
	{
		outDpiX = 96;
		outDpiY = 96;
		outColorDepth = m_colorDepth;	// for PixelFormats::Pbgr32
	}
}
