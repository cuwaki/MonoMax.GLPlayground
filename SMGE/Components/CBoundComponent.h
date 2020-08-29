#pragma once

#include "../GECommonIncludes.h"
#include "CDrawComponent.h"

namespace SMGE
{
	//class CBoundComponent;

	//struct SGRefl_BoundComponent : public SGRefl_Component
	//{
	//	using Super = SGRefl_Component;
	//	using TReflectionClass = CBoundComponent;

	//	SGRefl_BoundComponent(TReflectionClass& rc);
	//	//SGRefl_BoundComponent(const CUniqPtr<CBoundComponent>& uptr);// { persistentComponentsREFL_ RTTI « ø‰ ¿ÃΩ¥

	//	virtual void OnBeforeSerialize() const override;
	//	virtual operator CWString() const override;
	//	virtual SGReflection& operator=(CVector<TupleVarName_VarType_Value>& in) override;

	//	SGRefl_Transform& sg_transform_;

	//	TReflectionClass& outerBoundCompo_;
	//};

	//class CBoundComponent : public CDrawComponent, public nsRE::WorldModel
	//{
	//public:
	//	using Super = CDrawComponent;
	//	using TReflectionStruct = SGRefl_BoundComponent;

	//	friend struct TReflectionStruct;

	//public:
	//	CBoundComponent(CObject* outer);

	//	virtual void OnBeginPlay(class CObject* parent) override;
	//	virtual void OnEndPlay() override;

	//protected:
	//};
};
