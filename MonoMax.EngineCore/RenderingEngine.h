#pragma once
#include "common.h"
#include <iostream>
#include <ostream>
#include <fstream>
#include <functional>
#include <vector>
#include "../SMGE/GECommonIncludes.h"
#include "../SMGE/GEContainers.h"
#include "common/controls.hpp"

namespace SMGE
{
	namespace nsGE
	{
		class CGameBase;
	}

	namespace nsRE
	{
		namespace TransformConst
		{
			enum ETypeRot
			{
				PITCH = 0,	// x, y, z 순서로 인덱스로 활용할 수 있도록
				YAW,
				ROLL,
				ETypeRot_MAX
			};

			enum ETypeAxis
			{
				X = 0,
				Y,
				Z,
				ETypeAxis_MAX
			};

			const glm::vec3 Vec3_Zero(0);
			const glm::vec3 Vec3_OneHalf(0.5);
			const glm::vec3 Vec3_One(1);

			const glm::mat4 Mat4_Identity(1);

			const glm::vec3 WorldXAxis{ 1, 0, 0 };
			const glm::vec3 WorldYAxis{ 0, 1, 0 };
			const glm::vec3 WorldZAxis{ 0, 0, 1 };
			const glm::vec3 WorldAxis[3] = { WorldXAxis, WorldYAxis, WorldZAxis };	// access with ETypeAxis
		};

		class VertFragShaderSet
		{
		public:
			GLuint programID_;
			GLuint unif_MVPMatrixID_;
			GLuint unif_ViewMatrixID_;
			GLuint unif_ModelMatrixID_;
			GLuint unif_TextureSampleI_;
			GLuint unif_LightWorldPosition_;

			VertFragShaderSet() { Invalidate(); }
			VertFragShaderSet(const CWString& vertShadPath, const CWString& fragShadPath);
			~VertFragShaderSet();
			void Destroy();
			void Invalidate();

			VertFragShaderSet(const VertFragShaderSet& c) = delete;
			VertFragShaderSet& operator=(const VertFragShaderSet& c) = delete;

			VertFragShaderSet(VertFragShaderSet&& c) noexcept;
			VertFragShaderSet& operator=(VertFragShaderSet&& c) noexcept;
		};

		class TextureDDS
		{
		public:
			GLuint textureID_;

			TextureDDS() { Invalidate(); }
			TextureDDS(const CWString& texPath);
			~TextureDDS();
			void Destroy();
			void Invalidate();

			TextureDDS(const TextureDDS& c) = delete;
			TextureDDS& operator=(const TextureDDS & c) = delete;
			TextureDDS(TextureDDS && c) noexcept;
			TextureDDS& operator=(TextureDDS && c) noexcept;
		};

		class MeshOBJ
		{
		public:
			std::vector<glm::vec3> vertices_;
			std::vector<glm::vec2> uvs_;
			std::vector<glm::vec3> normals_;
			std::vector<glm::vec3> vertexColors_;

			MeshOBJ() { Invalidate(); }
			MeshOBJ(const CWString& objPath);
			~MeshOBJ() { Destroy(); }

			bool loadFromOBJFile(const CWString& objPath);
			bool loadFromPlainData(const std::vector<glm::vec3>& vertices, const std::vector<glm::vec2>& uvs, const std::vector<glm::vec3>& normals);
			bool setVertexColors(const std::vector<glm::vec3>& vertexColors);
			void Destroy();
			void Invalidate();

			MeshOBJ(const MeshOBJ& c) = delete;
			MeshOBJ& operator=(const MeshOBJ& c) = delete;
			MeshOBJ(MeshOBJ&& c) noexcept;
			MeshOBJ& operator=(MeshOBJ&& c) noexcept;
		};

		class AssetModel
		{
		protected:
			class RenderingModel* renderingModel_ = nullptr;
			
			TextureDDS texture_;
			VertFragShaderSet vfShaderSet_;
			MeshOBJ mesh_;

			friend class RenderingModel;

		public:
			AssetModel() {}
			AssetModel(const CWString& textureFilePath, const CWString& vertShadPath, const CWString& fragShadPath, const CWString& objPath);
			virtual ~AssetModel() { Destroy(); }
			void Destroy();
			void Invalidate();

			void ReadyToRender();
			class RenderingModel& GetRenderingModel() const;

			AssetModel(const AssetModel& c) = delete;
			AssetModel& operator=(const AssetModel& c) = delete;
			AssetModel(AssetModel&& c);
			AssetModel& operator=(AssetModel&& c);
		};

		class Transform
		{
		private:
			glm::vec3 translation_;
			glm::vec3 rotationDegree_;
			glm::vec3 scale_;

		protected:
			glm::mat4 currentTransform_;
			Transform* transformParent_ = nullptr;

			bool isDirty_ = false;

		public:
			Transform();

			const glm::mat4& GetTransform(bool forceRecalc = false);
			const glm::vec3& GetLocation() const;
			const glm::vec3& GetRotation() const;
			const glm::vec3& GetScale() const;

