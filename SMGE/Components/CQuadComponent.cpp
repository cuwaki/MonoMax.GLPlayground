#include "CQuadComponent.h"
#include "../CGameBase.h"
#include "../CEngineBase.h"
#include "../Objects/CActor.h"
#include "../../MonoMax.EngineCore/RenderingEnginePrimitive.h"

namespace SMGE
{
	CQuadComponent::CQuadComponent(CObject* outer) : Super(outer)
	{
		hasFace_ = true;	// 테스트 코드 - 디버깅용 눈에 보이라고
		Ctor();
	}
	CQuadComponent::CQuadComponent(CObject* outer, bool hasFace) : CQuadComponent(outer)
	{
		hasFace_ = hasFace;
	}

	void CQuadComponent::Ctor()
	{
		isGameRendering_ = false;
#if IS_EDITOR
		isEditorRendering_ = true;
#endif
		boundType_ = EBoundType::QUAD;

		// 쿼드는 X 와 Y 로만 만들어져야한다, Z 는 Configs::BoundEpsilon 로 고정이거나 마치 0처럼 취급될 것이다
	}

	void CQuadComponent::ReadyToDrawing()
	{
		CString resmKey;
		SPtr<nsRE::ResourceModelBase> primitiveResM;
		if (hasFace_)
		{
			resmKey = "primitiveK:quad_faced";
			primitiveResM = nsRE::CResourceModelProvider::FindResourceModel(resmKey);
			if (primitiveResM == nullptr)
				primitiveResM = nsRE::CResourceModelProvider::AddResourceModel(resmKey, std::make_shared<nsRE::QuadFacedResourceModel>());
		}
		else
		{
			resmKey = "primitiveK:quad";
			primitiveResM = nsRE::CResourceModelProvider::FindResourceModel(resmKey);
			if (primitiveResM == nullptr)
				primitiveResM = nsRE::CResourceModelProvider::AddResourceModel(resmKey, std::make_shared<nsRE::QuadResourceModel>());
		}

		primitiveResM->GetRenderModel(nullptr)->AddWorldModel(this);

		Super::ReadyToDrawing();
	}

	glm::vec3 CQuadComponent::getNormal(bool isWorld) const
	{
		if(isWorld)
			return GetFinalFront();
		else
			return GetPendingFront();
	}

	const SBound& CQuadComponent::GetBoundWorldSpace(bool isForceRecalc)
	{
		// 여기 - 최적화 필요 - IsDirty() 가 해소되는 것이 액터의 AfterTick 인 현재 상황에서는 무조건 재계산을 해야한다
		//if (isForceRecalc || IsDirty())
		{
			//RecalcFinal();	

			// 쿼드는 X 와 Y 로만 만들어져야한다, Z 는 Configs::BoundEpsilon 로 고정이거나 마치 0처럼 취급될 것이다

			const auto center = GetFinalPosition();
			const auto halfSize = GetFinalScales() * 0.5f;
			const auto xVec = GetFinalAxis(nsRE::TransformConst::ETypeAxis::X) * halfSize.x;
			const auto yVec = GetFinalAxis(nsRE::TransformConst::ETypeAxis::Y) * halfSize.y;

			quadBound_ = SQuadBound(center - xVec - yVec, center + xVec - yVec, center + xVec + yVec, center - xVec + yVec);	// 좌하, 우하, 우상, 좌상 - 반시계 방향으로
		}
		return quadBound_;
	}
};
