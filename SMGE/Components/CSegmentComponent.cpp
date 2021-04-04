#include "CSegmentComponent.h"
#include "../CGameBase.h"
#include "../CEngineBase.h"
#include "../Objects/CActor.h"
#include "../../MonoMax.EngineCore/RenderingEngineGizmo.h"
#include "CSphereComponent.h"
#include "CCubeComponent.h"

namespace SMGE
{
	CSegmentComponent::CSegmentComponent(CObject* outer) : Super(outer)
	{
		Ctor();
	}

	// 콤포넌트의 트랜스폼을 바꾼다, 보통은 이걸 안쓰고 부모의 트랜스폼이 반영된 것을 쓰게 될 것이다
	void CSegmentComponent::SetBoundDataComponent(float size, const glm::vec3& direction)
	{
		Scale(nsRE::TransformConst::DefaultAxis_Front, size);
#ifdef REFACTORING_TRNASFORM
		RotateDirection(glm::normalize(direction));
#else
		RotateQuat(glm::normalize(direction));
#endif
		// 테스트 코드 - 리칼크파이널 코드 재검토 - RecalcFinal();

		// 세그먼트는 Z 로만 만들어져야한다, X, Y 는 Configs::BoundEpsilon 로 고정이거나 마치 0처럼 취급될 것이다
	}

	float CSegmentComponent::getRayLength() const
	{
		return GetFinalScales()[nsRE::TransformConst::DefaultAxis_Front];
	}

	glm::vec3 CSegmentComponent::getRayDirection() const
	{
		return GetFinalFront();
	}

	void CSegmentComponent::Ctor()
	{
		isGameRendering_ = false;
#if IS_EDITOR
		isEditorRendering_ = true;
#endif
		boundType_ = EBoundType::SEGMENT;
	}

	void CSegmentComponent::OnBeginPlay(CObject* parent)
	{
		Super::OnBeginPlay(parent);
	}

	void CSegmentComponent::OnEndPlay()
	{
		Super::OnEndPlay();
	}

	CCubeComponent* CSegmentComponent::CreateOBB()
	{
		auto obb = CreateOBB();

		// 레이는 0에서 앞으로 뻗지만, OBB 는 큐브라서 중점에서 만들어지므로 Z축을 앵커로 잡아야한다
		assert(nsRE::TransformConst::DefaultAxis_Front == nsRE::TransformConst::ETypeAxis::Z);
		obb->Translate({ 0, 0, 0.5f });	// 단위크기니까 0.5로 하면 된다
		// 테스트 코드 - 리칼크파이널 코드 재검토 - obb->RecalcFinal();

		return obb;
	}

	const SBound& CSegmentComponent::GetBound()
	{
		RecalcFinal();	// 여기 - 여길 막으려면 dirty 에서 미리 캐시해놓는 시스템을 만들고, 그걸로 안될 때는 바깥쪽에서 리칼크를 불러줘야한다

		const auto start = GetFinalPosition();
		segBound_ = SSegmentBound(start, start + getRayDirection() * getRayLength());
		return segBound_;
	}

	SGReflection& CSegmentComponent::getReflection()
	{
		if (reflRayCompo_.get() == nullptr)
			reflRayCompo_ = std::make_unique<TReflectionStruct>(*this);

		// 세그먼트는 Z 로만 만들어져야한다, X, Y 는 Configs::BoundEpsilon 로 고정이거나 마치 0처럼 취급될 것이다

		return *reflRayCompo_.get();
	}

	void CSegmentComponent::ReadyToDrawing()
	{
		const auto resmKey = "gizmoK:segment";

		auto gizmorm = nsRE::CResourceModelProvider::FindResourceModel(resmKey);
		if (gizmorm == nullptr)
			gizmorm = nsRE::CResourceModelProvider::AddResourceModel(resmKey, std::make_shared<nsRE::SegmentResourceModel>());

		gizmorm->GetRenderModel(nullptr)->AddWorldObject(this);

		Super::ReadyToDrawing();
	}
};
