#include "CCubeComponent.h"
#include "../CGameBase.h"
#include "../CEngineBase.h"
#include "../Objects/CActor.h"
#include "../../MonoMax.EngineCore/RenderingEngineGizmo.h"

namespace SMGE
{
	CCubeComponent::CCubeComponent(CObject* outer) : Super(outer)
	{
		Ctor();
	}

	void CCubeComponent::Ctor()
	{
		isGameVisible_ = false;
#if IS_EDITOR
		isEditorVisible_ = true;
#endif
		boundType_ = EBoundType::CUBE;
	}

	void CCubeComponent::OnBeginPlay(CObject* parent)
	{
		Super::OnBeginPlay(parent);
	}

	void CCubeComponent::OnEndPlay()
	{
		Super::OnEndPlay();
	}

	SGReflection& CCubeComponent::getReflection()
	{
		if (reflCubeCompo_.get() == nullptr)
			reflCubeCompo_ = MakeUniqPtr<TReflectionStruct>(*this);
		return *reflCubeCompo_.get();
	}

	void CCubeComponent::ReadyToDrawing()
	{
		const auto resmKey = "gizmoK:cube";

		auto gizmorm = GetRenderingEngine()->GetResourceModel(resmKey);
		if (gizmorm == nullptr)
			gizmorm = new nsRE::CubeRM();

		// 여기 수정 - 이거 CResourceModel 로 내리든가, 게임엔진에서 렌더링을 하도록 하자
		GetRenderingEngine()->AddResourceModel(resmKey, std::move(gizmorm));

		gizmorm->GetRenderModel().AddWorldObject(this);

		Super::ReadyToDrawing();
	}

	bool CCubeComponent::CheckCollide(CBoundComponent* checkTarget, glm::vec3& outCollidingPoint)
	{
		return false;
	}

	class CCubeComponent* CCubeComponent::GetOBB()
	{
		weakOBB_ = this;	// 나 자신이 OBB이다
		return weakOBB_;
	}

	SCubeBound CCubeComponent::GetAABB()
	{
		// 액터가 무버블인 경우 aabb 는 계속 바뀔 것이기 때문에 매번 생성한다 / 아니면 캐싱한다
		SCubeBound aabb;

		// 모델좌표 -> 월드 좌표
		//glm::vec3 worldLB = this->centerPos_ - size_, worldRT = this->centerPos_ + size_;

		// 큐브 콤포넌트가 아니고 큐브 바운드 개체를 만들자 - 쓰고 버리는 개체

		return aabb;
	}
};
