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

	float CSphereComponent::GetRadius(bool isWorld) const
	{
		if(isWorld)
			return GetFinalScales()[nsRE::TransformConst::DefaultAxis_Front] * 0.5f;	// 반지름이니까
		else
			return GetPendingScales()[nsRE::TransformConst::DefaultAxis_Front] * 0.5f;	// 반지름이니까
	}

	const SBound& CSphereComponent::GetBoundWorldSpace(bool isForceRecalc)
	{
		if (isForceRecalc || IsDirty())
		{
			RecalcFinal();	// 여기 - 여길 막으려면 dirty 에서 미리 캐시해놓는 시스템을 만들고, 그걸로 안될 때는 바깥쪽에서 리칼크를 불러줘야한다

			sphereBound_ = SSphereBound(GetFinalPosition(), GetRadius(true));
		}
		return sphereBound_;
	}
};
