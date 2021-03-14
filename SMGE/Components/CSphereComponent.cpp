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
		isGameRendering_ = false;
#if IS_EDITOR
		isEditorRendering_ = true;
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
			reflSphereCompo_ = std::make_unique<TReflectionStruct>(*this);
		return *reflSphereCompo_.get();
	}

	void CSphereComponent::ReadyToDrawing()
	{
		const auto resmKey = "gizmoK:sphere";

		auto gizmorm = nsRE::CResourceModelProvider::FindResourceModel(resmKey);
		if (gizmorm == nullptr)
			gizmorm = nsRE::CResourceModelProvider::AddResourceModel(resmKey, std::make_shared<nsRE::SphereResourceModel>());

		gizmorm->GetRenderModel(nullptr)->AddWorldObject(this);

		Super::ReadyToDrawing();
	}

	float CSphereComponent::GetRadius() const
	{
		return GetWorldScales()[nsRE::TransformConst::DefaultAxis_Front] / 2.f;	// 반지름이니까
	}

	const SBound& CSphereComponent::GetBound()
	{
		RecalcMatrix();	// 여기 - 여길 막으려면 dirty 에서 미리 캐시해놓는 시스템을 만들고, 그걸로 안될 때는 바깥쪽에서 리칼크를 불러줘야한다

		sphereBound_ = SSphereBound(GetWorldPosition(), GetRadius());
		return sphereBound_;
	}
};
