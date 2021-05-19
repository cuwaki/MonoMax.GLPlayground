#pragma once
#include "common.h"
#include <iostream>
#include <ostream>
#include <fstream>
#include <functional>
#include <vector>
#include <map>
#include <forward_list>
#include <array>
#include <memory>
#include "../SMGE/GECommonIncludes.h"
#include "common/controls.hpp"

#define REFACTORING_TRNASFORM	// 20210321 회전 리팩토링

namespace SMGE
{
	class CGameBase;

	namespace nsRE
	{
		///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
		// Utility structures
		struct SRect3D
		{
			glm::vec3 lb_{ 0.f, 0.f, 0.f };
			glm::vec3 rt_{ 0.f, 0.f, 0.f };
		};
		struct SRect2D
		{
			glm::vec2 lb_{ 0.f, 0.f };
			glm::vec2 rt_{ 0.f, 0.f };
		};
		struct SVF_V2F_T2F
		{
			float vertexX_ = 0, vertexY_ = 0;
			float textureU_ = 0, textureV_ = 0;
		};

		///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
		namespace ColorConst
		{
			const glm::vec3 Gray{ 0.5f, 0.5f, 0.5f };
			const glm::vec3 Black{ 0.f, 0.f, 0.f };
			const glm::vec3 White{ 1.f, 1.f, 1.f };
			const glm::vec3 Red{ 1.f, 0.f, 0.f };
			const glm::vec3 Green{ 0.f, 1.f, 0.f };
			const glm::vec3 Blue{ 0.f, 0.f, 1.f };
		};

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

			constexpr size_t GL_LB = 0, GL_RB = 1, GL_RT = 2, GL_LT = 3;

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
			const ETypeAxis DefaultAxis_Left = ETypeAxis::X;	// 왜 Left 냐면 오른손 좌표계 기준으로 +Z가 앞이므로 +X는 Left 가 되기 때문이다

			glm::vec3 DefaultModelFrontAxis();

			glm::vec3 GetFrontAxis(const glm::mat3& currentRotMat);
			glm::vec3 GetFrontAxis(const glm::mat4& currentRotMat);

			glm::vec3 GetUpAxis(const glm::mat3& currentRotMat);
			glm::vec3 GetUpAxis(const glm::mat4& currentRotMat);

			glm::vec3 GetLeftAxis(const glm::mat3& currentRotMat);	// 왜 Left 냐면 오른손 좌표계 기준으로 +Z가 앞이므로 +X는 Left 가 되기 때문이다
			glm::vec3 GetLeftAxis(const glm::mat4& currentRotMat);

			glm::vec3 GetXAxis(const glm::mat3& currentRotMat);
			glm::vec3 GetYAxis(const glm::mat3& currentRotMat);
			glm::vec3 GetZAxis(const glm::mat3& currentRotMat);
		};


		class Transform
		{
		private:
			glm::vec3 pendingPosition_;
			glm::vec3 pendingScales_;

#ifdef REFACTORING_TRNASFORM
			glm::mat4 pendingRotationMatrix_;
#else
			glm::vec3 rotationRadianEuler_;
			glm::vec3 directionForQuat_;
#endif
			glm::vec3 anchor_;

		protected:
			glm::mat4 finalMatrix_;
			glm::vec3 finalScales_;

			Transform* parent_ = nullptr;
			std::forward_list<Transform*> children_;

			bool isAbsoluteTransform_ = false;
			bool isDirty_ = false;

		public:
			Transform();

			// 차후 필요시 열 것
			DELETE_COPY_CTOR(Transform);
			DELETE_MOVE_CTOR(Transform);

			//////////////////////////////////////////////////////////////////////////////////////////////////////////////
			// Get transform
			const glm::vec3& GetAnchor() const;
			glm::vec3 GetPositionWithAnchor(glm::vec3 basePos) const;

