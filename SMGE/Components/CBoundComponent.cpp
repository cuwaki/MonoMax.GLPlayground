#include "CBoundComponent.h"
#include "../CGameBase.h"
#include "../CEngineBase.h"
#include "CCubeComponent.h"
#include "../Objects/CMap.h"
#include "../Assets/CAssetManager.h"
#include "../../MonoMax.EngineCore/RenderingEngineGizmo.h"

//#define DEBUG_DRAW_AABB

namespace SMGE
{
	SGRefl_BoundComponent::SGRefl_BoundComponent(TReflectionClass& rc) : Super(rc), 
		isPickingTarget_(rc.isPickingTarget_),
		isCollideTarget_(rc.isCollideTarget_),
		gizmoColor_(rc.gizmoColor_)
	{
	}

	const SGReflection& SGRefl_BoundComponent::operator>>(CWString& out) const
	{
		Super::operator>>(out);

		out += _TO_REFL(bool, isPickingTarget_);
		out += _TO_REFL(bool, isCollideTarget_);
		out += _TO_REFL(glm::vec3, gizmoColor_);

		return *this;
	}

	SGReflection& SGRefl_BoundComponent::operator<<(const CVector<TupleVarName_VarType_Value>& in)
	{
		Super::operator<<(in);

		_FROM_REFL(isPickingTarget_, in);
		_FROM_REFL(isCollideTarget_, in);
		_FROM_REFL(gizmoColor_, in);

		return *this;
	}

	//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	CBoundComponent::CBoundComponent(CObject* outer) : CDrawComponent(outer)
	{
		weakOBB_ = nullptr;

		Ctor();
	}

	void CBoundComponent::Ctor()
	{
		//isPickingTarget_ = true;
		//isCollideTarget_ = true;
	}

	void CBoundComponent::OnBeforeRendering()
	{
		WorldObject::OnBeforeRendering();

		const auto rm = static_cast<const nsRE::GizmoRenderModel*>(renderModel_);
		rm->GetGizmoShaderSet()->set_vertexColorForFragment(gizmoColor_);
	}

	void CBoundComponent::OnBeginPlay(CObject* parent)
	{
#ifdef DEBUG_DRAW_AABB
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
				getTransientComponents().push_back(std::make_unique<CCubeComponent>(RTTI_CObject::NewDefault<CCubeComponent>(this)));

				auto aabb = dynamic_cast<CCubeComponent*>(getTransientComponents().back().get());
				if (aabb)
				{
					auto asset = CAssetManager::LoadAssetDefault<CComponent>(Globals::GetGameAssetPath(wtext("/templates/CCubeComponent.asset")));
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

		GetBoundWorldSpace(true);	// isdirty 일 때만 계산되므로 한번 강제로 갱신해준다
		CacheAABB();
	}

	void CBoundComponent::Tick(float td)
	{
		Super::Tick(td);

#ifdef DEBUG_DRAW_AABB
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
			return GetBoundWorldSpace().check(checkTarget->GetBoundWorldSpace(), outCross);
		
		return false;
	}

	const SAABB& CBoundComponent::GetAABB() const
	{
		return cachedAABB_;
	}

	void CBoundComponent::CacheAABB()
	{
#ifdef DEBUG_DRAW_AABB
		// 테스트 코드 - AABB
		if (cobjectTag_ != "debug aabb")
#endif
			cachedAABB_ = GetBoundWorldSpace();
	}

	const class CCubeComponent* CBoundComponent::GetOBB() const
	{
		if (weakOBB_ == nullptr)
			weakOBB_ = const_cast<CBoundComponent*>(this)->CreateOBB();

		return weakOBB_;
	}

	class CCubeComponent* CBoundComponent::CreateOBB()
	{
		auto map = FindOuter<CMap>(this);
		if (map != nullptr)	// 현재 맵이 없다면 beginplay 가 작동하지 않아서 제대로된 obb로서의 작동을 할 수 없다
		{
			// 이렇게 단위 크기의 큐브콤포넌트를 만들어서 자식 콤포넌트로 붙이면 현재의 루트객체의 트랜스폼을 따라서 자동으로 트랜스폼 조정이 되게 된다.
			// 점, 레이 같이 몇몇 특수한 케이스 외에는 추가 처리가 필요없다.
			// 단, 현재로서는 OBB 가 붙는 nsRE::Transform 이 GL좌표계의 1 단위로 크기 조정이 되어있다는 가정이 있다
			// 앞으로 SMGE 는 위와 같이 크기를 정규화(?) 한다는 룰이 있다면 이렇게 편하게 처리가 가능할 듯...

			getTransientComponents().push_back(std::make_unique<CCubeComponent>(RTTI_CObject::NewDefault<CCubeComponent>(this)));

			auto obbCube = dynamic_cast<CCubeComponent*>(getTransientComponents().back().get());
			if (obbCube)
			{
				// 테스트 코드 - OBB 가 눈에 보이도록
//				obbCube->isGameRendering_ = false;
//#if IS_EDITOR
//				obbCube->isEditorRendering_ = false;
//#endif

				// 여기 - LoadObject 와 같이 new 하고 reflect 까지 한번에 끝내주는 함수 필요
				auto asset = CAssetManager::LoadAssetDefault<CComponent>(Globals::GetGameAssetPath(wtext("/templates/CCubeComponent.asset")));
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
			reflMeshCompo_ = std::make_unique<TReflectionStruct>(*this);
		return *reflMeshCompo_.get();
	}

	void CBoundComponent::SetPickingTarget(bool isP)
	{
		isPickingTarget_ = isP;
	}
	
	void CBoundComponent::SetCollideTarget(bool isC)
	{
		isCollideTarget_ = isC;
	}
};
