#pragma once

#include "RenderingEngine.h"

namespace SMGE
{
	namespace nsRE
	{
		class GizmoRM : public ResourceModel
		{
		public:
			GizmoRM() : ResourceModel() {}
		protected:
			virtual void CreateFrom(const std::vector<glm::vec3>& vertices);
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
		class PlaneRM : public GizmoRM
		{
		public:
			PlaneRM();
		};

		// 면으로 그려지는 평면
		class PlaneFacedRM : public GizmoRM
		{
		public:
			PlaneFacedRM();
		};
		
		class RayRM : public GizmoRM
		{
		public:
			RayRM();
		};

		class PointRM : public GizmoRM
		{
		public:
			PointRM();
		};
	}
}
