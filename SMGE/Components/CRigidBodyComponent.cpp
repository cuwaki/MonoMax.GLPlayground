#include "CRigidBodyComponent.h"

namespace SMGE
{
	using namespace Physics;

	SGRefl_RigidBodyComponent::SGRefl_RigidBodyComponent(TReflectionClass& rc) : Super(rc), 
		mass_(rc.mass_),
		antiGravity_(rc.antiGravity_),
		normalForce_(rc.normalForce_)
	{
	}

	const SGReflection& SGRefl_RigidBodyComponent::operator>>(CWString& out) const
	{
		Super::operator>>(out);

		out += _TO_REFL(PFloat, mass_);
		out += _TO_REFL(PVec3, antiGravity_);
		out += _TO_REFL(PVec3, normalForce_);

		return *this;
	}

	SGReflection& SGRefl_RigidBodyComponent::operator<<(const CVector<TupleVarName_VarType_Value>& in)
	{
		Super::operator<<(in);

		_FROM_REFL(mass_, in);
		_FROM_REFL(antiGravity_, in);
		_FROM_REFL(normalForce_, in);

		return *this;
	}

	//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	CRigidBodyComponent::CRigidBodyComponent(CObject* outer) : CComponent(outer), CRigidBody(GWorld)
	{
		Ctor();
	}

	void CRigidBodyComponent::Ctor()
	{
	}

	SGReflection& CRigidBodyComponent::getReflection()
	{
		if (reflRigidCompo_.get() == nullptr)
			reflRigidCompo_ = std::make_unique<TReflectionStruct>(*this);
		return *reflRigidCompo_.get();
	}
};