			void Translate(glm::vec3 worldPos);
			void Rotate(glm::vec3 rotateDegrees);
			void RotateAxis(TransformConst::ETypeRot rType, float degrees);
			void Scale(float scale);
			void Scale(glm::vec3 scale);
			void ScaleAxis(TransformConst::ETypeAxis aType, float scale);

			void OnBeforeRendering();

			void Dirty() { isDirty_ = true; }
			bool IsDirty() { return isDirty_; }

		protected:
			void RecalcMatrix();
		};

		class WorldModel : public Transform
		{
		protected:
			const class RenderingModel* renderingModel_;

			friend class RenderingModel;

		public:
			WorldModel(const class RenderingModel* rm);
			virtual ~WorldModel();

			WorldModel(const WorldModel& c);
			WorldModel& operator=(const WorldModel& c) = delete;
			WorldModel(WorldModel&& c) noexcept;
			WorldModel& operator=(WorldModel&& c) noexcept = delete;
		};

		class RenderingModel
		{
		public:
			// static datas
			const AssetModel& asset_;

			// rendering datas
			GLsizei verticesSize_ = 0;

			GLuint vao_ = 0;
			GLuint vertexBuffer_ = 0;
			GLuint uvBuffer_ = 0;
			GLuint normalBuffer_ = 0;
			GLuint vertexColorBuffer_ = 0;
			GLuint glDrawType_ = GL_STATIC_DRAW;

			// shader
			GLuint usingTextureID_ = 0;
			GLuint usingTextureSampleI_ = 0;
			GLuint vertAttrArray_ = -1, uvAttrArray_ = -1, normAttrArray_ = -1, vertexColorAttrArray_ = -1;

#pragma region WorldModel
			mutable std::vector<WorldModel*> ptrWorldModels_;
			void AddWorldModel(WorldModel* wm) const;
			void RemoveWorldModel(WorldModel* wm) const;
			const std::vector<WorldModel*>& WorldModels() const;
#pragma endregion

			virtual ~RenderingModel();
			void Destroy();
			void Invalidate();

			RenderingModel(const AssetModel& asset, GLuint texSamp);

			RenderingModel(const RenderingModel& c) = delete;
			RenderingModel& operator=(const RenderingModel& c) = delete;
			RenderingModel(RenderingModel&& c) noexcept;
			RenderingModel& operator=(RenderingModel&& c) = delete;	// asset_ 때문에 구현 불가

			GLuint GetUsingProgram();
			bool GenBindData(const std::vector<glm::vec3>& vertices, const std::vector<glm::vec2>& uvs, const std::vector<glm::vec3>& normals, const std::vector<glm::vec3>& vertexColors);
			void Render(const glm::mat4& VP);
			void BeginRender();
			void EndRender();
			void SetWorldInfos(const glm::mat4& viewMatrix, const glm::vec3& lightPos);
		};
	}

	namespace nsRE
	{
		class CRenderingEngine;

		// deprecated
		//class OldModelAsset
		//{
		//protected:
		//	CRenderingEngine* re_ = nullptr;

		//	GLuint m_vao = -1, m_vbo = -1;
		//	GLuint m_rbo = -1, m_fbo = -1;
		//	GLuint m_prg = -1;

		//	bool m_isInitialized();
		//	void initRenderData();
		//	void createBufferObject(int width, int height);
		//	void destroyBufferObjects();

		//public:
		//	CWString vertShaderPath_;
		//	CWString fragShaderPath_;
		//	int vertexAttribNumber_ = 0;
		//	CVector<float> vertices_;

		//	OldModelAsset();
		//	OldModelAsset(CRenderingEngine* re);
		//	~OldModelAsset() noexcept;

		//	void initShaders();
		//	void drawGL(const glm::mat4& worldPos) const;
		//	void OnScreenResize(int width, int height);

		//public:
		//	static void OnScreenResize_Master(int width, int height);
		//	static CVector<OldModelAsset*> instances_;
		//};
		//class OldModelWorld
		//{
		//protected:
		//	CRenderingEngine* re_ = nullptr;
		//	const OldModelAsset& modelAsset_;

		//public:
		//	glm::mat4 modelMat;

		//	OldModelWorld(const OldModelAsset& ma) : modelAsset_(ma)
		//	{
		//		modelMat = glm::mat4(1.0f);
		//	}

		//	void SetRenderingEngine(CRenderingEngine* re)
		//	{
		//		re_ = re;
		//	}

		//	virtual void draw();
		//};

		namespace GLUtil
		{
			// deprecated
			//std::string getShaderCode(const char* filename);
			//void addShader(GLuint prgId, const std::string shadercode, GLenum shadertype);

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

			CCamera camera_;

			// 테스트 코드
			CHashMap<CWString, AssetModel> assetModels_;
			//CVector<WorldModel> worldModels_;
			// deprecated
			//CVector<OldModelWorld*> m_oldWorldModelList;

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
		};
	}
}
