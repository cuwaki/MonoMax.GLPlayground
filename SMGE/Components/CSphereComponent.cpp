#include "CSphereComponent.h"
#include "../CGameBase.h"
#include "../CEngineBase.h"
#include "../Objects/CActor.h"
#include "../../MonoMax.EngineCore/RenderingEngineGizmo.h"

namespace SMGE
{
	CSphereComponent::CSphereComponent(CObject* outer) : CBoundComponent(outer)
	{
		Ctor();
	}

	void CSphereComponent::Ctor()
	{
		isGameVisible_ = false;
#if IS_EDITOR
		isEditorVisible_ = true;
#endif
		boundType_ = EBoundType::SPHERE;
	}

	void CSphereComponent::OnBeginPlay(CObject* parent)
	{
		Super::OnBeginPlay(parent);
	}

	void CSphereComponent::OnEndPlay()
	{
		Super::OnEndPlay();
	}

	SGReflection& CSphereComponent::getReflection()
	{
		if (reflSphereCompo_.get() == nullptr)
			reflSphereCompo_ = MakeUniqPtr<TReflectionStruct>(*this);
		return *reflSphereCompo_.get();
	}

	void CSphereComponent::ReadyToDrawing()
	{
		const auto resmKey = "gizmoK:sphere";

		auto gizmorm = GetRenderingEngine()->GetResourceModel(resmKey);
		if (gizmorm == nullptr)
		{
			gizmorm = new nsRE::SphereRM();
			GetRenderingEngine()->AddResourceModel(resmKey, std::move(gizmorm));	// 여기 수정 - 이거 CResourceModel 로 내리든가, 게임엔진에서 렌더링을 하도록 하자
		}

		gizmorm->GetRenderModel().AddWorldObject(this);

		Super::ReadyToDrawing();
	}

	void CSphereComponent::CacheAABB()
	{
		cachedAABB_ = getBound();
	}

	float CSphereComponent::GetRadius() const
	{
		return GetWorldScales()[nsRE::TransformConst::DefaultAxis_Front] / 2.f;	// 반지름이니까
	}

	SSphereBound CSphereComponent::getBound()
	{
		RecalcMatrix();	// 여기 - 여길 막으려면 dirty 에서 미리 캐시해놓는 시스템을 만들고, 그걸로 안될 때는 바깥쪽에서 리칼크를 불러줘야한다

		SSphereBound sp(GetWorldPosition(), GetRadius());
		return sp;
	}
};
