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

		class SphereRSM : public GizmoRSM
		{
		public:
			SphereRSM(float radius);
		};

		class RayRSM : public GizmoRSM
		{
		public:
			RayRSM(float size, const glm::vec3& direction);
		};

		class PointRSM : public GizmoRSM
		{
		public:
			PointRSM();
		};
	}
}
