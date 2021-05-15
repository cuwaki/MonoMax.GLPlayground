#include "RenderingEnginePrimitive.h"
#include "RenderingEngineUtils.hpp"
#include "../SMGE/CGameBase.h"
#include <vector>

namespace SMGE
{
	namespace nsRE
	{
		///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
		PrimitiveShaderSet::PrimitiveShaderSet(const CWString& vertShadPath, const CWString& fragShadPath) : VertFragShaderSet(vertShadPath, fragShadPath)
		{
			unif_vertexColorForFragment = glGetUniformLocation(programID_, "vertexColorForFragment");
		}

		void PrimitiveShaderSet::set_vertexColorForFragment(const glm::vec3& primitiveVC) const
		{
			if (unif_vertexColorForFragment != -1)
				glUniform3f(unif_vertexColorForFragment, primitiveVC.r, primitiveVC.g, primitiveVC.b);
		}

		///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
		void PrimitiveResourceModel::CreateWithData(const std::vector<glm::vec3>& vertices, GLuint drawType)
		{
			vertShaderPath_ = SMGE::Globals::GetEngineAssetPath(wtext("primitive.vert"));
			fragShaderPath_ = SMGE::Globals::GetEngineAssetPath(wtext("primitive.frag"));

			std::vector<glm::vec2> dummy2(0);
			std::vector<glm::vec3> dummy3(0);

			GetMesh().loadFromPlainData(vertices, dummy2, dummy3);

			auto gizRendM = static_cast<PrimitiveRenderModel*>(GetRenderModel(nullptr));
			gizRendM->SetDrawType(drawType);
		}

		void PrimitiveResourceModel::NewAndRegisterRenderModel(const GLFWwindow* contextWindow) const
		{
			auto newOne = std::make_unique<PrimitiveRenderModel>(*this, 0);
			newOne->CreateFromResource();
			renderModelsPerContext_.insert(std::make_pair(contextWindow, std::move(newOne)));
		}

		void PrimitiveRenderModel::CreateFromResource()
		{
			const auto& gizResM = static_cast<const PrimitiveResourceModel&>(resourceModel_);
			vfShaderSet_ = VertFragShaderSet::FindOrLoadShaderSet<PrimitiveShaderSet>(gizResM.GetVertShaderPath(), gizResM.GetFragShaderPath());

			const auto& gizMesh = gizResM.GetMesh();
			GenGLMeshDatas(gizMesh.vertices_, gizMesh.uvs_, gizMesh.normals_, gizMesh.vertexColors_);
		}

		void PrimitiveRenderModel::BeginRender()
		{
			RenderModel::BeginRender();

			if (drawType_ == GL_POINTS)
				glEnable(GL_PROGRAM_POINT_SIZE);
			else if (drawType_ == GL_LINES)
				glLineWidth(1.f);
		}

		void PrimitiveRenderModel::CallGLDraw(size_t verticesSize) const
		{
			glDrawArrays(drawType_, 0, verticesSize);
		}

		SphereResourceModel::SphereResourceModel() : PrimitiveResourceModel()
		{
			const auto divides = 36;

			std::vector<glm::vec3> vertices;
			vertices = nsRE::makeSimpleSphere3D_Lines<glm::vec3>(divides, 0.5f);

			CreateWithData(vertices, GL_LINES);
		}

		CubeResourceModel::CubeResourceModel()
		{
			std::vector<glm::vec3> vertices;
			
			glm::vec3 centerPos(0);
			vertices = nsRE::makeSimpleCube3D_Lines<glm::vec3>(centerPos, glm::vec3(1.f));

			CreateWithData(vertices, GL_LINES);
		}

		QuadFacedResourceModel::QuadFacedResourceModel() : PrimitiveResourceModel()
		{
			std::vector<glm::vec3> vertices;

			// 쿼드이므로 두께가 없다, 가로세로 길이가 1인 쿼드의 면을 삼각형 두개로 만든다
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

			CreateWithData(vertices, GL_TRIANGLES);
		}

		QuadResourceModel::QuadResourceModel() : PrimitiveResourceModel()
		{
			std::vector<glm::vec3> vertices;

			// 쿼드이므로 두께가 없다, 가로세로 길이가 1인 쿼드의 외곽선을 만든다
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

			CreateWithData(vertices, GL_LINES);
		}

		CircleResourceModel::CircleResourceModel(int32 circumferenceSegmentNumber)
		{
			auto circleLines = makeCircle2DXY_Lines<glm::vec3>(circumferenceSegmentNumber, 0.5f);	// XY평면의 2D 써클
			CreateWithData(circleLines, GL_LINES);
		}

		CircleFacedResourceModel::CircleFacedResourceModel(int32 circumferenceSegmentNumber)
		{
			auto circleLines = makeCircle2DXY_Faced<glm::vec3>(circumferenceSegmentNumber, 0.5f);	// XY평면의 2D 써클
			CreateWithData(circleLines, GL_TRIANGLE_FAN);
		}

		PlaneResourceModel::PlaneResourceModel() : PrimitiveResourceModel()
		{
			glm::vec3 direction = TransformConst::DefaultModelFrontAxis();

			std::vector<glm::vec3> vertices;

			vertices.emplace_back(0.f, 0.f, 0.f);	// 시점
			vertices.emplace_back(glm::normalize(direction) * 1.f);	// 종점

			CreateWithData(vertices, GL_LINES);
		}

		SegmentResourceModel::SegmentResourceModel() : PrimitiveResourceModel()
		{
			glm::vec3 direction = TransformConst::DefaultModelFrontAxis();

			std::vector<glm::vec3> vertices;

			vertices.emplace_back(0.f, 0.f, 0.f);	// 시점
			vertices.emplace_back(glm::normalize(direction) * 1.f);	// 종점

			CreateWithData(vertices, GL_LINES);
		}

		PointResourceModel::PointResourceModel()
		{
			std::vector<glm::vec3> vertices;

			vertices.emplace_back(0.f, 0.f, 0.f);
			CreateWithData(vertices, GL_POINTS);
		}
	}
}
