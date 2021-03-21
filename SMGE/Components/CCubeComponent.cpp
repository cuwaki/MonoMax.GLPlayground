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
		isGameRendering_ = false;
#if IS_EDITOR
		isEditorRendering_ = true;
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
			reflCubeCompo_ = std::make_unique<TReflectionStruct>(*this);
		return *reflCubeCompo_.get();
	}

	void CCubeComponent::ReadyToDrawing()
	{
		const auto resmKey = "gizmoK:cube";

		auto gizmorm = nsRE::CResourceModelProvider::FindResourceModel(resmKey);
		if (gizmorm == nullptr)
			gizmorm = nsRE::CResourceModelProvider::AddResourceModel(resmKey, std::make_shared<nsRE::CubeResourceModel>());

		gizmorm->GetRenderModel(nullptr)->AddWorldObject(this);

		Super::ReadyToDrawing();
	}

	const SBound& CCubeComponent::GetBound()
	{
		RecalcFinal();	// 여기 - 여길 막으려면 dirty 에서 미리 캐시해놓는 시스템을 만들고, 그걸로 안될 때는 바깥쪽에서 리칼크를 불러줘야한다

		cubeBound_ = SCubeBound(GetWorldPosition(), GetWorldScales(), { GetWorldAxis(ETypeAxis::X), GetWorldAxis(ETypeAxis::Y), GetWorldAxis(ETypeAxis::Z) });
		return cubeBound_;
	}
};
