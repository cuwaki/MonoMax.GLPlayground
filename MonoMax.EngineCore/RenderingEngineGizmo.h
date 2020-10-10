#pragma once

#include "RenderingEngine.h"

namespace SMGE
{
	namespace nsRE
	{
		class GizmoRSM : public ResourceModel
		{
		public:
			GizmoRSM() : ResourceModel() {}
		protected:
			virtual void CreateFrom(const std::vector<glm::vec3>& vertices);
		};

		// 모든 GizmoRSM 들은 모델좌표계로 만들어져야한다, 그래서 direction 이나 centerPos 가 없는 것이다.
		class SphereRSM : public GizmoRSM
		{
		public:
			SphereRSM(float radius);
		};

		class CubeRSM : public GizmoRSM
		{
		public:
			CubeRSM(const glm::vec3& size);
		};

		class RayRSM : public GizmoRSM
		{
		public:
			RayRSM(float size);
		};

		class PointRSM : public GizmoRSM
		{
		public:
			PointRSM();
		};
	}
}