			glm::vec3 GetPendingPosition() const;
			glm::vec3 GetPendingScales() const;
			float GetPendingScale(TransformConst::ETypeAxis aType) const;
			glm::vec3 GetPendingAxis(TransformConst::ETypeAxis aType) const;
			glm::vec3 GetPendingFront() const;
			glm::vec3 GetPendingUp() const;
			glm::vec3 GetPendingLeft() const;
			glm::vec3 GetPendingRotationEulerDegreesWorld() const;

			glm::vec3 GetFinalPosition() const;
			float GetFinalScale(TransformConst::ETypeAxis aType) const;
			glm::vec3 GetFinalScales() const;
			glm::vec3 GetFinalAxis(TransformConst::ETypeAxis aType) const;
			glm::vec3 GetFinalFront() const;
			glm::vec3 GetFinalUp() const;
			glm::vec3 GetFinalLeft() const;
			glm::vec3 GetFinalRotationEulerDegreesWorld() const;
#ifdef REFACTORING_TRNASFORM
			const glm::mat4& GetPendingRotationMatrix() const { return pendingRotationMatrix_; }
#else
			const glm::vec3& GetRotationEuler() const;
			const glm::vec3& GetDirectionQuat() const;
#endif

			//////////////////////////////////////////////////////////////////////////////////////////////////////////////
			// Set, Change transform
			void SetAnchor(glm::vec3 anchor);
			void Translate(glm::vec3 worldPos);
			void Scale(float scale);
			void Scale(glm::vec3 scale);
			void Scale(TransformConst::ETypeAxis aType, float scale);

#ifdef REFACTORING_TRNASFORM
			void Rotate(const glm::mat4& newRotMat);
			void Rotate(const glm::vec3& basisAxis, float degrees);

			void RotateEuler(glm::vec3 rotateDegrees, bool isWorld);
			void RotateDirection(const glm::vec3& newDirNormalized, glm::vec3 newUpNormalized = TransformConst::WorldYAxis, bool isSecurePerp = true);
#else
			void RotateEuler(glm::vec3 rotateDegrees);
			void RotateEuler(TransformConst::ETypeRot rType, float degrees);
			void RotateQuat(const glm::vec3& dirForQuat);
#endif

			//////////////////////////////////////////////////////////////////////////////////////////////////////////////
			// etc
			const glm::mat4& FinalMatrix(bool isRecalc);
			const glm::mat4& FinalMatrixNoRecalc() const;

			virtual void OnBeforeRendering();

			void Dirty(bool isForce = false);
			bool IsDirty() const;
			void RecalcFinal();

			void ChangeParent(Transform* p);

			template<typename T>
			T* GetParent()
			{
				return dynamic_cast<T*>(parent_);
			}
			template<typename T>
			const T* GetParentConst() const
			{
				return dynamic_cast<const T*>(parent_);
			}

			template<typename T>
			T* GetTopParent()
			{
				if (GetParent<T>())
				{
					return GetParent<T>()->GetTopParent<T>();
				}
				return dynamic_cast<T*>(this);
			}
			template<typename T>
			const T* GetTopParentConst() const
			{
				if (GetParentConst<T>())
				{
					return GetParentConst<T>()->GetTopParentConst<T>();
				}

				return dynamic_cast<const T*>(this);
			}

			bool HasParent() const;
			bool IsTop() const;

		private:
			void RecalcFinal_Internal(const Transform* parent);
		};

		class VertFragShaderSet
		{
			static std::map<CWString, std::unique_ptr<VertFragShaderSet>> Cache;

		public:
			template<typename T>
			static T* FindOrLoadShaderSet(const CWString& vertShadPath, const CWString& fragShadPath)
			{
				std::wstring vsfsKey(vertShadPath + fragShadPath);
				SMGE::ToLowerInline(vsfsKey);

				auto cachedVFSet = Cache.find(vsfsKey);
				if (cachedVFSet != Cache.end())
				{
					return static_cast<T*>((*cachedVFSet).second.get());
				}

				Cache[vsfsKey] = std::make_unique<T>(vertShadPath.c_str(), fragShadPath.c_str());

				return static_cast<T*>(Cache[vsfsKey].get());
			}

