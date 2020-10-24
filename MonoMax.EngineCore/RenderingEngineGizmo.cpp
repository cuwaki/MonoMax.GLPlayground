#include "RenderingEngineGizmo.h"
#include "RenderingEngineUtils.hpp"
#include "../SMGE/CGameBase.h"
#include <vector>

namespace SMGE
{
	namespace nsRE
	{
		void GizmoRM::CreateFrom(const std::vector<glm::vec3>& vertices)
		{
			std::vector<glm::vec2> dummy2(0);
			std::vector<glm::vec3> dummy3(0);

			GetMesh().loadFromPlainData(vertices, dummy2, dummy3);

			auto gizmoVert = SMGE::Globals::GetEngineAssetPath(wtext("gizmo.vert")), gizmoFrag = SMGE::Globals::GetEngineAssetPath(wtext("gizmo.frag"));
			vfShaderSet_ = &VertFragShaderSet::FindOrLoadShaderSet(gizmoVert, gizmoFrag);

			CreateRenderModel();

			GetRenderModel().GenOpenGLBuffers(vertices, dummy2, dummy3, dummy3);
		}

		SphereRM::SphereRM() : GizmoRM()
		{
			const auto divides = 36;

			std::vector<glm::vec3> vertices;
			vertices.reserve(divides);

			vertices = nsRE::makeSimpleSphere3D_Line<glm::vec3>(divides, 0.5f);

			CreateFrom(vertices);
		}

		CubeRM::CubeRM()
		{
			std::vector<glm::vec3> vertices;
			
			glm::vec3 centerPos(0);
			vertices = nsRE::makeSimpleCube3D_Line<glm::vec3>(centerPos, glm::vec3(1.f));

			CreateFrom(vertices);
		}

		PlaneFacedRM::PlaneFacedRM() : GizmoRM()
		{
			std::vector<glm::vec3> vertices;
			vertices.reserve(6);

			// 평면이므로 두께가 없다, 가로세로 길이가 1인 평면의 면을 삼각형 두개로 만든다
			glm::vec2 xyOrigin(0), xySize(1);

			auto xySizeHalf = xySize / 2.f;

			auto xyTri1Base = xyOrigin - xySizeHalf;
			vertices.push_back({ xyTri1Base, 0.f });	// -0.5, -0.5, 0
			vertices.push_back({ xyTri1Base + xySize, 0.f });	// 0.5, 0.5, 0
			vertices.push_back({ xyTri1Base + glm::vec2{ 0.f, 1.f }, 0.f });	// -0.5, 0.5, 0

			auto xyTri2Base = xyOrigin + xySizeHalf;
			vertices.push_back({ xyTri2Base, 0.f });	// 0.5, 0.5, 0
			vertices.push_back({ xyTri2Base - xySize, 0.f });	// -0.5, -0.5, 0
			vertices.push_back({ xyTri2Base + glm::vec2{ 0.f, -1.f }, 0.f });	// 0.5, -0.5, 0

			CreateFrom(vertices);
		}

		PlaneRM::PlaneRM() : GizmoRM()
		{
			std::vector<glm::vec3> vertices;
			vertices.reserve(4);

			// 평면이므로 두께가 없다, 가로세로 길이가 1인 평면의 외곽선을 만든다
			glm::vec2 xyOrigin(0), xySize(1);

			auto xySizeHalf = xySize / 2.f;

			auto xyTri1Base = xyOrigin - xySizeHalf;
			vertices.push_back({ xyTri1Base, 0.f });	// -0.5, -0.5, 0
			vertices.push_back({ xyTri1Base + glm::vec2{ 1.f, 0.f }, 0.f });	// 0.5, -0.5, 0
			vertices.push_back({ xyTri1Base + glm::vec2{ 1.f, 1.f }, 0.f });	// 0.5, 0.5, 0
			vertices.push_back({ xyTri1Base + glm::vec2{ 0.f, 1.f }, 0.f });	// -0.5, 0.5, 0

			CreateFrom(vertices);
		}

		RayRM::RayRM() : GizmoRM()
		{
			std::vector<glm::vec3> vertices;
			vertices.reserve(2);

			vertices.emplace_back(0.f, 0.f, 0.f);

			glm::vec3 direction = TransformConst::DefaultModelFrontAxis();
			vertices.emplace_back(glm::normalize(direction) * 1.f);

			CreateFrom(vertices);
		}

		PointRM::PointRM()
		{
			// 문제 - 점이 제대로 안그려진다
			//std::vector<glm::vec3> vertices;
			//vertices.reserve(1);
			//vertices.emplace_back(0.f, 0.f, 0.f);
			//CreateFrom(vertices);

			const auto divides = 5;

			std::vector<glm::vec3> vertices;
			vertices.reserve(divides);

			vertices = nsRE::makeSimpleSphere3D_Line<glm::vec3>(divides, 0.1f);	// 임시 해결이다, 위의 문제를 해결하라!

			CreateFrom(vertices);
		}
	}
}
