#include "RenderingEngine.h"
#include "../SuperPompoko/SPPKGame.h"
#include "../SMGE/CEngineBase.h"

#include "common/objloader.hpp"
#include "common/shader.hpp"
#include "common/texture.hpp"
#include "common/controls.hpp"

namespace SMGE
{
	namespace nsRE
	{
		using namespace TransformConst;

		VertFragShaderSet::VertFragShaderSet(const CWString& vertShadPath, const CWString& fragShadPath)
		{
			Invalidate();

			programID_ = LoadShaders(vertShadPath.c_str(), fragShadPath.c_str());

			// 모든 셰이더가 공통으로 가져야할 것
			unif_MVPMatrixID_ = glGetUniformLocation(programID_, "MVP");
			unif_ViewMatrixID_ = glGetUniformLocation(programID_, "V");
			unif_ModelMatrixID_ = glGetUniformLocation(programID_, "M");
			unif_LightWorldPosition_ = glGetUniformLocation(programID_, "LightPosition_worldspace");

			unif_TextureSampleI_ = glGetUniformLocation(programID_, "myTextureSampler");
		}

		VertFragShaderSet::~VertFragShaderSet()
		{
			Destroy();
		}

		void VertFragShaderSet::Destroy()
		{
			if (programID_ != 0)
			{
				glDeleteProgram(programID_);
				programID_ = 0;
			}
		}

		void VertFragShaderSet::Invalidate()
		{
			programID_ = 0;
			unif_MVPMatrixID_ = -1;	// glGetUniform 계열에서 에러나면 -1 이 들어가길래 이렇게 한다
			unif_ViewMatrixID_ = -1;
			unif_ModelMatrixID_ = -1;
			unif_TextureSampleI_ = -1;
			unif_LightWorldPosition_ = -1;
		}

		VertFragShaderSet::VertFragShaderSet(VertFragShaderSet&& c) noexcept
		{
			operator=(std::move(c));
		}

		VertFragShaderSet& VertFragShaderSet::operator=(VertFragShaderSet&& c) noexcept
		{
			programID_ = c.programID_;
			unif_MVPMatrixID_ = c.unif_MVPMatrixID_;
			unif_ViewMatrixID_ = c.unif_ViewMatrixID_;
			unif_ModelMatrixID_ = c.unif_ModelMatrixID_;
			unif_TextureSampleI_ = c.unif_TextureSampleI_;
			unif_LightWorldPosition_ = c.unif_LightWorldPosition_;

			c.Invalidate();

			return *this;
		}

		TextureDDS::TextureDDS(const CWString& texPath)
		{
			Invalidate();

			if (texPath.length() > 0)
				textureID_ = loadDDS(texPath.c_str());
		}

		TextureDDS::~TextureDDS()
		{
			Destroy();
		}

		void TextureDDS::Destroy()
		{
			if (textureID_ != 0)
			{
				glDeleteTextures(1, &textureID_);
				textureID_ = 0;
			}
		}

		void TextureDDS::Invalidate()
		{
			textureID_ = 0;
		}

		TextureDDS::TextureDDS(TextureDDS&& c) noexcept
		{
			operator=(std::move(c));
		}

		TextureDDS& TextureDDS::operator=(TextureDDS&& c) noexcept
		{
			textureID_ = c.textureID_;
			c.Invalidate();

			return *this;
		}

		MeshOBJ::MeshOBJ(const CWString& objPath)
		{
			Invalidate();
			loadFromOBJFile(objPath);
		}

		bool MeshOBJ::loadFromOBJFile(const CWString& objPath)
		{
			bool ret = false;

			if (objPath.length() > 0)
			{
				ret = loadOBJ(objPath.c_str(), vertices_, uvs_, normals_);

				vertexColors_.resize(vertices_.size());	// 일단 버컬은 000 으로 채워놓자
			}

			return ret;
		}

		bool MeshOBJ::loadFromPlainData(const std::vector<glm::vec3>& vertices, const std::vector<glm::vec2>& uvs, const std::vector<glm::vec3>& normals)
		{
			if (vertices.size() == 0)
				return false;

			vertices_ = vertices;
			uvs_ = uvs;
			normals_ = normals;

			vertexColors_.resize(vertices.size());	// 일단 버컬은 000 으로 채워놓자

			assert(vertices_.size() == uvs_.size());
			assert(uvs_.size() == normals_.size());
			assert(normals_.size() == vertexColors_.size());

			return true;
		}

		bool MeshOBJ::setVertexColors(const std::vector<glm::vec3>& vertexColors)
		{
			if (vertexColors.size() == 0 || vertices_.size() != vertexColors.size())
				return false;

			vertexColors_ = vertexColors;
			return true;
		}

		void MeshOBJ::Destroy()
		{
			Invalidate();
		}

		void MeshOBJ::Invalidate()
		{
			vertices_.clear();
			uvs_.clear();
			normals_.clear();
			vertexColors_.clear();
		}

		MeshOBJ::MeshOBJ(MeshOBJ&& c) noexcept
		{
			operator=(std::move(c));
		}

