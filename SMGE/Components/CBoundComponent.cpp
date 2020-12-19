#include "CBoundComponent.h"
#include "../CGameBase.h"
#include "../CEngineBase.h"
#include "CCubeComponent.h"
#include "../Objects/CMap.h"
#include "../Assets/CAssetManager.h"
#include "../../MonoMax.EngineCore/RenderingEngineGizmo.h"

#define DRAW_AABB

namespace SMGE
{
	SGRefl_BoundComponent::SGRefl_BoundComponent(TReflectionClass& rc) : Super(rc), 
		gizmoColor_(rc.gizmoColor_)
	{
	}

	SGRefl_BoundComponent::operator CWString() const
	{
		auto ret = Super::operator CWString();

		ret += _TO_REFL(glm::vec3, gizmoColor_);

		return ret;
	}

	SGReflection& SGRefl_BoundComponent::operator=(CVector<TupleVarName_VarType_Value>& variableSplitted)
	{
		Super::operator=(variableSplitted);

		_FROM_REFL(gizmoColor_, variableSplitted);

		return *this;
	}

	//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	CBoundComponent::CBoundComponent(CObject* outer) : CDrawComponent(outer)
	{
		Ctor();
	}

	void CBoundComponent::Ctor()
	{
		weakOBB_ = nullptr;

		isPickingTarget_ = true;
		isCollideTarget_ = true;
	}

	void CBoundComponent::OnBeforeRendering()
	{
		WorldObject::OnBeforeRendering();

		static_cast<const nsRE::GizmoRenderModel*>(renderModel_)->GetGizmoShaderSet()->set_vertexColorForFragment(gizmoColor_);
	}

	void CBoundComponent::OnBeginPlay(CObject* parent)
	{
#ifdef DRAW_AABB
		// 테스트 코드 - AABB 표시하기
		auto aabbCube = findComponent<CCubeComponent>([](auto compPtr)
			{
				return compPtr->GetCObjectTag() == "debug aabb";
			});
		if (aabbCube == nullptr && this->GetCObjectTag() != "debug aabb")
		{
			auto map = FindOuter<CMap>(this);
			if (map != nullptr)
			{
				getTransientComponents().push_back(MakeUniqPtr<CCubeComponent>(RTTI_CObject::NewDefault<CCubeComponent>(this)));

				auto aabb = DCast<CCubeComponent*>(getTransientComponents().back().get());
				if (aabb)
				{
					auto asset = CAssetManager::LoadAsset<CComponent>(Globals::GetGameAssetPath(wtext("/templates/CCubeComponent.asset")));
					aabb->CopyFromTemplate(asset->getContentClass());

					aabb->isAbsoluteTransform_ = true;
					aabb->isCollideTarget_ = false;
					aabb->SetCObjectTag("debug aabb");
					aabb->SetGizmoColor({ 1.f, 0.f, 1.f });

					//registerComponent(aabb);

					//if (map->IsBeganPlay() || map->IsBeginningPlay())
					//	aabb->OnBeginPlay(this);
				}
			}
		}
#endif

		Super::OnBeginPlay(parent);

		CacheAABB();
	}

	void CBoundComponent::OnEndPlay()
	{
		Super::OnEndPlay();
	}

	void CBoundComponent::Tick(float td)
	{
		Super::Tick(td);

#ifdef DRAW_AABB
		// 테스트 코드 - AABB
		if (cobjectTag_ == "debug aabb")
		{
			auto parentBC = static_cast<CBoundComponent*>(parent_);
			const auto& aabb = parentBC->cachedAABB_;

			RotateEuler({ 0.f, 0.f, 0.f });
			Translate(aabb.center());
			Scale(aabb.getSize());
		}
#endif
	}

	bool CBoundComponent::CheckCollide(CBoundComponent* checkTarget, SSegmentBound& outCross)
	{
		if (checkTarget != nullptr)
			return GetBound().check(checkTarget->GetBound(), outCross);
		
		return false;
	}

	const SAABB& CBoundComponent::GetAABB() const
	{
		return cachedAABB_;
	}

	void CBoundComponent::CacheAABB()
	{
#ifdef DRAW_AABB
		// 테스트 코드 - AABB
		if (cobjectTag_ != "debug aabb")
#endif
			cachedAABB_ = GetBound();
	}

	const class CCubeComponent* CBoundComponent::GetOBB() const
	{
		if (weakOBB_ == nullptr)
			weakOBB_ = const_cast<CBoundComponent*>(this)->CreateOBB();

		return weakOBB_;
	}

	class CCubeComponent* CBoundComponent::CreateOBB()
	{
		auto thisCube = DCast<CCubeComponent*>(this);
		if (thisCube)
		{	// 나 자신이 OBB이다
			return thisCube;
		}

		auto map = FindOuter<CMap>(this);
		if (map != nullptr)	// 현재 맵이 없다면 beginplay 가 작동하지 않아서 제대로된 obb로서의 작동을 할 수 없다
		{
			// 이렇게 단위 크기의 큐브콤포넌트를 만들어서 자식 콤포넌트로 붙이면 현재의 루트객체의 트랜스폼을 따라서 자동으로 트랜스폼 조정이 되게 된다.
			// 점, 레이 같이 몇몇 특수한 케이스 외에는 추가 처리가 필요없다.

			getTransientComponents().push_back(MakeUniqPtr<CCubeComponent>(RTTI_CObject::NewDefault<CCubeComponent>(this)));

			auto obbCube = DCast<CCubeComponent*>(getTransientComponents().back().get());
			if (obbCube)
			{
				// 테스트 코드 - OBB 가 눈에 보이도록
//				obbCube->isGameVisible_ = false;
//#if IS_EDITOR
//				obbCube->isEditorVisible_ = false;
//#endif

				// 여기 - LoadObject 와 같이 new 하고 reflect 까지 한번에 끝내주는 함수 필요
				auto asset = CAssetManager::LoadAsset<CComponent>(Globals::GetGameAssetPath(wtext("/templates/CCubeComponent.asset")));
				obbCube->CopyFromTemplate(asset->getContentClass());

				// 여기 - new and register and beginplay 함수 필요 - 한방에 처리 되도록
				//{
				registerComponent(obbCube);

				if (map->IsBeganPlay() || map->IsBeginningPlay())
					obbCube->OnBeginPlay(this);
				// }

				return obbCube;
			}
		}

		return nullptr;
	}

	SGReflection& CBoundComponent::getReflection()
	{
		if (reflMeshCompo_.get() == nullptr)
			reflMeshCompo_ = MakeUniqPtr<TReflectionStruct>(*this);
		return *reflMeshCompo_.get();
	}
};
