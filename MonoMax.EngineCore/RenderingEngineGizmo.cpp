#include "RenderingEngineGizmo.h"
#include "RenderingEngineUtils.hpp"
#include "../SMGE/CGameBase.h"
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

			auto gizmoVert = SMGE::Globals::GetEngineAssetPath(wtext("gizmo.vert")), gizmoFrag = SMGE::Globals::GetEngineAssetPath(wtext("gizmo.frag"));
			vfShaderSet_ = &VertFragShaderSet::FindOrLoadShaderSet(gizmoVert, gizmoFrag);

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

		CubeRSM::CubeRSM(const glm::vec3& size)
		{
			std::vector<glm::vec3> vertices;
			
			glm::vec3 centerPos(0);
			vertices = nsRE::makeSimpleCube3D_Line<glm::vec3>(centerPos, size);

			CreateFrom(vertices);
		}

		RayRSM::RayRSM(float size) : GizmoRSM()
		{
			std::vector<glm::vec3> vertices;
			vertices.reserve(2);

			vertices.emplace_back(0.f, 0.f, 0.f);

			glm::vec3 direction = TransformConst::GetModelRotateDirectionAxis();
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