		MeshOBJ& MeshOBJ::operator=(MeshOBJ&& c) noexcept
		{
			vertices_ = std::move(c.vertices_);
			uvs_ = std::move(c.uvs_);
			normals_ = std::move(c.normals_);
			vertexColors_ = std::move(c.vertexColors_);
			c.Invalidate();

			return *this;
		}

		AssetModel::AssetModel(const CWString& textureFilePath, const CWString& vertShadPath, const CWString& fragShadPath, const CWString& objPath)
		{
			texture_.TextureDDS::TextureDDS(textureFilePath);
			vfShaderSet_.VertFragShaderSet::VertFragShaderSet(vertShadPath, fragShadPath);
			mesh_.MeshOBJ::MeshOBJ(objPath);
		}
		AssetModel::AssetModel(AssetModel&& c)
		{
			operator=(std::move(c));
		}

		AssetModel& AssetModel::operator=(AssetModel&& c)
		{
			renderingModel_ = c.renderingModel_;
			c.Invalidate();

			texture_ = std::move(c.texture_);
			vfShaderSet_ = std::move(c.vfShaderSet_);
			mesh_ = std::move(c.mesh_);

			return *this;
		}

		void AssetModel::Invalidate()
		{
			renderingModel_ = nullptr;
		}

		void AssetModel::Destroy()
		{
			if (renderingModel_ != nullptr)
			{
				delete renderingModel_;
				renderingModel_ = nullptr;
			}
		}

		void AssetModel::ReadyToRender()
		{
			if (renderingModel_ != nullptr)
				return;

			renderingModel_ = new RenderingModel(*this, 0);
		}

		class RenderingModel& AssetModel::GetRenderingModel() const
		{
			return *renderingModel_;
		}


		Transform::Transform()
		{
			translation_ = Vec3_Zero;
			rotationDegree_ = Vec3_Zero;
			scale_ = Vec3_One;

			Dirty();
			RecalcMatrix();
		}

		const glm::vec3& Transform::GetTranslation() const
		{
			return translation_;
		}

		const glm::vec3& Transform::GetRotation() const
		{
			return rotationDegree_;
		}

		const glm::vec3& Transform::GetScale() const
		{
			return scale_;
		}

		const glm::mat4& Transform::GetTransform(bool forceRecalc)
		{
			if(forceRecalc)
				RecalcMatrix();

			return currentTransform_;
		}

		void Transform::Translate(glm::vec3 worldPos)
		{
			translation_ = worldPos;

			Dirty();
		}

		void Transform::Rotate(glm::vec3 rotateDegrees)
		{
			rotationDegree_ = rotateDegrees;

			Dirty();
		}
		void Transform::RotateAxis(ETypeRot rType, float degrees)
		{
			assert(rType >= 0 && rType < ETypeRot::ETypeRot_MAX);
			rotationDegree_[rType] = degrees;

			Dirty();
		}

		void Transform::Scale(float scale)
		{
			scale_ = glm::vec3(scale);

			Dirty();
		}
		void Transform::Scale(glm::vec3 scale)
		{
			scale_ = scale;

			Dirty();
		}
		void Transform::ScaleAxis(ETypeAxis aType, float scale)
		{
			assert(aType >= 0 && aType < ETypeAxis::ETypeAxis_MAX);
			scale_[aType] = scale;

			Dirty();
		}

		void Transform::OnBeforeRendering()
		{
			RecalcMatrix();
		}

		void Transform::RecalcMatrix()
		{
			if (isDirty_ == false)
				return;

			currentTransform_ = glm::translate(Mat4_Identity, translation_);

			currentTransform_ = glm::rotate(currentTransform_, glm::radians(rotationDegree_[ETypeRot::PITCH]), WorldAxis[ETypeRot::PITCH]);
			currentTransform_ = glm::rotate(currentTransform_, glm::radians(rotationDegree_[ETypeRot::YAW]), WorldAxis[ETypeRot::YAW]);
			currentTransform_ = glm::rotate(currentTransform_, glm::radians(rotationDegree_[ETypeRot::ROLL]), WorldAxis[ETypeRot::ROLL]);

			currentTransform_ = glm::scale(currentTransform_, scale_);

			isDirty_ = false;
		}

		void RenderingModel::AddWorldModel(WorldModel* wm) const
		{
			ptrWorldModels_.push_back(wm);

			assert(wm->renderingModel_ == nullptr);	// 이렇지 않으면 이전 거에서 빼주는 처리가 필요하다

			if (wm->renderingModel_ == nullptr)
				wm->renderingModel_ = this;
		}
		void RenderingModel::RemoveWorldModel(WorldModel* wm) const
		{
			auto it = std::find(ptrWorldModels_.begin(), ptrWorldModels_.end(), wm);
			if (it != ptrWorldModels_.end())
				ptrWorldModels_.erase(it);
		}
		const std::vector<WorldModel*>& RenderingModel::WorldModels() const
		{
			return ptrWorldModels_;
		}

		RenderingModel::~RenderingModel()
		{
			Destroy();
		}

