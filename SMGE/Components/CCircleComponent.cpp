#include "CCircleComponent.h"
#include "../CGameBase.h"
#include "../CEngineBase.h"
#include "../Objects/CActor.h"
#include "../../MonoMax.EngineCore/RenderingEngineGizmo.h"

namespace SMGE
{
	CCircleComponent::CCircleComponent(CObject* outer) : Super(outer)
	{
		hasFace_ = true;	// 테스트 코드 - 임시로 눈에 잘 보이라고
		Ctor();
	}
	CCircleComponent::CCircleComponent(CObject* outer, bool hasFace) : CCircleComponent(outer)
	{
		hasFace_ = hasFace;
	}

	void CCircleComponent::Ctor()
	{
		isGameRendering_ = false;
#if IS_EDITOR
		isEditorRendering_ = true;
#endif
		boundType_ = EBoundType::CIRCLE;

		// 써클은 X 와 Y 로만 만들어져야한다, Z 는 Configs::BoundEpsilon 로 고정이거나 마치 0처럼 취급될 것이다
	}

	void CCircleComponent::ReadyToDrawing()
	{
		CString resmKey;
		std::shared_ptr<nsRE::ResourceModelBase> gizmorm;
		if (hasFace_)
		{
			resmKey = "gizmoK:circle_faced";
			gizmorm = nsRE::CResourceModelProvider::FindResourceModel(resmKey);
			if(gizmorm == nullptr)
				gizmorm = nsRE::CResourceModelProvider::AddResourceModel(resmKey, std::make_shared<nsRE::CircleFacedResourceModel>(SCircleBound::CIRCUMFERENCE_SEGMENT_MAX));
		}
		else
		{
			resmKey = "gizmoK:circle";
			gizmorm = nsRE::CResourceModelProvider::FindResourceModel(resmKey);
			if (gizmorm == nullptr)
				gizmorm = nsRE::CResourceModelProvider::AddResourceModel(resmKey, std::make_shared<nsRE::CircleResourceModel>(SCircleBound::CIRCUMFERENCE_SEGMENT_MAX));
		}

		gizmorm->GetRenderModel(nullptr)->AddWorldObject(this);

		Super::ReadyToDrawing();
	}

	glm::vec3 CCircleComponent::getNormal(bool isWorld) const
	{
		if (isWorld)
			return GetFinalFront();
		else
			return GetPendingFront();
	}

	const SBound& CCircleComponent::GetBoundWorldSpace(bool isForceRecalc)
	{
		if (isForceRecalc || IsDirty())
		{
			RecalcFinal();	// 여기 - 여길 막으려면 dirty 에서 미리 캐시해놓는 시스템을 만들고, 그걸로 안될 때는 바깥쪽에서 리칼크를 불러줘야한다

			// 써클은 X 와 Y 로만 만들어져야한다, Z 는 Configs::BoundEpsilon 로 고정이거나 마치 0처럼 취급될 것이다

			const auto center = GetFinalPosition();
			const auto halfSize = GetFinalScales() * 0.5f;
			circleBound_ = SCircleBound(getNormal(true), center, halfSize.x);
		}

		return circleBound_;
	}
};
