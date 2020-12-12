#include "CCubeComponent.h"
#include "../CGameBase.h"
#include "../CEngineBase.h"
#include "../Objects/CActor.h"
#include "../../MonoMax.EngineCore/RenderingEngineGizmo.h"
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
		{
			GetRenderingEngine()->AddResourceModel(resmKey, std::move(new nsRE::CubeRM()));	// 여기 수정 - 이거 CResourceModel 로 내리든가, 게임엔진에서 렌더링을 하도록 하자
			gizmorm = GetRenderingEngine()->GetResourceModel(resmKey);
		}

		gizmorm->GetRenderModel().AddWorldObject(this);

		Super::ReadyToDrawing();
	}

	bool CCubeComponent::CheckCollide(CBoundComponent* checkTarget, glm::vec3& outCollidingPoint)
	{
		return false;
	}

	const SBound& CCubeComponent::getBound()
	{
		RecalcMatrix();	// 여기 - 여길 막으려면 dirty 에서 미리 캐시해놓는 시스템을 만들고, 그걸로 안될 때는 바깥쪽에서 리칼크를 불러줘야한다

		cubeBound_ = SCubeBound(GetWorldPosition(), GetWorldScales(), { GetWorldAxis(ETypeAxis::X), GetWorldAxis(ETypeAxis::Y), GetWorldAxis(ETypeAxis::Z) });
		return cubeBound_;
	}
};
