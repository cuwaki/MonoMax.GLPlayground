#pragma once

#include "../GECommonIncludes.h"
#include "CDrawComponent.h"

namespace SMGE
{
	class CBoundComponent;

	using DELEGATE_OnCollide = std::function<void(class CActor *SRC, class CActor *TAR, const class CBoundComponent *SRC_BOUND, const class CBoundComponent *TAR_BOUND, const glm::vec3& COLL_POS)>;

	////////////////////////////////////////////////////////////////////////////////////////////////////////
	enum class EBoundType
	{
		POINT,
		RAY,
		PLANE,

		SPHERE,
		CUBE,
		CYLINDER,

		INHERIT_START,

		MAX = 255,
	};

	struct SBound
	{
		EBoundType type_ = EBoundType::MAX;
	};
	struct SPlaneBound : public SBound
	{
		SPlaneBound();

		glm::vec3 normal_;
		glm::vec3 size_;
	};
	struct SCubeBound : public SBound
	{
		SCubeBound();

		glm::vec3 centerPos_;
		glm::vec3 size_;
		glm::vec3 dir_[3];
	};

	struct SAABB
	{
		glm::vec3 lb_, rt_;

		bool isIntersect(const SAABB& other) const;

		bool isXContains(const SAABB& other) const;
		bool isYContains(const SAABB& other) const;
		bool isZContains(const SAABB& other) const;
		bool isIntersectPoints(const SAABB& other) const;
		inline bool isContains(const glm::vec3& point) const;

		inline const glm::vec3& lb() const;
		inline const glm::vec3& rt() const;
		inline glm::vec3 lt() const;
		inline glm::vec3 rb() const;
	};

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

		virtual class CCubeComponent* GetOBB() = 0;
		virtual SAABB GetAABB();

	protected:
		class CCubeComponent* CreateOBB();

	protected:
		bool isPickingTarget_;
		bool isCollideTarget_;

		EBoundType boundType_ = EBoundType::MAX;

		class CCubeComponent* weakOBB_;
	};
};
