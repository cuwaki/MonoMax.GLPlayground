#include "CSegmentComponent.h"
#include "../CGameBase.h"
#include "../CEngineBase.h"
#include "../Objects/CActor.h"
#include "../../MonoMax.EngineCore/RenderingEnginePrimitive.h"
#include "CSphereComponent.h"
#include "CCubeComponent.h"

namespace SMGE
{
	CSegmentComponent::CSegmentComponent(CObject* outer) : Super(outer)
	{
		Ctor();
	}

	void CSegmentComponent::Ctor()
	{
		isGameRendering_ = false;
#if IS_EDITOR
		isEditorRendering_ = true;
#endif
		boundType_ = EBoundType::SEGMENT;
	}

	void CSegmentComponent::ReadyToDrawing()
	{
		const auto resmKey = "primitiveK:segment";

		auto primitiveResM = nsRE::CResourceModelProvider::FindResourceModel(resmKey);
		if (primitiveResM == nullptr)
			primitiveResM = nsRE::CResourceModelProvider::AddResourceModel(resmKey, std::make_shared<nsRE::SegmentResourceModel>());

		primitiveResM->GetRenderModel(nullptr)->AddWorldModel(this);

		Super::ReadyToDrawing();
	}

	float CSegmentComponent::getLength(bool isWorld) const
	{
		if (isWorld)
			return GetFinalScales()[nsRE::TransformConst::DefaultAxis_Front];
		else
			return GetPendingScales()[nsRE::TransformConst::DefaultAxis_Front];
	}

	glm::vec3 CSegmentComponent::getDirection(bool isWorld) const
	{
		if(isWorld)
			return GetFinalFront();
		else
			return GetPendingFront();
	}

	CCubeComponent* CSegmentComponent::CreateOBB()
	{
		auto obb = CreateOBB();

		// 레이는 0에서 앞으로 뻗지만, OBB 는 큐브라서 중점에서 만들어지므로 Z축을 앵커로 잡아야한다
		assert(nsRE::TransformConst::DefaultAxis_Front == nsRE::TransformConst::ETypeAxis::Z);
		obb->Translate({ 0, 0, 0.5f });	// 단위크기니까 0.5로 하면 된다

		return obb;
	}

	// 콤포넌트의 트랜스폼을 바꾼다, 보통은 이걸 안쓰고 부모의 트랜스폼이 반영된 것을 쓰게 될 것이다
	void CSegmentComponent::SetBoundLocalSpace(float size, const glm::vec3& direction)
	{
		Scale(nsRE::TransformConst::DefaultAxis_Front, size);
#ifdef REFACTORING_TRNASFORM
		RotateDirection(glm::normalize(direction));
#else
		RotateQuat(glm::normalize(direction));
#endif

		// 세그먼트는 Z 로만 만들어져야한다, X, Y 는 Configs::BoundEpsilon 로 고정이거나 마치 0처럼 취급될 것이다
	}

	const SBound& CSegmentComponent::GetBoundWorldSpace(bool isForceRecalc)
	{
		// 여기 - 최적화 필요 - IsDirty() 가 해소되는 것이 액터의 AfterTick 인 현재 상황에서는 무조건 재계산을 해야한다
		//if (isForceRecalc || IsDirty())
		{
			//RecalcFinal();	

			const auto start = GetFinalPosition();
			const auto dir = getDirection(true);
			segBound_ = SSegmentBound(start, start + dir * getLength(true));
		}
		return segBound_;
	}

	SGReflection& CSegmentComponent::getReflection()
	{
		if (reflRayCompo_.get() == nullptr)
			reflRayCompo_ = std::make_unique<TReflectionStruct>(*this);

		// 세그먼트는 Z 로만 만들어져야한다, X, Y 는 Configs::BoundEpsilon 로 고정이거나 마치 0처럼 취급될 것이다

		return *reflRayCompo_.get();
	}
};
