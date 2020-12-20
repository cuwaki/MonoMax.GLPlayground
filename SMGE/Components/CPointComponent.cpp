#include "CPointComponent.h"
#include "../CGameBase.h"
#include "../CEngineBase.h"
#include "../Objects/CActor.h"
#include "../../MonoMax.EngineCore/RenderingEngineGizmo.h"
#include "CCubeComponent.h"

namespace SMGE
{
	CPointComponent::CPointComponent(CObject* outer) : CBoundComponent(outer)
	{
		Ctor();
	}

	void CPointComponent::Ctor()
	{
		isGameVisible_ = false;
#if IS_EDITOR
		isEditorVisible_ = true;
#endif

		boundType_ = EBoundType::POINT;

		isPickingTarget_ = false;	// 포인트는 이게 자연스럽다
		isCollideTarget_ = false;
	}

	void CPointComponent::OnBeginPlay(CObject* parent)
	{
		Super::OnBeginPlay(parent);
	}

	void CPointComponent::OnEndPlay()
	{
		Super::OnEndPlay();
	}

	SGReflection& CPointComponent::getReflection()
	{
		if (reflPointCompo_.get() == nullptr)
			reflPointCompo_ = MakeUniqPtr<TReflectionStruct>(*this);
		return *reflPointCompo_.get();
	}

	void CPointComponent::ReadyToDrawing()
	{
		const auto resmKey = "gizmoK:point";

		auto gizmorm = nsRE::CResourceModelProvider::FindResourceModel(resmKey);
		if (gizmorm == nullptr)
		{
			nsRE::CResourceModelProvider::AddResourceModel(resmKey, std::move(new nsRE::PointResourceModel()));	// 여기 수정 - 이거 CResourceModel 로 내리든가, 게임엔진에서 렌더링을 하도록 하자
			gizmorm = nsRE::CResourceModelProvider::FindResourceModel(resmKey);
		}

		gizmorm->GetRenderModel(nullptr)->AddWorldObject(this);

		Super::ReadyToDrawing();
	}

	const SBound& CPointComponent::GetBound()
	{
		RecalcMatrix();	// 여기 - 여길 막으려면 dirty 에서 미리 캐시해놓는 시스템을 만들고, 그걸로 안될 때는 바깥쪽에서 리칼크를 불러줘야한다

		pointBound_ = SPointBound(GetWorldPosition());
		return pointBound_;
	}
};
