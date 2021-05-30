#include "CRigidBodyComponent.h"
#include "CBoundComponent.h"

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
		boundOuter_ = dynamic_cast<CBoundComponent*>(outer);
		transformOuter_ = dynamic_cast<nsRE::Transform*>(outer);

		assert(boundOuter_);
		assert(transformOuter_);

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

	void CRigidBodyComponent::Tick(float dt)
	{
		Super::Tick(dt);
	}

	bool CRigidBodyComponent::IsPhysicsActivated() const
	{
		return IsActive();
	}
	void CRigidBodyComponent::SetPhysicsActivate(bool a)
	{
		if (a == true)
		{	// 물리 작동 재시작
			SetPhysicsPosition(getTransform().GetPendingPosition());
			SetOwnUniformVelocity(PVec3(0.));
		}

		SetActive(a);
	}

	nsRE::Transform& CRigidBodyComponent::getTransform()
	{
		assert(transformOuter_);
		return *transformOuter_;
	}

	const nsRE::Transform& CRigidBodyComponent::getTransform() const
	{
		assert(transformOuter_);
		return *transformOuter_;
	}
};
