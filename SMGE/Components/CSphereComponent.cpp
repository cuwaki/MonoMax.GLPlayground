#include "CSphereComponent.h"
#include "../CGameBase.h"
#include "../CEngineBase.h"
#include "../Objects/CActor.h"
#include "../../MonoMax.EngineCore/RenderingEnginePrimitive.h"

namespace SMGE
{
	CSphereComponent::CSphereComponent(CObject* outer) : CBoundComponent(outer)
	{
		Ctor();
	}

	void CSphereComponent::Ctor()
	{
		isGameRendering_ = false;
#if IS_EDITOR
		isEditorRendering_ = true;
#endif
		boundType_ = EBoundType::SPHERE;
	}

	SGReflection& CSphereComponent::getReflection()
	{
		if (reflSphereCompo_.get() == nullptr)
			reflSphereCompo_ = std::make_unique<TReflectionStruct>(*this);
		return *reflSphereCompo_.get();
	}

	void CSphereComponent::ReadyToDrawing()
	{
		const auto resmKey = "primitiveK:sphere";

		auto primitiveResM = nsRE::CResourceModelProvider::FindResourceModel(resmKey);
		if (primitiveResM == nullptr)
			primitiveResM = nsRE::CResourceModelProvider::AddResourceModel(resmKey, std::make_shared<nsRE::SphereResourceModel>());

		primitiveResM->GetRenderModel(nullptr)->AddWorldModel(this);

		Super::ReadyToDrawing();
	}

	float CSphereComponent::GetRadius(bool isWorld) const
	{
		if(isWorld)
			return GetFinalScales()[nsRE::TransformConst::DefaultAxis_Front] * 0.5f;	// 반지름이니까
		else
			return GetPendingScales()[nsRE::TransformConst::DefaultAxis_Front] * 0.5f;	// 반지름이니까
	}

	const SBound& CSphereComponent::GetBoundWorldSpace(bool isForceRecalc)
	{
		// 여기 - 최적화 필요 - IsDirty() 가 해소되는 것이 액터의 AfterTick 인 현재 상황에서는 무조건 재계산을 해야한다
		//if (isForceRecalc || IsDirty())
		{
			//RecalcFinal();	

			sphereBound_ = SSphereBound(GetFinalPosition(), GetRadius(true));
		}
		return sphereBound_;
	}
};