		RenderingModel::RenderingModel(const AssetModel& asset, GLuint texSamp) : asset_(asset)
		{
			Invalidate();

			usingTextureID_ = asset_.texture_.textureID_;
			usingTextureSampleI_ = texSamp;

			// 일단 공통 셰이더 용으로 하드코딩
			vertAttrArray_ = 0;
			uvAttrArray_ = 1;
			normAttrArray_ = 2;
			vertexColorAttrArray_ = 3;

			GenBindData(asset_.mesh_.vertices_, asset_.mesh_.uvs_, asset_.mesh_.normals_, asset_.mesh_.vertexColors_);
		}

		RenderingModel::RenderingModel(RenderingModel&& c) noexcept : asset_(c.asset_)
		{
			verticesSize_ = c.verticesSize_;

			vao_ = c.vao_;
			vertexBuffer_ = c.vertexBuffer_;
			uvBuffer_ = c.uvBuffer_;
			normalBuffer_ = c.normalBuffer_;
			vertexColorBuffer_ = c.vertexColorBuffer_;
			glDrawType_ = c.glDrawType_;

			usingTextureID_ = c.usingTextureID_;
			usingTextureSampleI_ = c.usingTextureSampleI_;
			vertAttrArray_ = c.vertAttrArray_, uvAttrArray_ = c.uvAttrArray_, normAttrArray_ = c.normAttrArray_, vertexColorAttrArray_ = c.vertexColorAttrArray_;

			c.Invalidate();
		}

		GLuint RenderingModel::GetUsingProgram()
		{
			return asset_.vfShaderSet_.programID_;
		}

		bool RenderingModel::GenBindData(const std::vector<glm::vec3>& vertices, const std::vector<glm::vec2>& uvs, const std::vector<glm::vec3>& normals, const std::vector<glm::vec3>& vertexColors)
		{
			if (vertices.size() == 0)
				return false;

			verticesSize_ = vertices.size();

			glGenVertexArrays(1, &vao_);
			glBindVertexArray(vao_);

			glGenBuffers(1, &vertexBuffer_);
			glBindBuffer(GL_ARRAY_BUFFER, vertexBuffer_);
			glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(glm::vec3), &vertices[0], glDrawType_);

			glGenBuffers(1, &uvBuffer_);
			glBindBuffer(GL_ARRAY_BUFFER, uvBuffer_);
			glBufferData(GL_ARRAY_BUFFER, uvs.size() * sizeof(glm::vec2), &uvs[0], glDrawType_);

			glGenBuffers(1, &normalBuffer_);
			glBindBuffer(GL_ARRAY_BUFFER, normalBuffer_);
			glBufferData(GL_ARRAY_BUFFER, normals.size() * sizeof(glm::vec3), &normals[0], glDrawType_);

			glGenBuffers(1, &vertexColorBuffer_);
			glBindBuffer(GL_ARRAY_BUFFER, vertexColorBuffer_);
			glBufferData(GL_ARRAY_BUFFER, vertexColors.size() * sizeof(glm::vec3), &vertexColors[0], glDrawType_);

			glBindVertexArray(0);

			return true;
		}

		void RenderingModel::Destroy()
		{
			if (vao_ != 0)
			{
				glDeleteBuffers(1, &vertexBuffer_);
				glDeleteBuffers(1, &uvBuffer_);
				glDeleteBuffers(1, &normalBuffer_);
				glDeleteBuffers(1, &vertexColorBuffer_);
				glDeleteVertexArrays(1, &vao_);

				vao_ = 0;
			}
		}

		void RenderingModel::Invalidate()
		{
			verticesSize_ = 0;

			vao_ = 0;
			vertexBuffer_ = 0;
			uvBuffer_ = 0;
			normalBuffer_ = 0;
			vertexColorBuffer_ = 0;
			glDrawType_ = GL_STATIC_DRAW;

			usingTextureID_ = 0;
			usingTextureSampleI_ = 0;
			vertAttrArray_ = -1, uvAttrArray_ = -1, normAttrArray_ = -1, vertexColorAttrArray_ = -1;
		}

		void RenderingModel::Render(const glm::mat4& VP)
		{
			for (auto& wmPtr : WorldModels())
			{	// 나를 사용하는 모든 월드 모델을 찍는다
				wmPtr->OnBeforeRendering();

				//glm::mat4 ModelMatrix = glm::mat4(1);
				//ModelMatrix = glm::translate(ModelMatrix, worldPos);

				glm::mat4 MVP = VP * wmPtr->GetTransform();

				// Send our transformation to the currently bound shader, 
				// in the "MVP" uniform
				glUniformMatrix4fv(asset_.vfShaderSet_.unif_MVPMatrixID_, 1, GL_FALSE, &MVP[0][0]);	// 이걸 유니폼으로 하지말고 VP를 프레임당 한번 고정해두고 셰이더 안에서 만드는 게 나을지도?? 프레임 비교 필요하겠다
				glUniformMatrix4fv(asset_.vfShaderSet_.unif_ModelMatrixID_, 1, GL_FALSE, &wmPtr->GetTransform()[0][0]);

				// Draw the triangles !
				glDrawArrays(GL_TRIANGLES, 0, verticesSize_);
			}
		}

