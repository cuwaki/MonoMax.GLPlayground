#include "CPointComponent.h"
#include "../CGameBase.h"
#include "../CEngineBase.h"
#include "../Objects/CActor.h"
#include "../../MonoMax.EngineCore/RenderingEnginePrimitive.h"
#include "CCubeComponent.h"

namespace SMGE
{
	CPointComponent::CPointComponent(CObject* outer) : CBoundComponent(outer)
	{
		Ctor();
	}

	void CPointComponent::Ctor()
	{
		isGameRendering_ = false;
#if IS_EDITOR
		isEditorRendering_ = true;
#endif

		boundType_ = EBoundType::POINT;
	}

	SGReflection& CPointComponent::getReflection()
	{
		if (reflPointCompo_.get() == nullptr)
			reflPointCompo_ = std::make_unique<TReflectionStruct>(*this);
		return *reflPointCompo_.get();
	}

	void CPointComponent::ReadyToDrawing()
	{
		const auto resmKey = "primitiveK:point";

		auto primitiveResM = nsRE::CResourceModelProvider::FindResourceModel(resmKey);
		if (primitiveResM == nullptr)
			primitiveResM = nsRE::CResourceModelProvider::AddResourceModel(resmKey, std::make_shared<nsRE::PointResourceModel>());

		primitiveResM->GetRenderModel(nullptr)->AddWorldModel(this);

		Super::ReadyToDrawing();
	}

	const SBound& CPointComponent::GetBoundWorldSpace(bool isForceRecalc)
	{
		// 여기 - 최적화 필요 - IsDirty() 가 해소되는 것이 액터의 AfterTick 인 현재 상황에서는 무조건 재계산을 해야한다
		//if (isForceRecalc || IsDirty())
		{
			//RecalcFinal();	

			pointBound_ = SPointBound(GetFinalPosition());
		}
		return pointBound_;
	}
};