			// 여기 - 아 이거 프로텍티여야하는데... 일단 컴파일이 안되서 열어둠
			VertFragShaderSet(const CWString& vertShadPath, const CWString& fragShadPath);

		protected:
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

			void Use() const;

			void SetUniform_Bool(std::string_view name, bool value) const
			{
				glUniform1i(glGetUniformLocation(programID_, name.data()), (int)value);
			}
			void SetUniform_Int(std::string_view name, int value) const
			{
				glUniform1i(glGetUniformLocation(programID_, name.data()), value);
			}
			void SetUniform_Float(std::string_view name, float value) const
			{
				glUniform1f(glGetUniformLocation(programID_, name.data()), value);
			}			
			void SetUniform_Vec2(std::string_view name, const glm::vec2& value) const
			{
				glUniform2fv(glGetUniformLocation(programID_, name.data()), 1, &value[0]);
			}
			void SetUniform_Vec2(std::string_view name, float x, float y) const
			{
				glUniform2f(glGetUniformLocation(programID_, name.data()), x, y);
			}			
			void SetUniform_Vec3(std::string_view name, const glm::vec3& value) const
			{
				glUniform3fv(glGetUniformLocation(programID_, name.data()), 1, &value[0]);
			}
			void SetUniform_Vec3(std::string_view name, float x, float y, float z) const
			{
				glUniform3f(glGetUniformLocation(programID_, name.data()), x, y, z);
			}
			void SetUniform_Vec4(std::string_view name, const glm::vec4& value) const
			{
				glUniform4fv(glGetUniformLocation(programID_, name.data()), 1, &value[0]);
			}
			void SetUniform_Vec4(std::string_view name, float x, float y, float z, float w) const
			{
				glUniform4f(glGetUniformLocation(programID_, name.data()), x, y, z, w);
			}
			void SetUniform_Mat2(std::string_view name, const glm::mat2& mat) const
			{
				glUniformMatrix2fv(glGetUniformLocation(programID_, name.data()), 1, GL_FALSE, &mat[0][0]);
			}
			void SetUniform_Mat3(std::string_view name, const glm::mat3& mat) const
			{
				glUniformMatrix3fv(glGetUniformLocation(programID_, name.data()), 1, GL_FALSE, &mat[0][0]);
			}
			void SetUniform_Mat4(std::string_view name, const glm::mat4& mat) const
			{
				glUniformMatrix4fv(glGetUniformLocation(programID_, name.data()), 1, GL_FALSE, &mat[0][0]);
			}
		};

		class TextureData
		{
		public:
			GLuint format_;
			GLuint mipMapCount_;
			GLuint width_, height_;
			unsigned char* image_ = nullptr;

			TextureData() {}
			TextureData(const CWString& texPath);
			~TextureData();

			bool loadFromFile(const CWString& texPath);
			void Destroy();

			TextureData(const TextureData& c) = delete;
			TextureData& operator=(const TextureData & c) = delete;
			TextureData(TextureData && c) noexcept;
			TextureData& operator=(TextureData && c) noexcept;
		};

		class MeshData
		{
		public:
			std::vector<glm::vec3> vertices_;
			std::vector<glm::vec2> uvs_;
			std::vector<glm::vec3> normals_;
			std::vector<glm::vec3> vertexColors_;

			MeshData() { Invalidate(); }
			MeshData(const CWString& objPath);
			~MeshData() { Destroy(); }

			bool loadFromFile(const CWString& objPath);
			bool loadFromPlainData(const std::vector<glm::vec3>& vertices, const std::vector<glm::vec2>& uvs, const std::vector<glm::vec3>& normals);
			bool setVertexColors(const std::vector<glm::vec3>& vertexColors);
			void Destroy();
			void Invalidate();

