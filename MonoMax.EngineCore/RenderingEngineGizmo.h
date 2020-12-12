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
			void set_vertexColorForFragment(const glm::vec3& gizmoVC);

			GLuint unif_vertexColorForFragment = 0;
		};

		class GizmoRM : public ResourceModel
		{
		public:
			GizmoRM() : ResourceModel() {}

		protected:
			virtual void CreateFrom(const std::vector<glm::vec3>& vertices);
			virtual void CallDefaultGLDraw(size_t verticesSize) const override;

			GizmoShaderSet* GetGizmoShaderSet() const
			{
				return static_cast<GizmoShaderSet *>(vfShaderSet_);
			}

			glm::vec3 gizmoColor_{ 1.f };
		};

		// 모든 GizmoRM 들은 모델좌표계로 만들어져야한다, 그래서 direction 이나 centerPos 가 없는 것이다.
		class SphereRM : public GizmoRM
		{
		public:
			SphereRM();
		};

		class CubeRM : public GizmoRM
		{
		public:
			CubeRM();
		};

		// 라인으로 그려지는 평면
		class QuadRM : public GizmoRM
		{
		public:
			QuadRM();
		};

		// 면으로 그려지는 평면
		class QuadFacedRM : public GizmoRM
		{
		public:
			QuadFacedRM();
		protected:
			virtual void CallDefaultGLDraw(size_t verticesSize) const override;
		};
		
		class SegmentRM : public GizmoRM
		{
		public:
			SegmentRM();
		};

		class PointRM : public GizmoRM
		{
		public:
			PointRM();

		protected:
			virtual void CallDefaultGLDraw(size_t verticesSize) const override;
		};
	}
}