		void RenderingModel::BeginRender()
		{
			glBindVertexArray(vao_);

			if (usingTextureID_ != 0)
			{
				glActiveTexture(GL_TEXTURE0 + usingTextureSampleI_);
				glBindTexture(GL_TEXTURE_2D, usingTextureID_);
				// Set our "myTextureSampler" sampler to user Texture Unit 0
				glUniform1i(asset_.vfShaderSet_.unif_TextureSampleI_, usingTextureSampleI_);
			}

			// 1rst attribute buffer : vertices
			glEnableVertexAttribArray(vertAttrArray_);
			glBindBuffer(GL_ARRAY_BUFFER, vertexBuffer_);
			glVertexAttribPointer(
				vertAttrArray_,                  // attribute
				3,                  // size
				GL_FLOAT,           // type
				GL_FALSE,           // normalized?
				0,                  // stride
				(void*)0            // array buffer offset
			);

			// 2nd attribute buffer : UVs
			glEnableVertexAttribArray(uvAttrArray_);
			glBindBuffer(GL_ARRAY_BUFFER, uvBuffer_);
			glVertexAttribPointer(
				uvAttrArray_,                                // attribute
				2,                                // size
				GL_FLOAT,                         // type
				GL_FALSE,                         // normalized?
				0,                                // stride
				(void*)0                          // array buffer offset
			);

			// 3rd attribute buffer : normals
			glEnableVertexAttribArray(normAttrArray_);
			glBindBuffer(GL_ARRAY_BUFFER, normalBuffer_);
			glVertexAttribPointer(
				normAttrArray_,                                // attribute
				3,                                // size
				GL_FLOAT,                         // type
				GL_FALSE,                         // normalized?
				0,                                // stride
				(void*)0                          // array buffer offset
			);

			glEnableVertexAttribArray(vertexColorAttrArray_);
			glBindBuffer(GL_ARRAY_BUFFER, vertexColorBuffer_);
			glVertexAttribPointer(
				vertexColorAttrArray_,                                // attribute
				3,                                // size
				GL_FLOAT,                         // type
				GL_FALSE,                         // normalized?
				0,                                // stride
				(void*)0                          // array buffer offset
			);
		}

		void RenderingModel::EndRender()
		{
			glDisableVertexAttribArray(vertAttrArray_);
			glDisableVertexAttribArray(uvAttrArray_);
			glDisableVertexAttribArray(normAttrArray_);
			glDisableVertexAttribArray(vertexColorAttrArray_);

			glBindVertexArray(0);
		}

		void RenderingModel::SetWorldInfos(const glm::mat4& viewMatrix, const glm::vec3& lightPos)
		{
			glUseProgram(GetUsingProgram());

			glUniformMatrix4fv(asset_.vfShaderSet_.unif_ViewMatrixID_, 1, GL_FALSE, &viewMatrix[0][0]);
			glUniform3f(asset_.vfShaderSet_.unif_LightWorldPosition_, lightPos.x, lightPos.y, lightPos.z);
		}