			MeshData(const MeshData& c) = delete;
			MeshData& operator=(const MeshData& c) = delete;
			MeshData(MeshData&& c) noexcept;
			MeshData& operator=(MeshData&& c) noexcept;
		};

		class WorldModel : public Transform
		{
		protected:
			class RenderModel* renderModel_;

			friend class RenderModel;

		public:
			WorldModel(class RenderModel* rm);
			virtual ~WorldModel();

			WorldModel(const WorldModel& c);
			WorldModel& operator=(const WorldModel& c) = delete;
			WorldModel(WorldModel&& c) noexcept;
			WorldModel& operator=(WorldModel&& c) noexcept = delete;

			void SetRendering(bool isv, bool propagate);
			bool IsRendering() const;

			class RenderModel* GetRenderModel() const;

		protected:
			bool isRendering_ = true;
		};

		class GLContextDependency
		{
			// 여기 - 이거 멀티컨텍스트 구현할 때 렌더모델 등 중복할당하지 말고 메모리 공유 기능 써볼까??? 아니면 그냥 바로 벌캔으로 변경할까? 이건 시간이 너무 오래 걸린다.
			// 일단 최소한으로 멀티컨텍스트 구현해서 샘플 게임을 만들도록 하자!!
		};

		// RenderModel은 GLContext 종속이다
		class RenderModel : public GLContextDependency
		{
		public:
			// static datas
			const class ResourceModelBase& resourceModel_;

			// rendering datas
			GLsizei verticesSize_ = 0;

			// gl buffers
			GLuint vao_ = 0;
			GLuint vertexBuffer_ = 0;
			GLuint uvBuffer_ = 0;
			GLuint normalBuffer_ = 0;
			GLuint vertexColorBuffer_ = 0;
			GLuint glDrawType_ = GL_STATIC_DRAW;

			// shader
			VertFragShaderSet* vfShaderSet_ = nullptr;

			// texture
			GLuint usingTextureID_ = 0;
			GLuint usingTextureSampleI_ = 0;

#pragma region WorldModel
			mutable std::vector<WorldModel*> worldModelsW_;
			void AddWorldModel(WorldModel* wm);
			void RemoveWorldModel(WorldModel* wm) const;
			const std::vector<WorldModel*>& WorldModels() const;
#pragma endregion

			virtual ~RenderModel();
			void Destroy();
			void Invalidate();

			RenderModel(const ResourceModelBase& resModelBase, GLuint texSamp);

			DELETE_COPY_CTOR(RenderModel);

			RenderModel(RenderModel&& c) noexcept;
			RenderModel& operator=(RenderModel&& c) = delete;	// resource_ 때문에 구현 불가

			bool GenGLMeshDatas(const std::vector<glm::vec3>& vertices, const std::vector<glm::vec2>& uvs, const std::vector<glm::vec3>& normals, const std::vector<glm::vec3>& vertexColors);
			virtual void CreateFromResource();

			virtual GLuint GetTextureID(int texSamp) const;
			virtual const VertFragShaderSet* GetShaderSet() const;
			virtual GLuint GetShaderID() const;

			virtual void CallGLDraw(size_t verticesSize) const;
			void RenderAll(const glm::mat4& VP);
			void Render(const glm::mat4& VP, class WorldModel* wm);
			virtual void BeginRender();
			virtual void EndRender();
			void UseShader(const glm::mat4& V, const glm::vec3& lightPos);
		};

		// ResourceModel은 GLContext 종속이 아니라서 여러 GLContext에서 공용으로 쓸 수 있다.
		class ResourceModelBase : public std::enable_shared_from_this<ResourceModelBase>
		{
		protected:
			mutable std::map<const GLFWwindow*, std::unique_ptr<RenderModel>> renderModelsPerContext_;

		public:
			ResourceModelBase() {}
#if defined(_DEBUG) || defined(DEBUG)
			~ResourceModelBase();
#endif
			virtual void Invalidate();

			class RenderModel* GetRenderModel(const GLFWwindow* contextWindow) const;

