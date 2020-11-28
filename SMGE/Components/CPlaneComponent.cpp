#include "CPlaneComponent.h"
#include "../CGameBase.h"
#include "../CEngineBase.h"
#include "../Objects/CActor.h"
#include "../../MonoMax.EngineCore/RenderingEngineGizmo.h"

namespace SMGE
{
	CPlaneComponent::CPlaneComponent(CObject* outer) : Super(outer)
	{
		hasFace_ = true;	// 테스트 코드
		Ctor();
	}
	CPlaneComponent::CPlaneComponent(CObject* outer, bool hasFace) : CPlaneComponent(outer)
	{
		hasFace_ = hasFace;
	}

	void CPlaneComponent::Ctor()
	{
		isGameVisible_ = false;
#if IS_EDITOR
		isEditorVisible_ = true;
#endif
		boundType_ = EBoundType::PLANE;
	}

	void CPlaneComponent::OnBeginPlay(CObject* parent)
	{
		Super::OnBeginPlay(parent);
	}

	void CPlaneComponent::OnEndPlay()
	{
		Super::OnEndPlay();
	}

	SGReflection& CPlaneComponent::getReflection()
	{
		if (reflPlaneCompo_.get() == nullptr)
			reflPlaneCompo_ = MakeUniqPtr<TReflectionStruct>(*this);
		return *reflPlaneCompo_.get();
	}

	void CPlaneComponent::ReadyToDrawing()
	{
		CString resmKey;
		nsRE::ResourceModelBase* gizmorm = nullptr;
		if (hasFace_)
		{
			resmKey = "gizmoK:plane_faced";
			gizmorm = GetRenderingEngine()->GetResourceModel(resmKey);
			if (gizmorm == nullptr)
			{
				gizmorm = new nsRE::PlaneFacedRM();
				GetRenderingEngine()->AddResourceModel(resmKey, std::move(gizmorm));	// 여기 수정 - 이거 CResourceModel 로 내리든가, 게임엔진에서 렌더링을 하도록 하자
			}
		}
		else
		{
			resmKey = "gizmoK:plane";
			gizmorm = GetRenderingEngine()->GetResourceModel(resmKey);
			if (gizmorm == nullptr)
			{
				gizmorm = new nsRE::PlaneRM();
				GetRenderingEngine()->AddResourceModel(resmKey, std::move(gizmorm));	// 여기 수정 - 이거 CResourceModel 로 내리든가, 게임엔진에서 렌더링을 하도록 하자
			}
		}

		gizmorm->GetRenderModel().AddWorldObject(this);

		Super::ReadyToDrawing();
	}

	bool CPlaneComponent::CheckCollide(CBoundComponent* checkTarget, glm::vec3& outCollidingPoint)
	{
		return false;
	}

	const class CCubeComponent* CPlaneComponent::GetOBB()
	{
		if(weakOBB_ == nullptr)
			weakOBB_ = CreateOBB();
		return weakOBB_;
	}

	glm::vec3 CPlaneComponent::getNormal() const
	{
		return GetWorldFront();
	}

	SPlaneBound CPlaneComponent::getBound() const
	{
		SPlaneBound ret(getNormal(), { 0,0,0 });
		
		// 여기 - 트랜스폼에서 필요한 것들 내보내야함 - 위치 크기 회전
		// 여기 - 사실 평면 컴포넌트는 없고, 쿼드 컴포넌트로 대체되어야맞다

		return ret;
	}
};
