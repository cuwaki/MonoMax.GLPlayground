#pragma once

#include "../GECommonIncludes.h"
#include "CDrawComponent.h"
#include "../CBoundCheck.h"

namespace SMGE
{
	class CBoundComponent;

	using DELEGATE_OnCollide = std::function<void(class CActor *SRC, class CActor *TAR, const class CBoundComponent *SRC_BOUND, const class CBoundComponent *TAR_BOUND, const SSegmentBound& COLL_POS)>;

	////////////////////////////////////////////////////////////////////////////////////////////////////////
	enum class ECheckCollideRule
	{
		NEAREST,
		FARTHEST,
		ALL,
		MAX,
	};

	class CBoundComponent;

	struct SGRefl_BoundComponent : public SGRefl_DrawComponent
	{
		using Super = SGRefl_DrawComponent;
		using TReflectionClass = CBoundComponent;

		SGRefl_BoundComponent(TReflectionClass& rc);

		virtual operator CWString() const override;
		virtual SGReflection& operator=(CVector<TupleVarName_VarType_Value>& in) override;

		bool& isPickingTarget_;
		bool& isCollideTarget_;
		glm::vec3& gizmoColor_;
	};

	class CBoundComponent : public CDrawComponent
	{
	public:
		using Super = CDrawComponent;
		using TReflectionStruct = SGRefl_BoundComponent;

		friend struct TReflectionStruct;

	public:
		CBoundComponent(CObject* outer);

		virtual void OnBeginPlay(class CObject* parent) override;
		virtual void Tick(float td) override;

		void Ctor();

	protected:
		virtual void OnBeforeRendering() override;

	public:
		virtual bool CheckCollide(CBoundComponent* checkTarget, SSegmentBound& outCross);

		EBoundType GetBoundType() const { return boundType_; }
		virtual const SBound& GetBoundWorldSpace(bool isForceRecalc = false) = 0;

		virtual const class CCubeComponent* GetOBB() const;
		const SAABB& GetAABB() const;
		virtual void CacheAABB();

		virtual bool IsPickingTarget() const { return isPickingTarget_; }
		virtual void SetPickingTarget(bool);
		virtual bool IsCollideTarget() const { return isCollideTarget_; }
		virtual void SetCollideTarget(bool);

		glm::vec3 GetGizmoColor() const { return gizmoColor_; }
		void SetGizmoColor(const glm::vec3& gc) { gizmoColor_ = gc; }

		virtual SGReflection& getReflection() override;

	protected:
		virtual class CCubeComponent* CreateOBB();

	protected:
		EBoundType boundType_ = EBoundType::MAX;

		SAABB cachedAABB_;
		mutable class CCubeComponent* weakOBB_;

		// reflection
		bool isPickingTarget_;
		bool isCollideTarget_;
		glm::vec3 gizmoColor_;

		std::unique_ptr<TReflectionStruct> reflMeshCompo_;
	};
};
