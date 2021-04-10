#include "CPlaneComponent.h"
#include "../CGameBase.h"
#include "../CEngineBase.h"
#include "../Objects/CActor.h"
#include "../../MonoMax.EngineCore/RenderingEngineGizmo.h"

namespace SMGE
{
	CPlaneComponent::CPlaneComponent(CObject* outer) : Super(outer)
	{
		Ctor();
	}

	void CPlaneComponent::Ctor()
	{
		isGameRendering_ = false;
#if IS_EDITOR
		isEditorRendering_ = true;
#endif
		boundType_ = EBoundType::PLANE;

		// 평면는 X 와 Y 로만 만들어져야한다, Z 는 Configs::BoundEpsilon 로 고정이거나 마치 0처럼 취급될 것이다
	}

	void CPlaneComponent::ReadyToDrawing()
	{
		const auto resmKey = "gizmoK:plane";

		auto gizmorm = nsRE::CResourceModelProvider::FindResourceModel(resmKey);
		if (gizmorm == nullptr)
			gizmorm = nsRE::CResourceModelProvider::AddResourceModel(resmKey, std::make_shared<nsRE::PlaneResourceModel>());

		gizmorm->GetRenderModel(nullptr)->AddWorldObject(this);

		Super::ReadyToDrawing();
	}

	glm::vec3 CPlaneComponent::getNormal() const
	{
		return GetFinalFront();
	}

	const SBound& CPlaneComponent::GetBound()
	{
		RecalcFinal();	// 여기 - 여길 막으려면 dirty 에서 미리 캐시해놓는 시스템을 만들고, 그걸로 안될 때는 바깥쪽에서 리칼크를 불러줘야한다

		// 평면는 X 와 Y 로만 만들어져야한다, Z 는 Configs::BoundEpsilon 로 고정이거나 마치 0처럼 취급될 것이다

		PlaneBound_ = SPlaneBound(getNormal(), GetFinalPosition());
		return PlaneBound_;
	}

	void CPlaneComponent::SetBound(const glm::vec3& ccw_p0, const glm::vec3& ccw_p1, const glm::vec3& ccw_p2)
	{
		const auto& worldLoc = GetFinalPosition();

		// ccw_p0 와 같이 이름 그대로 평면을 정의하는 점이 사각형의 순서대로 들어왔다면
		// ccw_p2 - ccw_p0 의 중점을 평면의 center 로 삼을 수 있고, 이것은 크기를 제한할 경우 SQuadBound 와 같이 사용할 수도 있게 된다
		const auto center = ccw_p0 + (ccw_p2 - ccw_p0) / 2.f;
		Translate(center);

		const auto normal = glm::normalize(glm::cross((ccw_p1 - ccw_p0), (ccw_p2 - ccw_p0)));
#ifdef REFACTORING_TRNASFORM
		// 여기 - 안급함 - 평면을 구성하는 라인을 따라서 up을 결정해서 넣어주는 것이 좋겠다
		RotateDirection(normal);
#else
		RotateQuat(normal);
#endif
		// 테스트 코드 - 리칼크파이널 코드 재검토 - RecalcFinal();// 여기 - SetBound 계열은 리칼크 해야하나??
		
		//auto pb = SPlaneBound(ccw_p0, ccw_p1, ccw_p2);
	}
};
