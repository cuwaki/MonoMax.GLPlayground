#pragma once

#include "RenderingEngine.h"

namespace SMGE
{
	namespace nsRE
	{
		class GizmoShaderSet : public VertFragShaderSet
		{
		public:
			GizmoShaderSet(const CWString& vertShadPath, const CWString& fragShadPath);
			void set_vertexColorForFragment(const glm::vec3& gizmoVC) const;

			GLuint unif_vertexColorForFragment = 0;
		};

		class GizmoResourceModel : public ResourceModel
		{
		public:
			GizmoResourceModel() : ResourceModel() {}

		protected:
			virtual void CreateFrom(const std::vector<glm::vec3>& vertices, GLuint drawType);
			virtual RenderModel* NewRenderModel(const GLFWwindow* contextWindow) const override;
		};

		class GizmoRenderModel : public RenderModel
		{
		public:
			GizmoRenderModel(const ResourceModelBase& asset, GLuint texSamp);
			GizmoShaderSet* GetGizmoShaderSet() const { return static_cast<GizmoShaderSet*>(vfShaderSet_); }

			GLuint drawType_ = GL_LINES;

		protected:
			virtual void CallGLDraw(size_t verticesSize) const override;
			virtual void BeginRender() override;
		};

		// 모든 GizmoResourceModel 들은 모델좌표계로 만들어져야한다, 그래서 direction 이나 centerPos 가 없는 것이다.
		class SphereResourceModel : public GizmoResourceModel
		{
		public:
			SphereResourceModel();
		};

		class CubeResourceModel : public GizmoResourceModel
		{
		public:
			CubeResourceModel();
		};

		// 라인으로 그려지는 평면
		class QuadResourceModel : public GizmoResourceModel
		{
		public:
			QuadResourceModel();
		};

		// 면으로 그려지는 평면
		class QuadFacedResourceModel : public GizmoResourceModel
		{
		public:
			QuadFacedResourceModel();
		};

		class CircleResourceModel : public GizmoResourceModel
		{
		public:
			CircleResourceModel(int32 circumferenceSegmentNumber);
		};
		class CircleFacedResourceModel : public GizmoResourceModel
		{
		public:
			CircleFacedResourceModel(int32 circumferenceSegmentNumber);
		};

		class SegmentResourceModel : public GizmoResourceModel
		{
		public:
			SegmentResourceModel();
		};

		class PointResourceModel : public GizmoResourceModel
		{
		public:
			PointResourceModel();
		};
	}
}
