#pragma once

#include "../GECommonIncludes.h"
#include "CDrawComponent.h"
#include "../CBoundCheck.h"

namespace SMGE
{
	class CBoundComponent;

	using DELEGATE_OnCollide = std::function<void(class CActor *SRC, class CActor *TAR, const class CBoundComponent *SRC_BOUND, const class CBoundComponent *TAR_BOUND, const glm::vec3& COLL_POS)>;

	////////////////////////////////////////////////////////////////////////////////////////////////////////
	enum class ECheckCollideRule
	{
		NEAREST,
		FARTHEST,
		ALL,
		MAX,
	};

	class CBoundComponent : public CDrawComponent
	{
	public:
		using Super = CDrawComponent;
		using TReflectionStruct = typename Super::TReflectionStruct;

		friend struct TReflectionStruct;

	public:
		CBoundComponent(CObject* outer);

		virtual void OnBeginPlay(class CObject* parent) override;
		virtual void OnEndPlay() override;

		void Ctor();

	public:
		virtual bool CheckCollide(CBoundComponent* checkTarget, glm::vec3& outCollidingPoint);

		virtual bool IsPickingTarget() const { return isPickingTarget_; }
		virtual bool IsCollideTarget() const { return isCollideTarget_; }

		EBoundType GetBoundType() const { return boundType_; }
		virtual const SBound& getBound() = 0;

		virtual const class CCubeComponent* GetOBB() const;

		const SAABB& GetAABB() const;
		virtual void CacheAABB() { cachedAABB_ = getBound(); }

	protected:
		virtual class CCubeComponent* CreateOBB();

	protected:
		bool isPickingTarget_;
		bool isCollideTarget_;

		EBoundType boundType_ = EBoundType::MAX;

		SAABB cachedAABB_;
		mutable class CCubeComponent* weakOBB_;
	};
};
