#include "CQuadComponent.h"
#include "../CGameBase.h"
#include "../CEngineBase.h"
#include "../Objects/CActor.h"
#include "../../MonoMax.EngineCore/RenderingEngineGizmo.h"

namespace SMGE
{
	CQuadComponent::CQuadComponent(CObject* outer) : Super(outer)
	{
		hasFace_ = true;	// 테스트 코드
		Ctor();
	}
	CQuadComponent::CQuadComponent(CObject* outer, bool hasFace) : CQuadComponent(outer)
	{
		hasFace_ = hasFace;
	}

	void CQuadComponent::Ctor()
	{
		isGameVisible_ = false;
#if IS_EDITOR
		isEditorVisible_ = true;
#endif
		boundType_ = EBoundType::QUAD;

		// 쿼드는 X 와 Y 로만 만들어져야한다, Z 는 Configs::BoundEpsilon 로 고정이거나 마치 0처럼 취급될 것이다
	}

	void CQuadComponent::OnBeginPlay(CObject* parent)
	{
		Super::OnBeginPlay(parent);
	}

	void CQuadComponent::OnEndPlay()
	{
		Super::OnEndPlay();
	}

	SGReflection& CQuadComponent::getReflection()
	{
		if (reflPlaneCompo_.get() == nullptr)
			reflPlaneCompo_ = MakeUniqPtr<TReflectionStruct>(*this);

		// 쿼드는 X 와 Y 로만 만들어져야한다, Z 는 Configs::BoundEpsilon 로 고정이거나 마치 0처럼 취급될 것이다

		return *reflPlaneCompo_.get();
	}

	void CQuadComponent::ReadyToDrawing()
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

	bool CQuadComponent::CheckCollide(CBoundComponent* checkTarget, glm::vec3& outCollidingPoint)
	{
		return false;
	}

	void CQuadComponent::CacheAABB()
	{
		cachedAABB_ = getBound();
	}

	glm::vec3 CQuadComponent::getNormal() const
	{
		return GetWorldFront();
	}

	SQuadBound CQuadComponent::getBound()
	{
		RecalcMatrix();	// 여기 - 여길 막으려면 dirty 에서 미리 캐시해놓는 시스템을 만들고, 그걸로 안될 때는 바깥쪽에서 리칼크를 불러줘야한다

		// 쿼드는 X 와 Y 로만 만들어져야한다, Z 는 Configs::BoundEpsilon 로 고정이거나 마치 0처럼 취급될 것이다

		const auto center = GetWorldPosition();
		const auto halfSize = GetWorldScales() / 2.f;
		const auto xVec = GetWorldAxis(nsRE::TransformConst::ETypeAxis::X) * halfSize.x;
		const auto yVec = GetWorldAxis(nsRE::TransformConst::ETypeAxis::Y) * halfSize.y;

		SQuadBound qd(center - xVec - yVec, center + xVec - yVec, center + xVec + yVec, center -xVec + yVec);	// 좌하, 우하, 우상, 좌상 - 반시계 방향으로
		return qd;
	}
};
