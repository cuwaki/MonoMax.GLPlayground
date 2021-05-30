#pragma once

#include "GECommonIncludes.h"
#include <array>
#include <forward_list>
#include <set>

namespace SMGE
{
	namespace Physics
	{
		using PFloat = float;
		using PVec3 = glm::vec<3, PFloat>;

//#define kg
//#define newton
//#define ms
		enum EMaterial
		{
			NONE,
			IRON,
			// ...
			MAX,
		};

		namespace Constants
		{
			constexpr PFloat _1KG = 1000.;
			constexpr PFloat _1M = 1.;								// GL 기본값 기준
			constexpr PFloat _DEFAULT_GRAVITY_Y_MS2 = -9.8;			// GL Y축 기준
		};

		struct SContactPlane
		{
			SContactPlane() {}
			SContactPlane(PVec3 n, EMaterial m)
			{
				normal_ = n;
				material_ = m;
			}
			PVec3 normal_;
			EMaterial material_;
			class CRigidBody* contact_;	// 개발 예정

			bool operator==(const SContactPlane& other)
			{
				return other.contact_ == this->contact_;
			}
		};

		struct SForce
		{
			PFloat durationMS_;
			PVec3 force_;
			class CRigidBody* from_;	// 개발 예정

			bool IsExpired() const;
			bool IsImpact() const;
			bool IsThrust() const { return !IsImpact(); }
		};

		class CRigidBody
		{
		public:
			CRigidBody(const class CWorld& world);
			~CRigidBody();

			void Tick(float dt);

			PFloat Mass() const { return mass_; }
			PVec3 Weight() const;
			EMaterial Material() const { return material_; }
			PVec3 NeedNormalForce(PVec3 contactPlaneNormal, PVec3* outPerpForce = nullptr) const;
			PVec3 CurrentFrictionalForce(EMaterial contactPlaneMat, PVec3 contactPlaneNormal, bool isKinetic) const;

			bool IsSliding(EMaterial contactPlaneMat, PVec3 contactPlaneNormal) const;
			PVec3 ComputeSlidingForce(EMaterial contactPlaneMat, PVec3 contactPlaneNormal) const;

			bool IsActive() const { return isActive_; }
			void SetActive(bool a);

			void SetOwnUniformVelocity(PVec3 ouv);
			PVec3 GetOwnUniformVelocity() const;

			void SetPhysicsPosition(PVec3 pos);
			PVec3 GetPhysicsPosition() const;
			PVec3 GetOldPhysicsPosition() const;

			void AddForce(SForce f);
			PVec3 GetTotalThrustForce() const;

			PVec3 ComputeAccelerationFromForce(PVec3 totalForce);
			PVec3 TotalAcceleration() const { return impactAcceleration_ + thrustAcceleration_; }

			const auto& GetContactPlanes() const;

		protected:
			void OnChangedActive();

		protected:
			PFloat mass_ = 0.;
			PVec3 antiGravity_{ 0. };
			PVec3 normalForce_{ 0. };
			EMaterial material_ = EMaterial::NONE;

			PVec3 ownUniformVelocity_{ 0. };	// 스스로 내는 등속도
			PVec3 position_{ 0. };
			PVec3 oldPosition_{ 0. };

			bool isActive_ = false;

			std::set<SContactPlane> contactPlanes_;

			std::forward_list<SForce> thrustForces_;
			PVec3 impactAcceleration_{ 0. };
			PVec3 thrustAcceleration_{ 0. };

		protected:
			const class CWorld& world_;
		};

		class CWorld
		{
		public:
			CWorld();
			void LoadConfigurations();

		public:
			void Tick(float td);

			void Register(CRigidBody& rb) const;
			void Unregister(CRigidBody& rb) const;

			PFloat GravityY() const { return gravity_.y; }
			PVec3 Gravity() const;

			PFloat FrictionalForce(EMaterial going, EMaterial contacting, bool isKinetic) const;

			// runtime datas
		protected:
			mutable CVector<std::reference_wrapper<CRigidBody>> rigidBodies_;	// mutable for Register, Unregister

			// configurations
		protected:
			PVec3 gravity_{ 0. };
			std::array< std::array< std::array<PFloat, 2>, EMaterial::MAX>, EMaterial::MAX> frictionalForceTable_;
		};

		extern CWorld GWorld;
	}
};