			virtual const MeshData& GetMesh() const;
			virtual const TextureData& GetTexture() const;

			DELETE_COPY_CTOR(ResourceModelBase);

			ResourceModelBase(ResourceModelBase&& c) noexcept;
			ResourceModelBase& operator=(ResourceModelBase&& c) noexcept;

		protected:
			virtual void NewAndRegisterRenderModel(const GLFWwindow* contextWindow) const;
		};

		class ResourceModel : public ResourceModelBase
		{
		protected:
			TextureData texture_;
			MeshData mesh_;
			CWString vertShaderPath_;
			CWString fragShaderPath_;

		public:
			ResourceModel() {}

			virtual void LoadFromFiles(const CWString& textureFilePath, const CWString& objPath, const CWString& vertShadPath, const CWString& fragShadPath);

			virtual const MeshData& GetMesh() const override { return mesh_; }
			virtual MeshData& GetMesh() { return mesh_; }
			virtual const TextureData& GetTexture() const override { return texture_; }
			virtual TextureData& GetTexture() { return texture_; }

			const CWString& GetVertShaderPath() const { return vertShaderPath_; }
			const CWString& GetFragShaderPath() const { return fragShaderPath_; }

			ResourceModel(ResourceModel&& c) noexcept;
			ResourceModel& operator=(ResourceModel&& c) noexcept;
		};
	
		class CRenderingEngine;

		class CResourceModelProvider
		{
			static CHashMap<CString, std::shared_ptr<ResourceModelBase>> ResourceModels;

		public:
			static std::shared_ptr<nsRE::ResourceModelBase> AddResourceModel(const CString& key, std::shared_ptr<ResourceModelBase> am);
			static bool RemoveResourceModel(const CString& key);
			static bool RemoveResourceModel(ResourceModelBase* am);
			static std::shared_ptr<ResourceModelBase> FindResourceModel(const CString& key);

			static const CHashMap<CString, std::shared_ptr<ResourceModelBase>>& GetResourceModels();
		};

		//////////////////////////////////////////////////////////////////////////////////////////////////////////////////
		#define SAFE_DELETE_GL(glObjectTypeName, count, ptrGLObjectName)	{ glDelete##glObjectTypeName (count, ptrGLObjectName); memset(ptrGLObjectName, 0, sizeof(*ptrGLObjectName) * count); }

		// 여기 - 아오, 멀티 컨텍스트 지원하려면 이거 바꿔야한다
		class CGLState : public GLContextDependency
		{
		public:
			static void PushState(std::string_view glState);
			static void PopState(std::string_view glState);
			static void TopState(std::string_view glState, std::any& out);

		protected:
			static std::stack<GLint> fboStack_;
		};

		//////////////////////////////////////////////////////////////////////////////////////////////////////////////////
		// 렌더 타겟
		class CRenderTarget : public GLContextDependency
		{
		protected:
			DEFAULT_COPY_CTOR(CRenderTarget);	// 이동 생성자에서 나만 쓴다!!

			constexpr static GLsizei QuadVertexNumber = 6;

		public:
			CRenderTarget(glm::vec2 size, GLuint colorFormat, GLuint depthStencil, SRect2D viewportNDC, glm::vec3 clearColor);
			~CRenderTarget();
		
			CRenderTarget(CRenderTarget&& other) noexcept;
			CRenderTarget& operator=(CRenderTarget&& other) noexcept;

			bool IsUsingBackBuffer() const;

			void BindFrameBuffer() const;
			void UnbindFrameBuffer() const;
			void ClearFrameBuffer(GLuint flags = 0) const;

			void SetClearColor(glm::vec3 cc) { clearColor_ = cc; }

			GLuint GetQuadVAO() const { return quadVAO_; }
			GLuint GetQuadVBO() const { return quadVBO_; }
			GLuint GetColorTextureName() const { return colorTextureName_; }
			GLsizei GetQuadVertexNumber() const { return QuadVertexNumber; }

		protected:
			CRenderingCamera* renderingCameraW_;
			
