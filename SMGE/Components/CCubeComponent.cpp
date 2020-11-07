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
			gizmorm = new nsRE::CubeRM();
			GetRenderingEngine()->AddResourceModel(resmKey, std::move(gizmorm));	// 여기 수정 - 이거 CResourceModel 로 내리든가, 게임엔진에서 렌더링을 하도록 하자
		}

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

	SAABB CCubeComponent::GetAABB()
	{
		RecalcMatrix();

		const int X = ETypeAxis::X, Y = ETypeAxis::Y, Z = ETypeAxis::Z;

		SCubeBound cb;
		cb.centerPos_ = GetWorldPosition();
		cb.size_ = GetWorldScales();
		cb.dir_[X] = GetWorldAxis(ETypeAxis::X);
		cb.dir_[Y] = GetWorldAxis(ETypeAxis::Y);
		cb.dir_[Z] = GetWorldAxis(ETypeAxis::Z);

		auto xHalfSize = cb.dir_[X] * cb.size_[X] * 0.5f;
		auto yHalfSize = cb.dir_[Y] * cb.size_[Y] * 0.5f;
		auto zHalfSize = cb.dir_[Z] * cb.size_[Z] * 0.5f;

		SAABB aabb;
		auto points = { cb.centerPos_ - xHalfSize, cb.centerPos_ - yHalfSize, cb.centerPos_ - zHalfSize, cb.centerPos_ + xHalfSize, cb.centerPos_ + yHalfSize, cb.centerPos_ + zHalfSize };
		aabb.lb_ = *points.begin();
		aabb.rt_ = *points.begin();

		std::for_each(points.begin(), points.end(), [&aabb](auto& point)
			{
				if (point.x < aabb.lb_.x)
					aabb.lb_.x = point.x;
				if (point.y < aabb.lb_.y)
					aabb.lb_.y = point.y;
				if (point.z < aabb.lb_.z)
					aabb.lb_.z = point.z;

				if (point.x > aabb.rt_.x)
					aabb.rt_.x = point.x;
				if (point.y > aabb.rt_.y)
					aabb.rt_.y = point.y;
				if (point.z > aabb.rt_.z)
					aabb.rt_.z = point.z;
			});

		return aabb;
	}
};
