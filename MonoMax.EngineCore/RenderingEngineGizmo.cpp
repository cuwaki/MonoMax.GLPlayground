#include "RenderingEngineGizmo.h"
#include "RenderingEngineUtils.hpp"
#include "../SMGE/CGameBase.h"
#include <vector>

namespace SMGE
{
	namespace nsRE
	{
		///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
		GizmoShaderSet::GizmoShaderSet(const CWString& vertShadPath, const CWString& fragShadPath) : VertFragShaderSet(vertShadPath, fragShadPath)
		{
			unif_vertexColorForFragment = glGetUniformLocation(programID_, "vertexColorForFragment");
		}

		void GizmoShaderSet::set_vertexColorForFragment(const glm::vec3& gizmoVC)
		{
			if (unif_vertexColorForFragment != -1)
				glUniform3f(unif_vertexColorForFragment, gizmoVC.r, gizmoVC.g, gizmoVC.b);
		}

		///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
		void GizmoRM::CreateFrom(const std::vector<glm::vec3>& vertices)
		{
			std::vector<glm::vec2> dummy2(0);
			std::vector<glm::vec3> dummy3(0);

			GetMesh().loadFromPlainData(vertices, dummy2, dummy3);

			auto gizmoVert = SMGE::Globals::GetEngineAssetPath(wtext("gizmo.vert")),
				 gizmoFrag = SMGE::Globals::GetEngineAssetPath(wtext("gizmo.frag"));
			
			vfShaderSet_ = VertFragShaderSet::FindOrLoadShaderSet<GizmoShaderSet>(gizmoVert, gizmoFrag);

			NewRenderModel();

			GetRenderModel().GenOpenGLBuffers(vertices, dummy2, dummy3, dummy3);
		}

		void GizmoRM::CallDefaultGLDraw(size_t verticesSize) const
		{
			glLineWidth(1.f);	// 여기 - 기즈모 렌더링 시스템으로 통합해야함
			GetGizmoShaderSet()->set_vertexColorForFragment(gizmoColor_);
			glDrawArrays(GL_LINES, 0, verticesSize);
		}

		SphereRM::SphereRM() : GizmoRM()
		{
			const auto divides = 36;

			std::vector<glm::vec3> vertices;
			vertices = nsRE::makeSimpleSphere3D_Lines<glm::vec3>(divides, 0.5f);

			CreateFrom(vertices);
		}

		CubeRM::CubeRM()
		{
			std::vector<glm::vec3> vertices;
			
			glm::vec3 centerPos(0);
			vertices = nsRE::makeSimpleCube3D_Lines<glm::vec3>(centerPos, glm::vec3(1.f));

			CreateFrom(vertices);
		}

		QuadFacedRM::QuadFacedRM() : GizmoRM()
		{
			std::vector<glm::vec3> vertices;

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

		void QuadFacedRM::CallDefaultGLDraw(size_t verticesSize) const
		{
			GetGizmoShaderSet()->set_vertexColorForFragment(gizmoColor_);
			glDrawArrays(GL_TRIANGLES, 0, verticesSize);
		}

		QuadRM::QuadRM() : GizmoRM()
		{
			std::vector<glm::vec3> vertices;

			// 평면이므로 두께가 없다, 가로세로 길이가 1인 평면의 외곽선을 만든다
			glm::vec2 xyOrigin(0), xySize(1);

			auto xySizeHalf = xySize / 2.f;

			// GL_LINES 를 위하여
			auto xyTri1Base = xyOrigin - xySizeHalf;
			vertices.push_back({ xyTri1Base, 0.f });	// -0.5, -0.5, 0
			vertices.push_back({ xyTri1Base + glm::vec2{ 1.f, 0.f }, 0.f });	// 0.5, -0.5, 0

			vertices.push_back({ xyTri1Base + glm::vec2{ 1.f, 0.f }, 0.f });	// 0.5, -0.5, 0
			vertices.push_back({ xyTri1Base + glm::vec2{ 1.f, 1.f }, 0.f });	// 0.5, 0.5, 0

			vertices.push_back({ xyTri1Base + glm::vec2{ 1.f, 1.f }, 0.f });	// 0.5, 0.5, 0
			vertices.push_back({ xyTri1Base + glm::vec2{ 0.f, 1.f }, 0.f });	// -0.5, 0.5, 0

			vertices.push_back({ xyTri1Base + glm::vec2{ 0.f, 1.f }, 0.f });	// -0.5, 0.5, 0
			vertices.push_back({ xyTri1Base, 0.f });	// -0.5, -0.5, 0

			CreateFrom(vertices);
		}

		SegmentRM::SegmentRM() : GizmoRM()
		{
			glm::vec3 direction = TransformConst::DefaultModelFrontAxis();

			std::vector<glm::vec3> vertices;

			vertices.emplace_back(0.f, 0.f, 0.f);	// 시점
			vertices.emplace_back(glm::normalize(direction) * 1.f);	// 종점

			CreateFrom(vertices);
		}

		PointRM::PointRM()
		{
			glEnable(GL_PROGRAM_POINT_SIZE);	// 여기 - 기즈모 렌더링 시스템으로 통합해야함

			std::vector<glm::vec3> vertices;

			vertices.emplace_back(0.f, 0.f, 0.f);
			CreateFrom(vertices);
		}

		void PointRM::CallDefaultGLDraw(size_t verticesSize) const
		{
			GetGizmoShaderSet()->set_vertexColorForFragment(gizmoColor_);
			glDrawArrays(GL_POINTS, 0, verticesSize);
		}
	}
}
