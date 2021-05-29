#pragma once

#include "../GECommonIncludes.h"
#include "../Physics.h"
#include "CComponent.h"

namespace SMGE
{
	using namespace Physics;

	class CRigidBodyComponent;

	struct SGRefl_RigidBodyComponent : public SGRefl_Component
	{
		using Super = SGRefl_Component;
		using TReflectionClass = CRigidBodyComponent;

		SGRefl_RigidBodyComponent(TReflectionClass& rc);

		virtual const SGReflection& operator>>(CWString& out) const override;
		virtual SGReflection& operator<<(const CVector<TupleVarName_VarType_Value>& in) override;

		PFloat& mass_;
		PVec3& antiGravity_;
		PVec3& normalForce_;
	};

	class CRigidBodyComponent : public CComponent, public CRigidBody
	{
		DECLARE_RTTI_CObject(CRigidBodyComponent)

	public:
		using Super = CComponent;
		using TReflectionStruct = SGRefl_RigidBodyComponent;

		friend struct TReflectionStruct;

	public:
		CRigidBodyComponent(CObject* outer);

		void Ctor();

		virtual SGReflection& getReflection() override;

	protected:
		UPtr<TReflectionStruct> reflRigidCompo_;
	};
};
