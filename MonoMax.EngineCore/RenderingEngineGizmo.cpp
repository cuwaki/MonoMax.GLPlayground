#include "RenderingEngineGizmo.h"
#include "RenderingEngineUtils.hpp"
#include <vector>

namespace SMGE
{
	namespace nsRE
	{
		void GizmoRSM::CreateFrom(const std::vector<glm::vec3>& vertices)
		{
			std::vector<glm::vec2> dummy2(0);
			std::vector<glm::vec3> dummy3(0);

			GetMesh().loadFromPlainData(vertices, dummy2, dummy3);
			GetShaderSet().VertFragShaderSet::VertFragShaderSet(wtext("e:/dev_project/gizmo.vert"), wtext("e:/dev_project/gizmo.frag"));
			CreateRenderModel();

			GetRenderModel().GenOpenGLBuffers(vertices, dummy2, dummy3, dummy3);
		}

		SphereRSM::SphereRSM(float radius) : GizmoRSM()
		{
			const auto divides = 36;

			std::vector<glm::vec3> vertices;
			vertices.reserve(divides);

			vertices = nsRE::makeSimpleSphere3D_Line<glm::vec3>(divides, radius);

			CreateFrom(vertices);
		}

		RayRSM::RayRSM(float size, const glm::vec3& direction) : GizmoRSM()
		{
			std::vector<glm::vec3> vertices;
			vertices.reserve(2);

			vertices.emplace_back(0.f, 0.f, 0.f);
			vertices.emplace_back(glm::normalize(direction) * size);

			CreateFrom(vertices);
		}

		PointRSM::PointRSM()
		{
			// 문제 - 점이 제대로 안그려진다
			//std::vector<glm::vec3> vertices;
			//vertices.reserve(1);
			//vertices.emplace_back(0.f, 0.f, 0.f);
			//CreateFrom(vertices);

			const auto divides = 5;

			std::vector<glm::vec3> vertices;
			vertices.reserve(divides);

			vertices = nsRE::makeSimpleSphere3D_Line<glm::vec3>(divides, 0.1f);

			CreateFrom(vertices);
		}
	}
}
