#pragma once
#include "common.h"
#include <iostream>
#include <ostream>
#include <fstream>
#include <functional>
#include <vector>
#include <map>
#include <forward_list>
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

			const glm::mat3 Mat3_Identity(1);
			const glm::mat4 Mat4_Identity(1);

			const glm::vec3 WorldXAxis{ 1, 0, 0 };
			const glm::vec3 WorldYAxis{ 0, 1, 0 };
			const glm::vec3 WorldZAxis{ 0, 0, 1 };
			const glm::vec3 WorldAxis[3] = { WorldXAxis, WorldYAxis, WorldZAxis };	// access with ETypeAxis

			const ETypeAxis DefaultAxis_Front = ETypeAxis::Z;
			const ETypeAxis DefaultAxis_Up = ETypeAxis::Y;
			const ETypeAxis DefaultAxis_Left = ETypeAxis::X;

			const glm::vec3 DefaultModelFrontAxis();
			const glm::vec3 DefaultModelFrontAxis(const glm::mat3& currentRotMat);
			const glm::vec3 DefaultModelFrontAxis(const glm::mat4& currentRotMat);

			const glm::vec3 DefaultModelUpAxis(const glm::mat3& currentRotMat);
			const glm::vec3 DefaultModelUpAxis(const glm::mat4& currentRotMat);

			const glm::vec3 DefaultModelLeftAxis(const glm::mat3& currentRotMat);
			const glm::vec3 DefaultModelLeftAxis(const glm::mat4& currentRotMat);
		};

		class VertFragShaderSet
		{
			static std::map<CWString, VertFragShaderSet> Cache;

		public:
			static VertFragShaderSet& FindOrLoadShaderSet(const CWString& vertShadPath, const CWString& fragShadPath);

		protected:
			VertFragShaderSet(const CWString& vertShadPath, const CWString& fragShadPath);

			VertFragShaderSet(const VertFragShaderSet& c) = delete;
			VertFragShaderSet& operator=(const VertFragShaderSet& c) = delete;

			VertFragShaderSet(VertFragShaderSet&& c) noexcept;
			VertFragShaderSet& operator=(VertFragShaderSet&& c) noexcept;

		public:
			GLuint programID_;
			GLuint unif_MVPMatrixID_;
			GLuint unif_ViewMatrixID_;
			GLuint unif_ModelMatrixID_;
			GLuint unif_TextureSampleI_;
			GLuint unif_LightWorldPosition_;
			GLuint vertAttrArray_, uvAttrArray_, normAttrArray_, vertexColorAttrArray_;

			VertFragShaderSet() { Invalidate(); }
			~VertFragShaderSet();
			void Destroy();
			void Invalidate();
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

		class Transform
		{
		private:
			glm::vec3 translation_;
			glm::vec3 rotationRadianEuler_;
			glm::vec3 lookAtDirection_;
			glm::vec3 scale_;

		protected:
			glm::mat4 transformMatrix_;
			Transform* parent_ = nullptr;
			std::forward_list<Transform*> children_;

			bool isDirty_ = false;

		public:
			Transform();

			const glm::mat4& GetMatrix(bool forceRecalc);
			const glm::mat4& GetMatrixNoRecalc() const;

			const glm::vec3& GetTranslation() const;
			const glm::vec3& GetRotationEuler() const;
			const glm::vec3& GetLookAtDirection() const;
			const glm::vec3& GetScales() const;
			float GetScale(TransformConst::ETypeAxis aType) const;

			glm::vec3 GetWorldPosition() const;
			glm::vec3 GetWorldAxis(TransformConst::ETypeAxis aType) const;
			glm::vec3 GetWorldFront() const;
			glm::vec3 GetWorldUp() const;
			glm::vec3 GetWorldLeft() const;
			float GetWorldScale(TransformConst::ETypeAxis aType) const;
			glm::vec3 GetWorldScales() const;

			void Translate(glm::vec3 worldPos);
			void RotateEuler(glm::vec3 rotateDegrees);
			void RotateEuler(TransformConst::ETypeRot rType, float degrees);
			void RotateQuat(const glm::vec3& lookAtDir);
			void Scale(float scale);
			void Scale(glm::vec3 scale);
			void Scale(TransformConst::ETypeAxis aType, float scale);

			void OnBeforeRendering();

			void Dirty();
			bool IsDirty() const;
			void RecalcMatrix();

			void ChangeParent(Transform* p);
			
			Transform* GetParent();
			const Transform* GetParentConst() const;

			Transform* GetTopParent();
			const Transform* GetTopParentConst() const;

			bool HasParent() const;
			bool IsTop() const;

		private:
			void RecalcMatrix_Internal(const Transform* parent);

			mutable glm::vec3 inheritedScale_;
		};

		class WorldObject : public Transform
		{
		protected:
			const class RenderModel* renderModel_;

			friend class RenderModel;

		public:
			WorldObject(const class RenderModel* rm);
			virtual ~WorldObject();

			WorldObject(const WorldObject& c);
			WorldObject& operator=(const WorldObject& c) = delete;
			WorldObject(WorldObject&& c) noexcept;
			WorldObject& operator=(WorldObject&& c) noexcept = delete;

			void SetVisible(bool isv) { isVisible_ = isv; }
			bool IsVisible() const { return isVisible_; }

		protected:
			bool isVisible_ = true;
		};

		// ResourceModel은 GLContext 종속이 아니라서 여러 GLContext에서 공용으로 쓸 수 있다.
		class ResourceModelBase
		{
		protected:
			class RenderModel* renderModel_ = nullptr;

			friend class RenderModel;

		public:
			ResourceModelBase() {}
			virtual ~ResourceModelBase();
			
			virtual void Invalidate();
			virtual void CreateRenderModel();
			
			virtual GLuint GetTextureID(int texSamp) const;
			virtual const MeshOBJ& GetMesh() const;
			virtual const VertFragShaderSet* GetShaderSet() const;
			virtual GLuint GetShaderID() const { return 0; }

			bool IsGizmo() const;
			virtual void CallDefaultGLDraw(size_t verticesSize) const;

			class RenderModel& GetRenderModel() const;

			ResourceModelBase(const ResourceModelBase& c) = delete;
			ResourceModelBase& operator=(const ResourceModelBase& c) = delete;
			ResourceModelBase(ResourceModelBase&& c) noexcept;
			ResourceModelBase& operator=(ResourceModelBase&& c) noexcept;
		};

		class ResourceModel : public ResourceModelBase
		{
		protected:
			TextureDDS texture_;
			VertFragShaderSet* vfShaderSet_ = nullptr;
			MeshOBJ mesh_;

		public:
			ResourceModel() {}
			ResourceModel(const CWString& textureFilePath, const CWString& vertShadPath, const CWString& fragShadPath, const CWString& objPath);

			virtual GLuint GetTextureID(int texSamp) const override { return texture_.textureID_; }
			virtual const MeshOBJ& GetMesh() const override { return mesh_; }
			virtual MeshOBJ& GetMesh() { return mesh_; }
			virtual const VertFragShaderSet* GetShaderSet() const override { return vfShaderSet_; }
			virtual GLuint GetShaderID() const override { return vfShaderSet_->programID_; }

			ResourceModel(ResourceModel&& c) noexcept;
			ResourceModel& operator=(ResourceModel&& c) noexcept;
		};

		// RenderModel은 GLContext 종속이다
		class RenderModel
		{
		public:
			// static datas
			const ResourceModelBase& resource_;

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

#pragma region WorldObject
			mutable std::vector<WorldObject*> ptrWorldObjects_;
			void AddWorldObject(WorldObject* wm) const;
			void RemoveWorldObject(WorldObject* wm) const;
			const std::vector<WorldObject*>& WorldObjects() const;
#pragma endregion

			virtual ~RenderModel();
			void Destroy();
			void Invalidate();

			RenderModel(const ResourceModelBase& asset, GLuint texSamp);

			RenderModel(const RenderModel& c) = delete;
			RenderModel& operator=(const RenderModel& c) = delete;
			RenderModel(RenderModel&& c) noexcept;
			RenderModel& operator=(RenderModel&& c) = delete;	// resource_ 때문에 구현 불가

			bool GenOpenGLBuffers(const std::vector<glm::vec3>& vertices, const std::vector<glm::vec2>& uvs, const std::vector<glm::vec3>& normals, const std::vector<glm::vec3>& vertexColors);
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

			CHashMap<CString, ResourceModelBase*> resourceModels_;
			//CVector<WorldObject> WorldObjects_;

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

			bool AddResourceModel(const CString& key, ResourceModelBase* am);
			bool RemoveResourceModel(ResourceModelBase* am);
			ResourceModelBase* GetResourceModel(const CString& key);

			void getWriteableBitmapInfo(double& outDpiX, double& outDpiY, int& outColorDepth);
			void ScreenPosToWorld(const glm::vec2& mousePos, glm::vec3& outWorldPos, glm::vec3& outWorldDir);

			CCamera* GetCamera() { return &camera_; }
		};
	}
}
