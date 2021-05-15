#include "CCubeComponent.h"
#include "../CGameBase.h"
#include "../CEngineBase.h"
#include "../Objects/CActor.h"
#include "../../MonoMax.EngineCore/RenderingEnginePrimitive.h"
#include <algorithm>

namespace SMGE
{
	using namespace nsRE::TransformConst;

	CCubeComponent::CCubeComponent(CObject* outer) : Super(outer)
	{
		Ctor();
	}

	void CCubeComponent::Ctor()
	{
		isGameRendering_ = false;
#if IS_EDITOR
		isEditorRendering_ = true;
#endif
		boundType_ = EBoundType::CUBE;
	}

	SGReflection& CCubeComponent::getReflection()
	{
		if (reflCubeCompo_.get() == nullptr)
			reflCubeCompo_ = std::make_unique<TReflectionStruct>(*this);
		return *reflCubeCompo_.get();
	}

	void CCubeComponent::ReadyToDrawing()
	{
		const auto resmKey = "primitiveK:cube";

		auto primitiveResM = nsRE::CResourceModelProvider::FindResourceModel(resmKey);
		if (primitiveResM == nullptr)
			primitiveResM = nsRE::CResourceModelProvider::AddResourceModel(resmKey, std::make_shared<nsRE::CubeResourceModel>());

		primitiveResM->GetRenderModel(nullptr)->AddWorldModel(this);

		Super::ReadyToDrawing();
	}

	const SBound& CCubeComponent::GetBoundWorldSpace(bool isForceRecalc)
	{
		// 여기 - 최적화 필요 - IsDirty() 가 해소되는 것이 액터의 AfterTick 인 현재 상황에서는 무조건 재계산을 해야한다
		//if (isForceRecalc || IsDirty())
		{
			//RecalcFinal();	

			cubeBound_ = SCubeBound(GetFinalPosition(), GetFinalScales(), 
				{
					GetFinalAxis(ETypeAxis::X), GetFinalAxis(ETypeAxis::Y), GetFinalAxis(ETypeAxis::Z)
				});
		}
		return cubeBound_;
	}
};
