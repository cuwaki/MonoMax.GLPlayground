#pragma once

#include "RenderingEngine.h"

namespace SMGE
{
	namespace nsRE
	{
		class PrimitiveShaderSet : public VertFragShaderSet
		{
		public:
			PrimitiveShaderSet(const CWString& vertShadPath, const CWString& fragShadPath);
			void set_vertexColorForFragment(const glm::vec3& primitiveVC) const;

			GLuint unif_vertexColorForFragment = 0;
		};

		class PrimitiveResourceModel : public ResourceModel
		{
		public:
			PrimitiveResourceModel() : ResourceModel() {}
			
			// 귀찮아서 일단 수동 등록만 가능하도록 해둠
			//PrimitiveResourceModel(const CString rmKey);

		protected:
			virtual void CreateWithData(const std::vector<glm::vec3>& vertices, GLuint drawType);
			virtual void NewAndRegisterRenderModel(const GLFWwindow* contextWindow) const override;
		};

		class PrimitiveRenderModel : public RenderModel
		{
		public:
			PrimitiveRenderModel(const ResourceModelBase& resModelBase, GLuint texSamp) : RenderModel(resModelBase, texSamp) {}
			PrimitiveShaderSet* GetPrimitiveShaderSet() const { return static_cast<PrimitiveShaderSet*>(vfShaderSet_); }

			virtual void CreateFromResource() override;

			void SetDrawType(GLuint glDrawType) { drawType_ = glDrawType; }

		protected:
			virtual void CallGLDraw(size_t verticesSize) const override;
			virtual void BeginRender() override;

			GLuint drawType_ = GL_LINES;
		};

		// 모든 PrimitiveResourceModel 들은 모델좌표계로 만들어져야한다, 그래서 direction 이나 centerPos 가 없는 것이다.
		class SphereResourceModel : public PrimitiveResourceModel
		{
		public:
			SphereResourceModel();
		};

		class CubeResourceModel : public PrimitiveResourceModel
		{
		public:
			CubeResourceModel();
		};

		// 라인으로 그려지는 쿼드
		class QuadResourceModel : public PrimitiveResourceModel
		{
		public:
			QuadResourceModel();
		};

		// 면으로 그려지는 쿼드
		class QuadFacedResourceModel : public PrimitiveResourceModel
		{
		public:
			QuadFacedResourceModel();
		};

		class CircleResourceModel : public PrimitiveResourceModel
		{
		public:
			CircleResourceModel(int32 circumferenceSegmentNumber);
		};
		class CircleFacedResourceModel : public PrimitiveResourceModel
		{
		public:
			CircleFacedResourceModel(int32 circumferenceSegmentNumber);
		};

		class PlaneResourceModel : public PrimitiveResourceModel
		{
		public:
			PlaneResourceModel();
		};

		class SegmentResourceModel : public PrimitiveResourceModel
		{
		public:
			SegmentResourceModel();
		};

		class PointResourceModel : public PrimitiveResourceModel
		{
		public:
			PointResourceModel();
		};
	}
}
