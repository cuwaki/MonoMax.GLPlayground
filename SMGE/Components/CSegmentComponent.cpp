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
		RotateQuat(glm::normalize(direction));

		// 세그먼트는 Z 로만 만들어져야한다, X, Y 는 Configs::BoundEpsilon 로 고정이거나 마치 0처럼 취급될 것이다

		RecalcMatrix();
	}

	float CSegmentComponent::getRayLength() const
	{
		return GetWorldScales()[nsRE::TransformConst::DefaultAxis_Front];
	}

	glm::vec3 CSegmentComponent::getRayDirection() const
	{
		return GetWorldFront();
	}

	void CSegmentComponent::Ctor()
	{
		isGameVisible_ = false;
#if IS_EDITOR
		isEditorVisible_ = true;
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

		return obb;
	}

	const SBound& CSegmentComponent::GetBound()
	{
		RecalcMatrix();	// 여기 - 여길 막으려면 dirty 에서 미리 캐시해놓는 시스템을 만들고, 그걸로 안될 때는 바깥쪽에서 리칼크를 불러줘야한다

		const auto start = GetWorldPosition();
		segBound_ = SSegmentBound(start, start + getRayDirection() * getRayLength());
		return segBound_;
	}

	SGReflection& CSegmentComponent::getReflection()
	{
		if (reflRayCompo_.get() == nullptr)
			reflRayCompo_ = MakeUniqPtr<TReflectionStruct>(*this);

		// 세그먼트는 Z 로만 만들어져야한다, X, Y 는 Configs::BoundEpsilon 로 고정이거나 마치 0처럼 취급될 것이다

		return *reflRayCompo_.get();
	}

	void CSegmentComponent::ReadyToDrawing()
	{
		const auto resmKey = "gizmoK:ray";

		auto gizmorm = nsRE::CResourceModelProvider::FindResourceModel(resmKey);
		if (gizmorm == nullptr)
		{
			nsRE::CResourceModelProvider::AddResourceModel(resmKey, std::move(new nsRE::SegmentRM()));	// 여기 수정 - 이거 CResourceModel 로 내리든가, 게임엔진에서 렌더링을 하도록 하자
			gizmorm = nsRE::CResourceModelProvider::FindResourceModel(resmKey);
		}

		gizmorm->GetRenderModel(nullptr)->AddWorldObject(this);

		Super::ReadyToDrawing();
	}
};
