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
		isGameVisible_ = false;
#if IS_EDITOR
		isEditorVisible_ = true;
#endif
		boundType_ = EBoundType::PLANE;

		// 평면는 X 와 Y 로만 만들어져야한다, Z 는 Configs::BoundEpsilon 로 고정이거나 마치 0처럼 취급될 것이다
	}

	void CPlaneComponent::ReadyToDrawing()
	{
		Super::ReadyToDrawing();

		// 아무것도 그리지 않음 - 나중에 축이나 그리든가
	}

	glm::vec3 CPlaneComponent::getNormal() const
	{
		return GetWorldFront();
	}

	const SBound& CPlaneComponent::GetBound()
	{
		RecalcMatrix();	// 여기 - 여길 막으려면 dirty 에서 미리 캐시해놓는 시스템을 만들고, 그걸로 안될 때는 바깥쪽에서 리칼크를 불러줘야한다

		// 평면는 X 와 Y 로만 만들어져야한다, Z 는 Configs::BoundEpsilon 로 고정이거나 마치 0처럼 취급될 것이다

		PlaneBound_ = SPlaneBound(getNormal(), GetWorldPosition());
		return PlaneBound_;
	}

	void CPlaneComponent::SetBound(const glm::vec3& ccw_p0, const glm::vec3& ccw_p1, const glm::vec3& ccw_p2)
	{
		// ccw_p0 와 같이 이름 그대로 평면을 정의하는 점이 순서대로 들어왔다면
		// ccw_p2 - ccw_p0 의 중점을 평면의 center 로 삼을 수 있고, 이것은 크기를 제한할 경우 SQuadBound 와 같이 사용할 수도 있게 된다
		const auto center = (ccw_p2 - ccw_p0) / 2.f;
		Translate(center);

		const auto normal = glm::normalize(glm::cross((ccw_p1 - ccw_p0), (ccw_p2 - ccw_p0)));
		RotateQuat(normal);
		
		//auto pb = SPlaneBound(ccw_p0, ccw_p1, ccw_p2);
	}
};
