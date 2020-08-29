#pragma once

#include "../GECommonIncludes.h"
#include "CDrawComponent.h"

namespace SMGE
{
	class CBoundComponent;

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
		virtual bool IsPickingTarget() const { return isPickingTarget_; }
		virtual bool IsCollideTarget() const { return isCollideTarget_; }

	protected:
		bool isPickingTarget_;
		bool isCollideTarget_;
	};
};