			SRect2D viewportNDC_;
			glm::vec2 size_;
			GLuint colorFormat_ = 0;
			GLuint depthStencilFormat_ = 0;
			glm::vec3 clearColor_{ 1.f, 0.f, 1.f };

			// gl objects
			GLuint quadVAO_ = 0;
			GLuint quadVBO_ = 0;
			GLuint fbo_ = 0, rbo_ = 0, colorTextureName_ = 0;
		};

		//////////////////////////////////////////////////////////////////////////////////////////////////////////////////
		// 렌더링 패스
		class CRenderingPass
		{
		public:
			CRenderingPass();

			DELETE_COPY_CTOR(CRenderingPass);
			DELETE_MOVE_CTOR(CRenderingPass);

			virtual void RenderTo(const glm::mat4& V, const glm::mat4& VP, CRenderTarget*& writeRT, CRenderTarget*& readRT) = 0;
		};

		class CPostEffectPass : public CRenderingPass
		{
		public:
			CPostEffectPass(std::wstring_view vsFilePath, std::wstring_view fsFilePath);

			DELETE_COPY_CTOR(CPostEffectPass);
			DELETE_MOVE_CTOR(CPostEffectPass);

			virtual void RenderTo(const glm::mat4& V, const glm::mat4& VP, CRenderTarget*& writeRT, CRenderTarget*& readRT) override;

		protected:
			VertFragShaderSet* posteffectShader_ = nullptr;
		};

		//////////////////////////////////////////////////////////////////////////////////////////////////////////////////
		// 렌더링 엔진
		class CRenderingEngine
		{
		private:
			const GLint GL_ColorType = GL_BGRA;	// PixelFormats::Pbgr32
			const int m_colorDepth = 4;			// GL_ColorType == GL_BGRA

			float m_dpiRate;
			int m_widthOriginal, m_heightOriginal;
			int m_widthWindowDPI, m_heightWindowDPI;

			int m_bufferLengthW, m_bufferLengthF;	// window 기반 버퍼 크기와 frame 기반 버퍼 크기
			int m_framebufferWidth, m_framebufferHeight;

			glm::vec4 m_clearColor;
			GLFWwindow* m_window = nullptr;
			char* m_glRenderHandle = nullptr;
			bool m_isRunning = false;

			CRenderingCamera renderingCamera_;
			std::unique_ptr<CRenderTarget> renderTarget0_, renderTarget1_, renderTargetBackBuffer_;
			std::vector<std::unique_ptr<CRenderingPass>> renderingPasses_;
			std::unique_ptr<CPostEffectPass> lastRenderingPass_;

			void initWindow();

			// 여기 - 20210214 생각 - 게임 말고 엔진이 들어와야한다, 엔진->게임으로 구조를 바꾸자
			std::unique_ptr<class CGameBase> gameBase_;

		public:
			CRenderingEngine(int widthOriginal, int heightOriginal, float dpiRate);
			virtual ~CRenderingEngine();

			void Init();
			void DeInit();

			int GetWidth() const { return m_widthWindowDPI; }
			int GetHeight() const { return m_heightWindowDPI; }
			void Resize(int width, int height);

			void Stop();

			void Tick();
#if IS_EDITOR
			void Render(char* imgBuffer);
			void getWriteableBitmapInfo(double& outDpiX, double& outDpiY, int& outColorDepth);
#else
			void Render();
#endif
			HWND GetHwndGLFWWindow() const;
			void ScreenPosToWorld(const glm::vec2& mousePos, glm::vec3& outWorldPos, glm::vec3& outWorldDir);

			const CRenderingCamera& GetRenderingCamera() const { return renderingCamera_; }
			CRenderingCamera& GetRenderingCamera() { return renderingCamera_; }

			std::vector<std::unique_ptr<CRenderingPass>>& GetRenderingPasses() { return renderingPasses_; }

		protected:
			void ResizeRenderTargets();
		};
	}
}
