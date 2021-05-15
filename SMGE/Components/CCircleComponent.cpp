#include "CCircleComponent.h"
#include "../CGameBase.h"
#include "../CEngineBase.h"
#include "../Objects/CActor.h"
#include "../../MonoMax.EngineCore/RenderingEnginePrimitive.h"

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
		std::shared_ptr<nsRE::ResourceModelBase> primitiveResM;
		if (hasFace_)
		{
			resmKey = "primitiveK:circle_faced";
			primitiveResM = nsRE::CResourceModelProvider::FindResourceModel(resmKey);
			if(primitiveResM == nullptr)
				primitiveResM = nsRE::CResourceModelProvider::AddResourceModel(resmKey, std::make_shared<nsRE::CircleFacedResourceModel>(SCircleBound::CIRCUMFERENCE_SEGMENT_MAX));
		}
		else
		{
			resmKey = "primitiveK:circle";
			primitiveResM = nsRE::CResourceModelProvider::FindResourceModel(resmKey);
			if (primitiveResM == nullptr)
				primitiveResM = nsRE::CResourceModelProvider::AddResourceModel(resmKey, std::make_shared<nsRE::CircleResourceModel>(SCircleBound::CIRCUMFERENCE_SEGMENT_MAX));
		}

		primitiveResM->GetRenderModel(nullptr)->AddWorldObject(this);

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
		// 여기 - 최적화 필요 - IsDirty() 가 해소되는 것이 액터의 AfterTick 인 현재 상황에서는 무조건 재계산을 해야한다
		//if (isForceRecalc || IsDirty())
		{
			//RecalcFinal();	

			// 써클은 X 와 Y 로만 만들어져야한다, Z 는 Configs::BoundEpsilon 로 고정이거나 마치 0처럼 취급될 것이다

			const auto center = GetFinalPosition();
			const auto halfSize = GetFinalScales() * 0.5f;
			circleBound_ = SCircleBound(getNormal(true), center, halfSize.x);
		}

		return circleBound_;
	}
};