		WorldModel::WorldModel(const RenderingModel* rm)
		{
			if(rm != nullptr)
				rm->AddWorldModel(this);
		}
		WorldModel::~WorldModel()
		{
			if (renderingModel_ != nullptr)
				renderingModel_->RemoveWorldModel(this);

			renderingModel_ = nullptr;
		}
		WorldModel::WorldModel(const WorldModel& c)
		{
			this->WorldModel::WorldModel(c.renderingModel_);
		}
		WorldModel::WorldModel(WorldModel&& c) noexcept
		{
			this->WorldModel::WorldModel(c.renderingModel_);
			c.~WorldModel();
		}
	}

	namespace nsRE
	{
		////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
		////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

		// deprecated
		//CVector<OldModelAsset*> OldModelAsset::instances_;

		//OldModelAsset::OldModelAsset()
		//{
		//	instances_.push_back(this);
		//}
		//
		//OldModelAsset::OldModelAsset(CRenderingEngine* re) : OldModelAsset()
		//{
		//	re_ = re;
		//}

		//OldModelAsset::~OldModelAsset()
		//{
		//	destroyBufferObjects();

		//	auto found = std::find(instances_.begin(), instances_.end(), this);
		//	instances_.erase(found);
		//}

		//void OldModelAsset::OnScreenResize(int width, int height)
		//{
		//	createBufferObject(width, height);
		//}

		//void OldModelAsset::OnScreenResize_Master(int width, int height)
		//{
		//	for (auto inst : instances_)
		//	{
		//		inst->OnScreenResize(width, height);
		//	}
		//}

		//bool OldModelAsset::m_isInitialized()
		//{
		//	return (m_vao != -1);
		//}

		//void OldModelAsset::initRenderData()
		//{
		//	if (m_isInitialized())
		//		return;

		//	glGenVertexArrays(1, &m_vao);
		//	glGenBuffers(1, &m_vbo);

		//	// 테스트 코드
		//	////getShaderCode("../../../../simple_color_vs.glsl"),
		//	////getShaderCode("../../../../simple_color_fs.glsl")
		//	//vertShaderPath_ = "simple_color_vs.glsl";
		//	//fragShaderPath_ = "simple_color_fs.glsl";

		//	//vertices_ = {
		//	//0.0f, 0.5f, 0.0f,
		//	//-0.5f, -0.5f, 0.0f,
		//	//0.5f, -0.5f, 0.0f,
		//	//};
		//	//vertexAttribNumber_ = 3;
		//}

		//void OldModelAsset::initShaders()
		//{
		//	if (m_isInitialized())
		//		return;

		//	initRenderData();

		//	m_prg = glCreateProgram();
		//	GLUtil::addShader(m_prg, GLUtil::getShaderCode(ToASCII(vertShaderPath_).c_str()), GL_VERTEX_SHADER);
		//	GLUtil::addShader(m_prg, GLUtil::getShaderCode(ToASCII(fragShaderPath_).c_str()), GL_FRAGMENT_SHADER);
		//	glLinkProgram(m_prg);

		//	glBindVertexArray(m_vao);
		//	{
		//		auto dataSize = vertices_.size() * sizeof(decltype(vertices_[0]));

		//		glBindBuffer(GL_ARRAY_BUFFER, m_vbo);
		//		glBufferData(GL_ARRAY_BUFFER, dataSize, &vertices_[0], GL_STATIC_DRAW);
		//		glVertexAttribPointer(0, vertexAttribNumber_, GL_FLOAT, GL_FALSE, 0, (void*)0);
		//		glBindBuffer(GL_ARRAY_BUFFER, 0);
		//	}
		//	glBindVertexArray(0);
		//}

		//void OldModelAsset::drawGL(const glm::mat4& worldPos) const
		//{
		//	glBindVertexArray(m_vao);
		//	{
		//		glUseProgram(m_prg);
		//		glUniformMatrix4fv(0, 1, GL_FALSE, &worldPos[0][0]);
		//		//glUniformMatrix4fv(0, 1, GL_FALSE, glm::value_ptr(worldPos));

		//		glEnableVertexAttribArray(0);
		//		glDrawArrays(GL_TRIANGLES, 0, vertexAttribNumber_);
		//	}
		//	glBindVertexArray(0);
		//}

		//void OldModelAsset::createBufferObject(int width, int height)
		//{
		//	//destroyBufferObjects();

		//	//glGenFramebuffers(1, &m_fbo);
		//	//glGenRenderbuffers(1, &m_rbo);

		//	//glBindRenderbuffer(GL_RENDERBUFFER, m_rbo);
		//	//glRenderbufferStorage(GL_RENDERBUFFER, GL_BGRA, width, height);
		//	//glBindFramebuffer(GL_DRAW_FRAMEBUFFER, m_fbo);
		//	//glFramebufferRenderbuffer(GL_DRAW_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_RENDERBUFFER, m_rbo);
		//}

		//void OldModelAsset::destroyBufferObjects()
		//{
		//	GLUtil::safeDeleteVertexArray(m_vao);
		//	GLUtil::safeDeleteBuffer(m_vbo);
		//	GLUtil::safeDeleteProgram(m_prg);
		//	GLUtil::safeDeleteFramebuffer(m_fbo);
		//	GLUtil::safeDeleteFramebuffer(m_rbo);
		//}

		//void OldModelWorld::draw()
		//{
		//	modelAsset_.drawGL(modelMat);
		//}

		////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
		////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

		CRenderingEngine::CRenderingEngine() : m_clearColor(0.8f, 0.8f, 0.6f, 1.0f)
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

			glfwWindowHint(GLFW_SAMPLES, 4);
			glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
			glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
			glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE); // To make MacOS happy; should not be needed
			glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

#if IS_EDITOR
			glfwWindowHint(GLFW_VISIBLE, GLFW_FALSE);
#endif

			// 윈도우 독립 시키기
			m_window = glfwCreateWindow(640, 480, "Hidden OpenGL m_window", NULL, NULL);

			if (!m_window)
			{
				glfwTerminate();
				throw std::exception();
			}

			glfwMakeContextCurrent(m_window);

			// glew 때문에 막음, 서로 인클루드 빼라며 충돌남
			//if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
			//{
			//	glfwTerminate();
			//	throw std::exception();
			//}

			// But on MacOS X with a retina screen it'll be m_width*2 and m_windowHeight*2, so we get the actual framebuffer size:
			glfwGetFramebufferSize(m_window, &m_framebufferWith, &m_framebufferHeight);

			// Initialize GLEW
			glewExperimental = true; // Needed for core profile
			if (glewInit() != GLEW_OK) {
				fprintf(stderr, "Failed to initialize GLEW\n");
				getchar();
				glfwTerminate();
				return;
			}

