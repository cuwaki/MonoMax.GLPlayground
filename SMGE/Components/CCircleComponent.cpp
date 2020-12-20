#include "CCircleComponent.h"
#include "../CGameBase.h"
#include "../CEngineBase.h"
#include "../Objects/CActor.h"
#include "../../MonoMax.EngineCore/RenderingEngineGizmo.h"

namespace SMGE
{
	CCircleComponent::CCircleComponent(CObject* outer) : Super(outer)
	{
		//hasFace_ = true;	// 테스트 코드
		Ctor();
	}
	CCircleComponent::CCircleComponent(CObject* outer, bool hasFace) : CCircleComponent(outer)
	{
		hasFace_ = hasFace;
	}

	void CCircleComponent::Ctor()
	{
		isGameVisible_ = false;
#if IS_EDITOR
		isEditorVisible_ = true;
#endif
		boundType_ = EBoundType::CIRCLE;

		// 써클은 X 와 Y 로만 만들어져야한다, Z 는 Configs::BoundEpsilon 로 고정이거나 마치 0처럼 취급될 것이다
	}

	void CCircleComponent::OnBeginPlay(CObject* parent)
	{
		Super::OnBeginPlay(parent);
	}

	void CCircleComponent::OnEndPlay()
	{
		Super::OnEndPlay();
	}

	void CCircleComponent::ReadyToDrawing()
	{
		CString resmKey;
		nsRE::ResourceModelBase* gizmorm = nullptr;
		if (hasFace_)
		{
			resmKey = "gizmoK:circle_faced";
			gizmorm = nsRE::CResourceModelProvider::FindResourceModel(resmKey);
			if (gizmorm == nullptr)
			{
				nsRE::CResourceModelProvider::AddResourceModel(resmKey, std::move(new nsRE::CircleFacedResourceModel(SCircleBound::CIRCUMFERENCE_SEGMENT_MAX)));	// 여기 수정 - 이거 CResourceModel 로 내리든가, 게임엔진에서 렌더링을 하도록 하자
				gizmorm = nsRE::CResourceModelProvider::FindResourceModel(resmKey);
			}
		}
		else
		{
			resmKey = "gizmoK:circle";
			gizmorm = nsRE::CResourceModelProvider::FindResourceModel(resmKey);
			if (gizmorm == nullptr)
			{
				nsRE::CResourceModelProvider::AddResourceModel(resmKey, std::move(new nsRE::CircleResourceModel(SCircleBound::CIRCUMFERENCE_SEGMENT_MAX)));	// 여기 수정 - 이거 CResourceModel 로 내리든가, 게임엔진에서 렌더링을 하도록 하자
				gizmorm = nsRE::CResourceModelProvider::FindResourceModel(resmKey);
			}
		}

		gizmorm->GetRenderModel(nullptr)->AddWorldObject(this);

		Super::ReadyToDrawing();
	}

	glm::vec3 CCircleComponent::getNormal() const
	{
		return GetWorldFront();
	}

	const SBound& CCircleComponent::GetBound()
	{
		RecalcMatrix();	// 여기 - 여길 막으려면 dirty 에서 미리 캐시해놓는 시스템을 만들고, 그걸로 안될 때는 바깥쪽에서 리칼크를 불러줘야한다

		// 써클은 X 와 Y 로만 만들어져야한다, Z 는 Configs::BoundEpsilon 로 고정이거나 마치 0처럼 취급될 것이다

		const auto center = GetWorldPosition();
		const auto halfSize = GetWorldScales() / 2.f;

		circleBound_ = SCircleBound(getNormal(), center, halfSize.x);
		return circleBound_;
	}
};