#if IS_EDITOR
#else
			// Ensure we can capture the escape key being pressed below
			glfwSetInputMode(m_window, GLFW_STICKY_KEYS, GL_TRUE);
			// Hide the mouse and enable unlimited mouvement
			glfwSetInputMode(m_window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

			// Set the mouse at the center of the screen
			glfwPollEvents();
			glfwSetCursorPos(m_window, m_width / 2, m_height / 2);
#endif

			glClearColor(m_clearColor.r, m_clearColor.g, m_clearColor.b, m_clearColor.a);

			glEnable(GL_DEPTH_TEST);
			glDepthFunc(GL_LESS);

			glFrontFace(GL_CCW);
			glCullFace(GL_BACK);
			glEnable(GL_CULL_FACE);

			//glEnable(GL_MULTISAMPLE);
		}

		void CRenderingEngine::Resize(int width, int height)
		{
			glfwSetWindowSize(m_window, width, height);

			m_width = width;
			m_height = height;

			m_bufferLengthW = m_width * m_height * m_colorDepth;

			// 체크 사항 - 이거 다시 체크해야하려나??
			//glfwGetFramebufferSize(m_window, &m_framebufferWith, &m_framebufferHeight);
			m_bufferLengthF = m_framebufferWith * m_framebufferHeight * m_colorDepth;

			// 체크 사항 - 이거 없어도 되는데 왜 하는걸까?
			//free(GLRenderHandle);
			//GLRenderHandle = (char*)malloc(m_bufferLengthW);

			// deprecated
			//OldModelAsset::OnScreenResize_Master(m_width, m_height);

			glViewport(0, 0, m_width, m_height);
		}

		const int CRenderingEngine::GetBufferLenght()
		{
			return m_bufferLengthW;
		}

		void CRenderingEngine::Init()
		{
			if (m_window != nullptr)
				throw std::exception("it is trying reinit");

			initWindow();

			//////////////////////////////////////////////////////////////////////////////////////////
			// 초기 카메라 처리
			camera_.ComputeMatricesFromInputs(true, m_width, m_height);
			float cameraInitialDist = 20;
			camera_.SetCameraPos({ cameraInitialDist/2,cameraInitialDist,cameraInitialDist });
			camera_.SetCameraLookAt({ 0,0,0 });

			//////////////////////////////////////////////////////////////////////////////////////////
			// 게임 처리
			smge_game = new SMGE::SPPKGame(nullptr);
			smge_game->GetEngine()->SetRenderingEngine(this);

			//////////////////////////////////////////////////////////////////////////////////////////
			// 테스트 코드
			/*
			CWString suzanAMName = L"assets/models/suzanne";
			assetModels_.emplace(std::make_pair(suzanAMName, AssetModel(suzanAMName + L"/suzanne.DDS", suzanAMName + L"/suzanne.vert", suzanAMName + L"/suzanne.frag", suzanAMName + L"/suzanne.obj")));
			AssetModel& suzanAM = assetModels_.find(suzanAMName)->second;

			suzanAM.ReadyToRender();
			//renderingModels_.emplace(std::make_pair(suzanAMName, RenderingModel(suzanAM, 0)));
			//const RenderingModel& suzanRM = renderingModels_.find(suzanAMName)->second;

			worldModels_.reserve(30);	// 이거 안하면 난리남! 재할당될 때 기존 값들 다 날라감!
			worldModels_.emplace_back(&suzanAM.GetRenderingModel()).Translate({ 0, 0, 0 });
			worldModels_.emplace_back(&suzanAM.GetRenderingModel()).Translate({ -2, 0, 0 });
			worldModels_.emplace_back(&suzanAM.GetRenderingModel()).Translate({ +2, 0, 0 });

			////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
			std::vector<glm::vec3> planeVertices
			{
				{-1.0f, 0, -1.0f},
				{ -1.0f, 0, 1.0f},
				{ 1.0f, 0,  -1.0f},

				{ -1.0f, 0, 1.0f},
				{ 1.0f, 0, 1.0f},
				{ 1.0f, 0,  -1.0f},
			};
			std::vector<glm::vec2> planeUvs{ {0,0}, {0,0}, {0,0}, {0,0}, {0,0}, {0,0} };
			std::vector<glm::vec3> planeNormals{ {0,1,0}, {0,1,0},{0,1,0},{0,1,0},{0,1,0},{0,1,0} };
			std::vector<glm::vec3> planeVertexColors{ {0,0.5,0}, {0,0.5,0},{0,0.5,0},{0,0.5,0},{0,0.5,0},{0,0.5,0} };

			CWString planeAMName = L"plane";
			assetModels_.emplace(std::make_pair(planeAMName, AssetModel(L"", suzanAMName + L"/suzanne.vert", suzanAMName + L"/suzanne.frag", L"")));
			AssetModel &planeAsset = assetModels_.find(planeAMName)->second;
			planeAsset.mesh_.loadFromPlainData(planeVertices, planeUvs, planeNormals);
			planeAsset.mesh_.setVertexColors(planeVertexColors);

			renderingModels_.emplace(std::make_pair(planeAMName, RenderingModel(planeAsset, 0)));
			const RenderingModel& planeRM = renderingModels_.find(planeAMName)->second;

			WorldModel& plane1 = worldModels_.emplace_back(planeRM);
			plane1.Scale(10.f);
			plane1.Translate(glm::vec3(0, -3, 0));

			///////////////////////////////////////////////////////////////
			std::vector<glm::vec3> cubeVertices
			{
				{ -1.0f,-1.0f,-1.0f,},{ -1.0f,-1.0f, 1.0f,},{ -1.0f, 1.0f, 1.0f,},
				{  1.0f, 1.0f,-1.0f,},{ -1.0f,-1.0f,-1.0f,},{ -1.0f, 1.0f,-1.0f,},
				{  1.0f,-1.0f, 1.0f,},{ -1.0f,-1.0f,-1.0f,},{  1.0f,-1.0f,-1.0f,},
				{  1.0f, 1.0f,-1.0f,},{  1.0f,-1.0f,-1.0f,},{ -1.0f,-1.0f,-1.0f,},
				{ -1.0f,-1.0f,-1.0f,},{ -1.0f, 1.0f, 1.0f,},{ -1.0f, 1.0f,-1.0f,},
				{  1.0f,-1.0f, 1.0f,},{ -1.0f,-1.0f, 1.0f,},{ -1.0f,-1.0f,-1.0f,},
				{ -1.0f, 1.0f, 1.0f,},{ -1.0f,-1.0f, 1.0f,},{  1.0f,-1.0f, 1.0f,},
				{  1.0f, 1.0f, 1.0f,},{  1.0f,-1.0f,-1.0f,},{  1.0f, 1.0f,-1.0f,},
				{  1.0f,-1.0f,-1.0f,},{  1.0f, 1.0f, 1.0f,},{  1.0f,-1.0f, 1.0f,},
				{  1.0f, 1.0f, 1.0f,},{  1.0f, 1.0f,-1.0f,},{ -1.0f, 1.0f,-1.0f,},
				{  1.0f, 1.0f, 1.0f,},{ -1.0f, 1.0f,-1.0f,},{ -1.0f, 1.0f, 1.0f,},
				{  1.0f, 1.0f, 1.0f,},{ -1.0f, 1.0f, 1.0f,},{  1.0f,-1.0f, 1.0f	},
			};
			std::vector<glm::vec2> cubeUvs;
			cubeUvs.resize(cubeVertices.size());	// uv are all 000

			std::vector<glm::vec3> cubeNormals;	// 각 삼각형에 대하여 정점 normal, vertColor 만들어주기
			std::vector<glm::vec3> cubeVertexColors;
			cubeNormals.reserve(cubeVertices.size());
			cubeVertexColors.reserve(cubeVertices.size());

			for (size_t ii = 0; ii < cubeVertices.size(); ii += 3)
			{
				glm::vec3 _0 = cubeVertices[ii + 0], _1 = cubeVertices[ii + 1], _2 = cubeVertices[ii + 2];
				glm::vec3 _0_to_1 = _1 - _0, _0_to_2 = _2 - _0;
				glm::vec3 face_normal = glm::normalize(glm::cross(_0_to_1, _0_to_2));

				cubeNormals.push_back(face_normal);
				cubeVertexColors.push_back({ 0.5,0,0 });

				cubeNormals.push_back(face_normal);
				cubeVertexColors.push_back({ 0,0.5,0 });

				cubeNormals.push_back(face_normal);
				cubeVertexColors.push_back({ 0,0,0.5 });
			}

			CWString cubeAMName = L"cube";
			assetModels_.emplace(std::make_pair(cubeAMName, AssetModel(L"", suzanAMName + L"/suzanne.vert", suzanAMName + L"/suzanne.frag", L"")));
			AssetModel& cubeAsset = assetModels_.find(cubeAMName)->second;
			cubeAsset.mesh_.loadFromPlainData(cubeVertices, cubeUvs, cubeNormals);
			cubeAsset.mesh_.setVertexColors(cubeVertexColors);

			renderingModels_.emplace(std::make_pair(cubeAMName, RenderingModel(cubeAsset, 0)));
			const RenderingModel& cubeRM = renderingModels_.find(cubeAMName)->second;

			WorldModel& cube1 = worldModels_.emplace_back(cubeRM);

			const float cubeDist = 10.f;
			// 윗면 4점
			cube1.Scale(TransformConst::Vec3_OneHalf);
			//cube1.RotateAxis(ETypeRot::ROLL, theta);
			cube1.Translate(glm::vec3(-cubeDist, -cubeDist, -cubeDist));

			worldModels_.emplace_back(cube1).Translate(glm::vec3(+cubeDist, -cubeDist, -cubeDist));
			worldModels_.emplace_back(cube1).Translate(glm::vec3(-cubeDist, -cubeDist, +cubeDist));
			worldModels_.emplace_back(cube1).Translate(glm::vec3(+cubeDist, -cubeDist, +cubeDist));

			// 아랫면 4점
			worldModels_.emplace_back(cube1).Translate(glm::vec3(-cubeDist, +cubeDist, -cubeDist));
			worldModels_.emplace_back(cube1).Translate(glm::vec3(+cubeDist, +cubeDist, -cubeDist));
			worldModels_.emplace_back(cube1).Translate(glm::vec3(-cubeDist, +cubeDist, +cubeDist));
			worldModels_.emplace_back(cube1).Translate(glm::vec3(+cubeDist, +cubeDist, +cubeDist));
			*/
		}

		void CRenderingEngine::DeInit()
		{
			delete smge_game;
		}

		void CRenderingEngine::Tick()
		{
			smge_game->GetEngine()->Tick(0.01f);

			// 테스트 코드
			//double currentTime = glfwGetTime();
			//float theta = currentTime * 2.f;

			//int i = 0;
			//for (auto& wm : worldModels_)
			//{
			//	wm.RotateAxis(ETypeRot::YAW, glm::degrees(-theta * ((++i % 3) + 1)));
			//}
		}

		void CRenderingEngine::Render(char* imgBuffer)
		{
			glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

			glm::mat4 VP = camera_.GetProjectionMatrix() * camera_.GetViewMatrix();

			// 테스트 코드
			// 라이트를 회전시키자
			const float lightRotateRadius = 4;
			glm::vec4 lightPos = glm::vec4(0, 8, lightRotateRadius, 1);
			static double lastTime = glfwGetTime();
			double currentTime = glfwGetTime();
			float theta = currentTime * 2.f;
			glm::mat4 lightRotateMat(1);
			lightRotateMat = glm::rotate(lightRotateMat, theta, glm::vec3(0, 1, 0));
			lightPos = lightRotateMat * lightPos;
			
			for (auto& it : assetModels_)
			{
				auto& rm = it.second->GetRenderingModel();

				rm.SetWorldInfos(camera_.GetViewMatrix(), glm::vec3(lightPos));	// 셰이더 마다 1회
				rm.BeginRender();
				rm.Render(VP);
				rm.EndRender();
			}

			// smge_game->GetEngine() <- 얘가 월드모델을 상속한 액터를 갖게 된다
			//smge_game->GetEngine()->Render(0.01f);
			
			if (imgBuffer != nullptr)
				glReadPixels(0, 0, m_width, m_height, GL_BGRA, GL_UNSIGNED_BYTE, imgBuffer);	// m_colorDepth, PixelFormats::Pbgr32

			glfwSwapBuffers(m_window);
			glfwPollEvents();
		}

		void CRenderingEngine::Stop()
		{
			isRunning = false;
		}

		bool CRenderingEngine::AddAssetModel(const CWString& key, AssetModel* am)
		{
			auto already = GetAssetModel(key);
			if (already == nullptr)
			{
				assetModels_[key] = am;
				return true;
			}

			return false;
		}

		bool CRenderingEngine::RemoveAssetModel(AssetModel* am)
		{
			auto found = std::find_if(assetModels_.begin(), assetModels_.end(),
				[am](auto&& pair)
				{
					return pair.second == am;
				});

			if (found == assetModels_.end())
			{
				return false;
			}

			assetModels_.erase(found);
			return true;
		}

		AssetModel* CRenderingEngine::GetAssetModel(const CWString& key)
		{
			auto clIt = assetModels_.find(key);
			return clIt != assetModels_.end() ? clIt->second : nullptr;
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
			// deprecated
			//std::string getShaderCode(const char* filename)
			//{
			//	std::string shaderCode;
			//	std::ifstream file(filename, std::ios::in);

			//	if (!file.good())
			//	{
			//		throw new std::exception();
			//	}

			//	file.seekg(0, std::ios::end);
			//	shaderCode.resize((unsigned int)file.tellg());
			//	file.seekg(0, std::ios::beg);
			//	file.read(&shaderCode[0], shaderCode.size());
			//	file.close();

			//	return shaderCode;
			//}

			//void addShader(GLuint prgId, const std::string shadercode, GLenum shadertype)
			//{
			//	if (prgId < 0)
			//		throw new std::exception();

			//	GLuint id = glCreateShader(shadertype);

			//	if (id < 0)
			//		throw new std::exception();

			//	const char* code = shadercode.c_str();

			//	glShaderSource(id, 1, &code, NULL);
			//	glCompileShader(id);
			//	glAttachShader(prgId, id);
			//	glDeleteShader(id);
			//}

			void safeDeleteVertexArray(GLuint& vao)
			{
				if (vao != 0)
					glDeleteVertexArrays(1, &vao);
				vao = 0;
			}

			void safeDeleteBuffer(GLuint& vbo)
			{
				if (vbo != 0)
					glDeleteBuffers(1, &vbo);
				vbo = 0;
			}

			void safeDeleteProgram(GLuint& prog)
			{
				if (prog != 0)
					glDeleteProgram(prog);
				prog = 0;
			}

			void safeDeleteFramebuffer(GLuint& fbo)
			{
				if (fbo != 0)
					glDeleteFramebuffers(1, &fbo);
				fbo = 0;
			}
		}
	}
}
